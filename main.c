#include <libopencm3/cm3/nvic.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/timer.h>
#include <stdio.h>
#include <assert.h>
#include <math.h>
#include <libopencm3/stm32/usart.h>
#include <string.h>
#include "math/linalg.h"
#include "fdt/fdt_parser.h"
#include "leg.h"
#include "body.h"
#include "math/linalg_util.h"
#include "gait/gait.h"
#include "scpi/scpi.h"
#include "boards/board.h"

#define MAX_NUM_APPENDAGES 64

#define RECV_BUF_SIZE 128

/* FDT Linked binary */
extern fdt_header_t octapod;

/* Body state */
body_t body;
leg_t* legs = NULL;
gait_target* current_targets = NULL;
gait_t current_gait;
uint8_t  gait_index = 0;


gait_step test_gait[] = {
        {
                .raise = {true, false, false, true, true, false, false, true}
        },
        {
                .raise = {false, true, true, false, false, true, true, false}
        }
};


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
    gait->phi = 0;
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
    gait->phi = da;

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
                targets[k].tangle = gait->phi * (targets[k].raise ? 1 : -1);
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
    body_reset(&body);
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
                mat4_make_rotz((float) ((a / 180.0f) * M_PI), &temp);//YAW
                break;
            case 2:
                mat4_make_rotx((float) ((a / 180.0f) * M_PI), &temp);//PITCH
                break;
            case 3:
                mat4_make_roty((float) ((a / 180.0f) * M_PI), &temp);//ROLL
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
    start_gait(&current_gait, &test_gait[0], current_targets, &v, angle);
    return SCPI_SUCCESS;
}

scpi_status_t scpi_gait_stop_query(const scpi_context_t *context, char *args){
    if(active_gait(&current_gait)){
        uart2_send("1\n");
    }else{
        uart2_send("0\n");
    }
    return SCPI_SUCCESS;
}

scpi_status_t scpi_gait_stop(const scpi_context_t *context, char *args){
    stop_gait(&current_gait);
    return SCPI_SUCCESS;
}

scpi_context_t scpi = {
        .root = {
                .name = ".",
                .sub = (scpi_command_t[]){
                        {
                                .name = "*IDN",
                                .set = NULL,
                                .get = NULL
                        },
                        {
                                .name = "*RST",
                                .set = NULL,
                                .get = NULL
                        },
                        {
                                .name = "*OPC",
                                .set = NULL,
                                .get = NULL
                        },
                        {
                                .name = "BODy",
                                .sub = (scpi_command_t[]){
                                        {
                                                .name = "RESet",
                                                .set = body_mat_reset,
                                                .get = NULL
                                        },
                                        {
                                                .name = "ROTate",
                                                .set = body_rot_set,
                                                .get = NULL
                                        },
                                        {
                                                .name = "TRAnslate",
                                                .set = body_tra_set,
                                                .get = NULL
                                        },
                                        SCPI_END_LIST
                                }

                        },
                        {
                                .name = "GAIt",
                                .sub = (scpi_command_t[]){
                                        {
                                                .name = "STOp",
                                                .set = scpi_gait_stop,
                                                .get = scpi_gait_stop_query
                                        },
                                        {
                                                .name = "STArt",
                                                .set = scpi_gait_start,
                                                .get = NULL
                                        },
                                        {
                                                .name = "STAtus",
                                                .set = NULL,
                                                .get = NULL
                                        },
                                        SCPI_END_LIST
                                },
                        },
                        {
                            .name = NULL,

                        },
                        SCPI_END_LIST
                },
        }
};





