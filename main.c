#include <libopencm3/cm3/nvic.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/timer.h>
#include <stdio.h>
#include <assert.h>
#include <math.h>
#include <libopencm3/stm32/usart.h>
#include <string.h>
#include "linalg/linalg.h"
#include "fdt/dtb_parser.h"
#include "leg.h"
#include "platforms/board.h"
#include "platforms/i2c.h"
#include "platforms/log.h"
#include "body.h"
#include "linalg/linalg_util.h"
#include "gait/gait.h"
#include "scpi/scpi.h"

#define MAX_NUM_APPENDAGES 64

#define RECV_BUF_SIZE 128
#ifdef DEBUG
extern void initialise_monitor_handles(void);

#endif

/* FDT Linked binary */
extern fdt_header_t octapod;

/* Body state */
body_t body;
leg_t* legs = NULL;
gait_target* targets = NULL;
gait_t gait;
uint8_t  gait_index = 0;


gait_step test_gait[] = {
        {
                .raise = {true, false, false, true, true, false, false, true}
        },
        {
                .raise = {false, true, true, false, false, true, true, false}
        }
};


/* Set STM32 to 168 MHz. */
static void clock_setup(void)
{
    rcc_clock_setup_pll(&rcc_hse_25mhz_3v3[RCC_CLOCK_3V3_168MHZ]);
}

void sys_tick_handler(){
    //dev_systick();
}

// TODO: Remove this POS
/********************REMOVE********************/
uint8_t recv_buf[RECV_BUF_SIZE];
volatile int recv_ndx_nxt = 0, recv_complete = 0;
void usart2_isr(void)
{
    uint32_t	reg;
    int			i;

    do {
        reg = USART_SR(USART2);
        if (reg & USART_SR_RXNE) {
            recv_buf[recv_ndx_nxt] = (uint8_t) USART_DR(USART2);
            if(!recv_complete){
                if(recv_buf[recv_ndx_nxt] == '\r'){
                    recv_buf[recv_ndx_nxt] = '\0';
                    recv_complete = 1;
                    recv_ndx_nxt = 0;
                }else{
                    i = (recv_ndx_nxt + 1) % RECV_BUF_SIZE;
                    recv_ndx_nxt = i;
                }
            }


        }
    } while ((reg & USART_SR_RXNE) != 0); /* can read back-to-back interrupts */
}

void usart2_setup(){
    gpio_mode_setup(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO2 | GPIO3);

    /* Actual Alternate function number (in this case 7) is part
     * depenedent, CHECK THE DATA SHEET for the right number to
     * use.
     */
    gpio_set_af(GPIOA, GPIO_AF7, GPIO2 | GPIO3);


    /* This then enables the clock to the USART1 peripheral which is
     * attached inside the chip to the APB1 bus. Different peripherals
     * attach to different buses, and even some UARTS are attached to
     * APB1 and some to APB2, again the data sheet is useful here.
     */
    rcc_periph_clock_enable(RCC_USART2);

    /* Set up USART/UART parameters using the libopencm3 helper functions */
    usart_set_baudrate(USART2, 115200);
    usart_set_databits(USART2, 8);
    usart_set_stopbits(USART2, USART_STOPBITS_1);
    usart_set_mode(USART2, USART_MODE_TX_RX);
    usart_set_parity(USART2, USART_PARITY_NONE);
    usart_set_flow_control(USART2, USART_FLOWCONTROL_NONE);
    usart_enable(USART2);

    /* Enable interrupts from the USART */
    nvic_enable_irq(NVIC_USART2_IRQ);

    /* Specifically enable recieve interrupts */
    usart_enable_rx_interrupt(USART2);
}

void uart2_send(char* s){
    while(*s){
        usart_send_blocking(USART2, (uint16_t) *s);
        s++;
    }
}

/********************REMOVE********************/


void init_gait(gait_t *gait, gait_target *targets) {
    gait->state = GAIT_IDLE;
    gait->vec = VEC4_ZERO();
    gait->angle = 0;
    for (int k = 0; k < 8; ++k) {
        targets[k].initial = VEC4_ZERO();
        targets[k].target = VEC4_ZERO();
        targets[k].iangle = 0;
        targets[k].tangle = 0;
        targets[k].raise = false;

    }
}

void start_gait(gait_t* gait, gait_step *step, gait_target *targets, vec4* mov, float da){

    if(gait->state != GAIT_IDLE)
        return;

    gait->state = GAIT_RUNNING;
    gait->vec = *mov;
    gait->angle = da;

    /* Go to next */
    for (int k = 0; k < 8; ++k) {
        targets[k].target = *mov;
        targets[k].raise = step->raise[k];
        targets[k].tangle = da * (targets[k].raise ? 1 : -1);
        vec_scalel((vecx *)&targets[k].target, targets[k].raise ? 1 : -1);
    }
}

