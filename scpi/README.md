# SCPI commands
The software can use commands passed over `run_command(...)` to control the body and gait. 

The command palette may be extended with custom commands using `scpi_reg_command(...)` in a board module.

## Command syntax
The command parser tries to follow SCPI syntax where possible.

### Get/set
Each command can have a get or set operation (or none).

- A get command is done by appending a question-mark to the end of the command, for example `*IDN?`.

- A set command does not end with a question-mark and can take zero or more arguments, 
for example `*RST` or `GAIT:BEGin arg1,arg2,arg3`. All arguments after the first should be separated by a comma `,`.

### Long and short form 
All commands have a long and short form, for example the command `BEGin` can be accessed with both `BEG` and `BEGIN`. 
Commands are also case-insensitive, command name case only shows their long/short form.

### Subcommands
Each command may have zero or more sub-commands.

To access a sub-command you specify its path (`:cmd:subcmd:subsubcmd`) with colon as divider. 
The leading colon is optional and is makes no difference for a single command.

Multiple command may be used by seperating them with a semicolon `;`. 

When using a subcommand `sub1`of command `cmd` 
subsequent commands will be assumed to be subcommands of `cmd`, so `cmd:sub1; sub2 == :cmd:sub1; :cmd:sub2`. 

To avoid this, commands can use the leading colon `:` to specify their absolute path, `cmd:sub1; :sub2 == :cmd:sub1; :sub2`.

Specials commands (starts with `*`) are an exception and is not affected by preceding commands nor affect commands after them.
Example, the command below:
```
gait:beg test; stat?; :BODY:TRANSLATE 0,0,0; *IDN?; rotate 0,0,0; :battery?
```
Gets parsed into the following command sequence:
```
:GAIT:BEGin test
:GAIT:STATus?
:BODY:TRAnslate 0,0,0
*IDN?
:BODY:ROTate 0,0,0
:BATtery:VOLTage?
```


## Default SCPI commands:

- `*IDN` Get identity
    - _?_  
     Returns a string of the form "`gpa-ash-carrier, $VERSION$, $BOARD$, $USER$`".
    
- `*RST` Reset
    - _(no args)_  
    Resets all states to their defaults (power on) states
    
- `*OPC`
    - _?_  
    Rerurns an empty string, useful to indicate that all commands have been executed.
    
- `BATtery`
    - `[VOLTage]`
        - _?_  
        Returns battery voltage in volts (if supported by hardware)
        
    - `PERCent`
        - _?_  
        Returns remaining charge in percent (if supported by hardware)

- `GAIT`
    - `BEGin`
        - _name_  
        Request to start gait with $name. Does nothing if a gait is already active.
        
    - `STATus`
        - _?_  
        Returns name of gait if currently active, otherwise empty string. 
        
    - `END`
        - _(no args)_  
        Request to stop current gait
        
    - `POSitions`
        - _?_  
        Returns all leg positions as `x1,y1,z1,...xN,yN,zN`.[^1]
        
- `BODY`
    - `TRAnslate`
        - _x,y,z_  
        Translates (shifts) the body by (x,y,z). [^note]
        
    - `ROTate`
        - _a,b,c_
        Rotates the body around (x,y,z) by (a,b,c) in degrees in that order. [^note]
        
    - `RESet`
        - _(no args)_  
        Resets body rotation and translation to zero.
        
    -  `MATrix`
        - _?_  
        Returns the current body model 4x4 matrix (row,column) as `m10,m11,..,m33`.
                
 [^1]: Only returns gait/home positions. Each position must be multiplied by the body model matrix to get their real position. 
 
 [^note]: Translation and rotation commands modifies the body model matrix without resetting it.
 The order of translate/rotate commands affects the resulting body position.
 
 The following will rotate the body by 45 degrees and shift forward by 40 units, placing the center of the robot at (40,0)

```
body:rotate 0,0,45; translate 40,0,0
```
Instead, the following will shift the body by 40mm before rotating the entire state by 45 degrees, placing the center of the robot at (28, 28)
```
body:translate 40,0,0; rotate 0,0,45
```
Following translate/rotate continues to modify the resulting position (This allows for some interesting movements).
As such, use `BODY:RESet` to reset the state before issuing multiple translate/rotate commands. 

## Custom commands
See _boards/board-name_ for any custom commands they may implement.