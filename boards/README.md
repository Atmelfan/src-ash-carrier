# Board support module

The software can support many different boards (as long as their MCU is supported by libopencm3) and configurations.  
See [Adding support for a new board](#adding-support-for-a-new-board) on how to support a new/custom board.

## Supported boards

* [gpa-ash-carrier](www.github.com/Atmelfan/pcb-ash-carrier) | `-DBOARD=gpa-ash-carrier`  
    My custom board. Uses I2C servo drivers or CAN.

* [1bitsy](www.1bitsy.org) | `-DBOARD=1bitsy`  
    1Bitsy Development board. Bitbangs PWM signals for up to 18 servos. 

Specify the desired board during the build process with "`cmake -DBOARD=<board-name> .`". By default `gpa-ash-carrier` is used.
    

## Adding support for a new board

Here's is a basic template for adding support for a new board:

```cpp
void board_init(){
    /* Init */
}


void board_update(body_t* body, gait_t* gait, legs_t* legs, uint8_t c){
    
    /* ... Modify body and/or gait */   
    
    /* Calculates the position of each leg */ 
    legs_update(legs, body->model, gait);
    
    /* ... Modify leg positions */
    
    /* Update servos for leg[i] */
    for(int i = 0; i < c; i++) {
        uint16_t buf[3];
        ik_dof3_calc(legs[i], &buf, 3);
        /* Update servos for leg[i] from buf[0..2]*/
    }
    
}
```
For more information and examples, refer to already implemented boards.