int main(void)
{

    board_init();

    printf("Hello world!\n");

    /*  */
    fdt_header_t* fdt = &octapod;
    fdt_token* root = fdt_get_tokens(fdt);

    /* Read legs  */
    fdt_token* legs_node = fdt_find_subnode(fdt, root, "legs");
    uint32_t num_legs = fdt_node_get_u32(fdt, legs_node, "#num-legs", 0);
    uint32_t servo_scale = fdt_node_get_u32(fdt, legs_node, "servo-scale", 0);

    if(!num_legs || (num_legs >= MAX_NUM_APPENDAGES)){
        printf("property '#num-legs' missing or out of range\n");
        //assert(false && "Property '#num-legs' missing or 0");
    }else{
        /* Allocate appendages*/
        legs = malloc(sizeof(leg_t)*num_legs);
        current_targets = malloc(sizeof(gait_target)*num_legs);
        assert(legs && current_targets && "Out of memory");

        /* Initialize appendages */
        for(int j = 0; j < num_legs; ++j){
            leg_init(&legs[j], servo_scale);
        }

        /*Read appendages from FDT*/
        for(fdt_token* l = fdt_token_next(fdt, legs_node); fdt_token_get_type(l) != FDT_END_NODE ; l = fdt_token_next(fdt, l)){
            /*Check for nodes*/
            if(fdt_token_get_type(l) == FDT_BEGIN_NODE){
                //printf("-> %s:\n", fdt_trace(fdt, l, buffer));

                uint32_t reg = fdt_node_get_u32(fdt, l, "reg", num_legs);

                /* Check leg index within range*/
                if(reg >= num_legs){
                    l = fdt_node_end(fdt, l);
                    continue;
                }

                /* Read leg parameters*/
                if(!leg_from_node(&legs[reg], fdt, l)){
                    l = fdt_node_end(fdt, l);
                    continue;
                }

                vec4 s = legs[reg].home_position;
                printf("home at %f, %f, %f\n", s.members[0], s.members[1], s.members[2]);

                /*Exit node*/
                l = fdt_node_end(fdt, l);
            }
        }
    }
    printf("ready!\n");

    //TODO: (in order) gait, commands+remote, light

    /* Get application parameters node */
    fdt_token* chosen = fdt_find_subnode(fdt, root, "chosen");

    /* Timekeeping */
    uint32_t last = 0, dt = 0, now = 0, t = 0;

    /* Body struct keeps track of body rotation and translation */
    body_init(&body);
    init_gait(&current_gait, current_targets);

    /*TODO: Gait code modularization */
    mat4 gait_mat = MAT4_IDENT();

    while(true){

        /* Calculate delta-t */
        now = board_systick();
        dt = now - last;

        if(active_gait(&current_gait)){
            if((t + dt < 2500)){
                t = t + dt;

            }else{
                t = 0;
                gait_index++;
                if(gait_index > 1)
                    gait_index = 0;
                next_gait(&current_gait, &test_gait[gait_index], current_targets);
            }
        }

        /* Update legs */
        for(int i = 0; i < num_legs; ++i){
            vec4 l = legs[i].home_position;
            vec4 tmp = VEC4(0, 0, 0, 1), target;

            target = l;

            /* TODO:  and translate with gait */
            //mat4 gait_model = MAT4_IDENT();
            //vecmat_mul((matxx*)&gait_model, (vecx*)&target, (vecx*)&tmp);
            //target = tmp;
            if(active_gait(&current_gait)){
                vec4 c = VEC4_ZERO();
                vec_sub((vecx *) &current_targets[i].target, (vecx *) &current_targets[i].initial, (vecx *) &c);
                vec_scalel((vecx *)&c, t / 2500.0f);
                vec_addl((vecx *) &c, (vecx *) &current_targets[i].initial);

                c.members[2] += 100.0f*sinf(3.14f*t/2500.0f) * (current_targets[i].raise ? 1 : 0);
                mat4_make_rotz(current_targets[i].iangle + (current_targets[i].tangle - current_targets[i].iangle) * t / 2500.0f,
                               &gait_mat);
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
            scpi_execute(&scpi, (char*)recv_buf, NULL);
            usart_send_blocking(USART2, '\n');
            recv_complete = 0;
        }

        /* Save last time */
        last = now;
    }

    return 0;
}