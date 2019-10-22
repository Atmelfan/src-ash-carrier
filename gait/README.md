# Gait


## Gait definitions (`*.gait.dtsi`)

Gait definitions as their name imply define different gaits (way of walking). 

The basic format is based on devicetrees and uses its compiler (hence `.dtsi`).

A gait definition must have:

- An entry node (`gait-name`)  
    Defines how to start the gait.  
    _Properties:_
    - `#leg-cells` **REQUIRED**  
        Must specify the number of legs required.
    - `start` **REQUIRED**  
        Must specify one or more start points. Will select one randomly when starting.
    - `exit` **REQUIRED**  
        Must specify one exit node.

- One or more steps nodes (`gait-name@X`)  
    Defines which legs to use during the step and which step to take next.  
    _Properties:_
    - `reg` *OPTIONAL*  
        Not used but the compiler will complain if not present.
    - `legs` **REQUIRED**  
        Specifies which legs to raise/stand on.
    - `is-exit` *OPTIONAL*/(**REQUIRED**)  
        Specified that gait may end here. Must be specified in at least one node.
    - `next` **REQUIRED**  
        Specifies which step node to use next. Must loop eventually.
    - `translate` *OPTIONAL*  
        Specifes amount to translate body with (defaults to 0,0,0).
    
- An exit node (`gait-name@exit`)  
    Defines how to stop the gait.  
    _Properties:_
    - None yet
     
Nodes may have a label (`label_name:`) to be refereed by but must have a unique name. 

```dts
/* Includes definitions and macros */
#include "gait.dtsi"

/{
    /* Gait entry point */
    gait-simple {
        #leg-cells = < 8 >; // Number of legs
        start = < &simple1 &simple2>; // Use one of these when starting'
        exit = < & simple_exit >; // Exit here
    };

    /* First step */
    simple1: gait-simple@simple1 {
        reg = < 1 >; // Not used, but DTC will complain if not present
        
        /* Specify which leg should be raised and which to stand on */
        legs = <
            RAISE    RAISE
            STAND    STAND
            RAISE    RAISE
            STAND    STAND
         >;
         
         is-exit; // Can stop at this node
         next = < &simple2 >; // Go to first step
    };

    /* Second step */
    simple2: gait-simple@simple2 {
        reg = < 2 >; // Not used, but DTC will complain if not present
        
        /* Specify which leg should be raised and which to stand on */
        legs = <
            STAND    STAND
            RAISE    RAISE
            STAND    STAND
            RAISE    RAISE
         >;
         
         is-exit; // Can stop at this node
         next = < &simple1 >; // Go to first step
    };
    
    simple_exit: gait-simple@exit {
        reg = < 3 >; // Not used, but DTC will complain if not present
    };
};
```

Gaits are registered by including them in the main config file and adding their entry point to th`/chosen/gaits` property.