void stop_gait(gait_t *gait) {
    /* Request end of gait */
    if(gait->state == GAIT_RUNNING)
        gait->state = GAIT_ENDNOW;
}



void next_gait(gait_t *gait, gait_step *step, gait_target *targets){
    /* Gait is ending, do not update */

    switch (gait->state){
        case GAIT_RUNNING:
            /* Go to next */
            for (int k = 0; k < 8; ++k) {
                targets[k].initial = targets[k].target;
                targets[k].target = gait->vec;
                targets[k].iangle = targets[k].tangle;
                targets[k].raise = step->raise[k];
                targets[k].tangle = gait->angle * (targets[k].raise ? 1 : -1);
                vec_scalel((vecx *)&targets[k].target, targets[k].raise ? 1 : -1);
            }
            break;
        case GAIT_ENDNOW:
            /* Go home */
            for (int k = 0; k < 8; ++k) {
                targets[k].initial = targets[k].target;
                targets[k].target = VEC4_ZERO();
                targets[k].iangle = targets[k].tangle;
                targets[k].tangle = 0;
                targets[k].raise = step->raise[k];
                vec_scalel((vecx *)&targets[k].target, targets[k].raise ? 1 : -1);
            }
            gait->state = GAIT_FINAL;
            break;
        case GAIT_FINAL:
            gait->state = GAIT_IDLE;
            break;
        default:
            break;

    }

}

bool active_gait(gait_t* gait){
    return gait->state != GAIT_IDLE;
}

scpi_status_t body_mat_reset(const scpi_context_t *context, char *args){
    body.model = MAT4_IDENT();
    //logd_printfs(LOG_INFO, "rot\n");
    return SCPI_SUCCESS;
}

scpi_status_t body_rot_set(const scpi_context_t *context, char *args){
    char* end;
    char* token = strtok_r(args, " ", &end);
    //logd_printfs(LOG_INFO, "rot\n");
    for (int i = 0; token; ++i){
        mat4 temp = MAT4_IDENT();
        float a = atoff(token);
        if(a > 10)
            a = 10;
        else if(a < -10)
            a = -10;

        switch (i){
            case 1:
                mat4_rotz((float) ((a / 180.0f) * M_PI), &temp);//YAW
                break;
            case 2:
                mat4_rotx((float) ((a / 180.0f) * M_PI), &temp);//PITCH
                break;
            case 3:
                mat4_roty((float) ((a / 180.0f) * M_PI), &temp);//ROLL
                break;
            default:
                break;
        }
        mat_matmull((matxx *) &body.model, (matxx *) &temp);
        token = strtok_r(NULL, ",", &end);
    }
    return SCPI_SUCCESS;
}

scpi_status_t body_tra_set(const scpi_context_t *context, char *args){
    char* end;
    char* token = strtok_r(args, " ", &end);
    //logd_printfs(LOG_DEBUG, "--tra\n");
    mat4 temp = MAT4_IDENT();
    for (int i = 0; token; ++i){
        //logd_printf(LOG_DEBUG, "%s\n", token);
        float a = atoff(token);
        if(a > 40)
            a = 40;
        else if(a < -40)
            a = -40;
        switch (i){
            case 1:
                temp.MAT4_M(3,0) = -a;//YAW
                break;
            case 2:
                temp.MAT4_M(3,1) = -a;//PITCH
                break;
            case 3:
                temp.MAT4_M(3,2) = -a;//PITCH
                break;
            default:
                break;
        }

        token = strtok_r(NULL, ",", &end);
    }
    mat_matmull((matxx *) &body.model, (matxx *) &temp);
    return SCPI_SUCCESS;
}


scpi_status_t scpi_gait_start(const scpi_context_t *context, char *args){
    char* end;
    char* token = strtok_r(args, " ", &end);
    vec4 v = VEC4_ZERO();
    float angle = 0;
    for (int i = 0; token; ++i){
        //logd_printf(LOG_DEBUG, "%s\n", token);
        float a = atoff(token);
        if(a > 40)
            a = 40;
        else if(a < -40)
            a = -40;
        switch (i){
            case 1:
                v.members[0] = fmodf(-a, 30);//YAW

                break;
            case 2:
                v.members[1] = fmodf(-a, 30);//PITCH
                break;
            case 3:
                v.members[2] = fmodf(-a, 30);//PITCH
                break;
            case 4:
                angle = (float)(M_PI * fmodf(a, 7.5f) / 180.0f);
                break;
            default:
                break;
        }

        token = strtok_r(NULL, ",", &end);
    }
    start_gait(&gait, &test_gait[0], targets, &v, angle);
    return SCPI_SUCCESS;
}

