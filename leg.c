//
// Created by atmelfan on 2017-09-25.
//

#include <stdio.h>
#include <math.h>
#include "leg.h"
#include "platforms/log.h"

bool leg_from_node(leg_t* l, fdt_header_t* fdt, fdt_token* node){
    if(fdt_token_get_type(node) != FDT_BEGIN_NODE)
        return false;

    /* Read transform matrix */
    fdt_token* matrix = fdt_node_get_prop(fdt, node, "transform", false);
    if(matrix == NULL){
        logd_printfs(LOG_ERROR, "no transform");
        return false;
    }
    l->transform = MAT4_ZERO();
    for(int i = 0; i < 16; ++i){
        l->transform.members[i] = ((int32_t)fdt_read_u32(&matrix->cells[i]))/1000.0f;
    }

    /* Read home vector */
    fdt_token* home = fdt_node_get_prop(fdt, node, "home", false);
    if(home == NULL){
        return false;
    }
    l->home_position = VEC4_ZERO();
    for(int i = 0; i < 3; ++i){
        l->home_position.members[i] = ((int32_t)fdt_read_u32(&home->cells[i]))/1000.0f;
    }
    l->home_position.members[3] = 1.0f;

    /* Read offset vector */
    fdt_token* offset = fdt_node_get_prop(fdt, node, "offset", false);
    l->offset_position = VEC4_ZERO();
    if(offset == NULL){
        for(int i = 0; i < 3; ++i){
            l->offset_position.members[i] = 0;
        }
    }else{
        for(int i = 0; i < 3; ++i){
            l->offset_position.members[i] = ((int32_t)fdt_read_u32(&offset->cells[i]))/1000.0f;
        }
    }

    l->offset_position.members[3] = 1.0f;

    /*  */
    fdt_token* ik = fdt_find_subnode(fdt, node, "inverse-kinematics");
    if(ik){

        /*  */
        fdt_token* servos = fdt_node_get_prop(fdt, ik, "servos", false);
        fdt_token* length = fdt_node_get_prop(fdt, ik, "length", false);
        fdt_token* invert = fdt_node_get_prop(fdt, ik, "invert", false);

        /*  */
        uint32_t servo_phandle = fdt_read_u32(&servos->cells[0]);
        pwm_dev_t* pca = (pwm_dev_t *) dev_find_device_phandle(servo_phandle);

        /*  */
        if(pca && length && fdt_prop_len(fdt, length) == 3*sizeof(uint32_t) ){
            for (int i = 0; i < 3; ++i) {

                /* Leg lengths */
                l->lengths[i] = fdt_read_u32(&length->cells[i]);

                /* Servo indices & offset */
                l->servo_index[i] = fdt_read_u32(&servos->cells[1 + i*2 + 0]);
                l->servo_offsets_100[i] = fdt_read_u32(&servos->cells[1 + i*2 + 1]);
            }

            /* Read inverted values */
            l->invert[0] =  l->invert[1] =  l->invert[2] = 0;
            if(invert){
                /* If invert token exist */
                if(fdt_prop_len(fdt, invert) == 0){
                    l->invert[0] =  l->invert[1] =  l->invert[2] = 1;
                }else{
                    for (int i = 0; i < fdt_prop_len(fdt, invert)/sizeof(uint32_t) && i < 3; ++i) {
                        l->invert[i] = (uint8_t)(fdt_read_u32(&invert->cells[i]) ? 1 : 0);
                    }
                }
            }

            l->pwm_dev = pca;
        }else{
            logd_printfs(LOG_ERROR, "no pwm driver or malformed ik data");
            return false;
        }
    }else{
        logd_printfs(LOG_ERROR, "no ik node");
        return false;
    }

    /*  */
    l->ready = true;
    return true;
}

void leg_init(leg_t *l, uint32_t scale) {
    /* Initialize leg */
    l->home_position = VEC4_ZERO();
    l->transform = MAT4_ZERO();
    l->scale = scale;
    l->pwm_dev = NULL;
    for (int j = 0; j < 3; ++j) {
        l->servo_offsets_100[j] = 0;
    }
    l->status = LEG_AT_HOME;
    l->ready = false;
}


/**
 * Set a servo to the requested angle
 * @param pca, pwm device
 * @param index, servo channel
 * @param degres_10, degrees scaled by 10
 * @param scale, number of us to turn 90 degrees
 */
void leg_set_servo(pwm_dev_t *pca, uint32_t index, int32_t degres_10, uint32_t scale) {
    uint32_t period = ((pwm_driver_t*)pca->dev.drv)->period;

    uint32_t us = (uint32_t) (1500 + (int32_t)(degres_10*scale)/900);

    if(us < 250)
        us = 250;
    else if(us > 2750)
        us = 2750;

    uint32_t counts = (us*period)/20000;

    set_pwm(pca, (uint16_t) index, 0, counts);
}

void leg_move_to_vec(leg_t* l, vec4* vec){
    vec4 s = VEC4_ZERO();
    vecmat_mul((matxx*)&l->transform, (vecx*)vec, (vecx*)&s);
    //vec4 t = VEC4_ZERO();
    //vec_add((vecx*)&s, (vecx*)&ik_appendages[reg].offset_position, (vecx*)&t);
    //logd_printf(LOG_DEBUG, "move to %f, %f, %f\n", s.members[0], s.members[1], s.members[2]);
    leg_move_to_local(l, &s);

}

void leg_move_to_local(leg_t* l, vec4* loc){
    if(!l->pwm_dev)
        return;

    float x = loc->members[0], y = loc->members[1], z = loc->members[2];

    /* Leg lengths */
    const float L1 = l->lengths[0]/10.0f;
    const float L2 = l->lengths[1]/10.0f;
    const float L3 = l->lengths[2]/10.0f;

    /*  */
    float d_sq = x*x + y*y;
    float d = sqrtf(d_sq) - L1;
    float v = sqrtf(d*d + z*z);
    float s1 = (v*v + L2*L2 - L3*L3)/(2*v*L2);
    float s2 = (L2*L2 + L3*L3 - v*v)/(2*L2*L3);

    /*  */
    float S0 = (1800.0f/(float)M_PI)*(atan2f(y, x));
    float S1 = (1800.0f/(float)M_PI)*(acosf(s1) + atan2f(z, d));
    float S2 = (1800.0f/(float)M_PI)*(acosf(s2));

    /*  */
    leg_set_servo(l->pwm_dev, l->servo_index[0], ( (int32_t)S0 - l->servo_offsets_100[0])  * (l->invert[0] ? -1 : 1), l->scale);
    leg_set_servo(l->pwm_dev, l->servo_index[1], ((int32_t)-S1 - l->servo_offsets_100[1])  * (l->invert[1] ? -1 : 1), l->scale);
    leg_set_servo(l->pwm_dev, l->servo_index[2], ( (int32_t)S2 - l->servo_offsets_100[2])  * (l->invert[2] ? -1 : 1), l->scale);
}

