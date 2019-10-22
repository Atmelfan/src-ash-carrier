# src-ash-carrier

Sourcecode for the carrier on my octapod.


## Architecture

* /  
    Contains the main code.

* boards/<boardname>  
    Contains support code for <boardname>. Implements all interfaces between the main code and the outide world.

* gait/  
    Contains code related to gait and gait definitions `<gaitname>.gait.dtsi`.
    
* math/  
    Linear algebra library, provides support for vectors and matrices 
    (Mostly 1x4 vectors and 4x4 matrices).

* scpi/  
    SCPI command parser, not very compliant with the SCPI standard but uses 
    the same syntax, structure etc.
    