scpi_status_t scpi_gait_stop_query(const scpi_context_t *context, char *args){
    if(active_gait(&gait)){
        uart2_send("1\n");
    }else{
        uart2_send("0\n");
    }
    return SCPI_SUCCESS;
}

scpi_status_t scpi_gait_stop(const scpi_context_t *context, char *args){
    stop_gait(&gait);
    return SCPI_SUCCESS;
}

/**
 * Set a servo to the requested angle
 * @param pca, pwm device
 * @param index, servo channel
 * @param degres_10, degrees scaled by 10
 * @param scale, number of us to turn 90 degrees
 */
void set_servo(pwm_dev_t *pca, uint32_t index, int32_t degres_10, uint32_t scale) {
    uint32_t period = ((pwm_driver_t*)pca->dev.drv)->period;

    uint32_t us = (uint32_t) (1500 + (int32_t)(degres_10*scale)/900);

    if(us < 250)
        us = 250;
    else if(us > 2750)
        us = 2750;

    uint32_t counts = (us*period)/20000;

    //TODO: Call board_set_servo()
    //set_pwm(pca, (uint16_t) index, 0, counts);
}





int main(void)
{

#ifdef DEBUG
    initialise_monitor_handles();
#endif
    clock_setup();
    //TODO: init board
    //jetson_batocp(false);

    printf("Hello world!\n");

    /*  */
    fdt_header_t* fdt = &octapod;
    fdt_token* root = fdt_get_tokens(fdt);
    fdt_token* bootmsg = fdt_node_get_prop(fdt, root, "bootmsg", false);
    printf("%s\n", bootmsg->prop_str);

    /*Override board init file with "/platform" node*/
    //board_init_fdt(fdt, fdt_find_subnode(fdt, root, "platform"));

    /* Read legs  */
    fdt_token* legs_node = fdt_find_subnode(fdt, root, "legs");
    uint32_t num_legs = fdt_node_get_u32(fdt, legs_node, "#num-legs", 0);
    uint32_t servo_scale = fdt_node_get_u32(fdt, legs_node, "servo-scale", 0);
    //logd_push("legs");
    if(!num_legs || (num_legs >= MAX_NUM_APPENDAGES)){
        //logd_printfs(LOG_ERROR, "property '#num-legs' missing or out of range\n");
        //assert(false && "Property '#num-legs' missing or 0");
    }else{
        /* Allocate appendages*/
        legs = malloc(sizeof(leg_t)*num_legs);
        targets = malloc(sizeof(gait_target)*num_legs);
        assert(legs && targets && "Out of memory");

        /* Initialize appendages */
        for(int j = 0; j < num_legs; ++j){
            leg_init(&legs[j], servo_scale);
        }

        /*Read appendages from FDT*/
        for(fdt_token* l = fdt_token_next(fdt, legs_node); fdt_token_get_type(l) != FDT_END_NODE ; l = fdt_token_next(fdt, l)){
            /*Check for nodes*/
            if(fdt_token_get_type(l) == FDT_BEGIN_NODE){
                //printf("-> %s:\n", fdt_trace(fdt, l, buffer));
                //logd_push(l->name);

                uint32_t reg = fdt_node_get_u32(fdt, l, "reg", num_legs);

                /* Check leg index within range*/
                if(reg >= num_legs){
                    //logd_printf(LOG_ERROR, "reg out of range", reg);
                    //logd_pop();
                    l = fdt_node_end(fdt, l);
                    continue;
                }

                /* Read leg parameters*/
                if(!leg_from_node(&legs[reg], fdt, l)){
                    //logd_pop();
                    l = fdt_node_end(fdt, l);
                    continue;
                }

                if(legs[reg].pwm_dev){
                    //leg_move_to_vec(&ik_appendages[reg], &ik_appendages[reg].home_position);
                    //vec4 s = legs[reg].home_position;
                    //logd_printf(LOG_DEBUG, "home at %f, %f, %f\n", s.members[0], s.members[1], s.members[2]);
                }else{
                    //logd_printfs(LOG_WARNING, "no servo driver\n");
                }

                /* Read ik parameters */
                //TODO: Remove this shit
                fdt_token* ik = fdt_find_subnode(fdt, l, "inverse-kinematics");
                if(ik){
                    fdt_token* test = fdt_node_get_prop(fdt, ik, "test", false);

                    if(test){
                        fdt_token* invert = fdt_node_get_prop(fdt, ik, "invert", false);
                        fdt_token* servos = fdt_node_get_prop(fdt, ik, "servos", false);

                        /* Find servo driver */
                        uint32_t servo_phandle = fdt_read_u32(&servos->cells[0]);
                        //TODO: use board_set_pwm?
                        //pwm_dev_t* pca = (pwm_dev_t *) dev_find_device_phandle(servo_phandle);

                        /*if(pca){
                            int32_t s[3];
                            for (int i = 0; i < 3; ++i) {
                                uint32_t index = fdt_read_u32(&servos->cells[1 + 2*i + 0]);
                                s[i] = ((int32_t)fdt_read_u32(&test->cells[i]) - (int32_t)fdt_read_u32(&servos->cells[1 + 2*i + 1])) * (invert ? -1 : 1);
                                set_servo(pca, index, (int32_t) s[i], servo_scale);
                            }
                            //logd_printf(LOG_DEBUG, "positioned to %d, %d, %d\n", s[0]/10, s[1]/10, s[2]/10);
                        }*/

                    }

                }else{
                    //logd_printfs(LOG_INFO, "node 'inverse-kinematics' missing\n");
                }

                /*Exit node*/
                //logd_pop();
                l = fdt_node_end(fdt, l);
            }
        }
    }
    //logd_pop();
    //logd_printfs(LOG_INFO, "ready!\n");

    while(!targets || !legs);

    //TODO: (in order) gait, commands+remote, light

    /* Get application parameters node */
    fdt_token* chosen = fdt_find_subnode(fdt, root, "chosen");

    /* Timekeeping */
    uint32_t last = 0, dt = 0, now = 0, t = 0, tboot = 0;

    /* Body struct keeps track of body rotation and translation */
    body_init(&body);
    init_gait(&gait, targets);

    /*Init uart */
    usart2_setup();

    /*TODO: Gait code modularization */
    mat4 gait_mat = MAT4_IDENT();

    while(true){

        /* Calculate delta-t */
        now = 0;// dev_systick_get();//TODO: replace with board_get_systick()
        dt = now - last;

        /* Raise from floor slowly */
        if(tboot < 5000)
            tboot += dt;

        //TODO: translate above floor
        //body.offset.members[2] = tboot/100.0f;


        if(active_gait(&gait)){
            if((t + dt < 2500)){
                t = t + dt;

            }else{
                t = 0;
                gait_index++;
                if(gait_index > 1)
                    gait_index = 0;
                next_gait(&gait, &test_gait[gait_index], targets);
            }
        }

        /* Update legs */
        for(int i = 0; i < num_legs; ++i){
            vec4 l = legs[i].home_position;
            vec4 tmp = VEC4(0, 0, 0, 1), target;

            /* Subtract body offset */
            //vec_sub((vecx *) &l, (vecx *) &body.offset, (vecx*) &tmp);
            target = tmp;

            /* TODO:  and translate with gait */
            //mat4 gait_model = MAT4_IDENT();
            //vecmat_mul((matxx*)&gait_model, (vecx*)&target, (vecx*)&tmp);
            //target = tmp;
            if(active_gait(&gait)){
                vec4 c = VEC4_ZERO();
                vec_sub((vecx *) &targets[i].target, (vecx *) &targets[i].initial, (vecx *) &c);
                vec_scalel((vecx *)&c, t / 2500.0f);
                vec_addl((vecx *) &c, (vecx *) &targets[i].initial);

                c.members[2] += 100.0f*sinf(3.14f*t/2500.0f) * (targets[i].raise ? 1 : 0);
                mat4_make_rotz(targets[i].iangle + (targets[i].tangle - targets[i].iangle)*t/2500.0f, &gait_mat);
                mat4_make_translate(&c, &gait_mat);
                vecmat_mul((matxx*)&gait_mat, (vecx*)&target, (vecx*)&tmp);
                target = tmp;
            }


            /* Rotate & translate all leg vectors with model matrix*/
            vecmat_mul((matxx*)&body.model, (vecx*)&target, (vecx*)&tmp);
            target = tmp;

            /* Move to target */
            leg_move_to_vec(&legs[i], &target);
        }

        if(recv_complete){
            //logd_printf(LOG_INFO, "> %s\n", recv_buf);
            //scpi_execute(&scpi, (char*)recv_buf, NULL);
            usart_send_blocking(USART2, '\n');
            recv_complete = 0;
        }

        /* Save last time */
        last = now;
    }

    return 0;
}