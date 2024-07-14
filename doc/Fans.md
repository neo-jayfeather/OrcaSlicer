# Auxiliary Fan
OrcaSlicer uses the `M106 P2` command to control auxiliary cooling fan.

Auxiliary fans are not attached to printheads, and are used to cool parts, but used in addition to a part cooling fan, which may be mounted on the printhead or connected via a duct. BBL X1C and P1S printers have auxiliary fans found on the left side of the chassis. 

> ## Klipper <br>
> If you are using Klipper, you can define a `M106` macro to control the both normal part cooling fan and auxiliary fan and exhaust fan. Below is a reference configuration for Klipper.   

> [!NOTE]
> Don't forget to change the pin name to the actual pin name you are using in the configuration.

```
# instead of using [fan], we define the default part cooling fan with [fan_generic] here
# this is the default part cooling fan
[fan_generic fan0]
pin: PA7
cycle_time: 0.01
hardware_pwm: false

# this is the auxiliary fan
# comment out it if you don't have auxiliary fan
[fan_generic fan2]
pin: PA8
cycle_time: 0.01
hardware_pwm: false

# this is the exhaust fan
# comment out it if you don't have exhaust fan
[fan_generic fan3]
pin: PA9
cycle_time: 0.01
hardware_pwm: false

[gcode_macro M106]
gcode:
    {% set fan = 'fan' + (params.P|int if params.P is defined else 0)|string %}
    {% set speed = (params.S|float / 255 if params.S is defined else 1.0) %}
    SET_FAN_SPEED FAN={fan} SPEED={speed}

```

# Part Cooling Fan
OrcaSlicer uses the `M106 P1?` command to control the fan.
# Mainboard Fan
OrcaSlicer uses the `M106 p
# Additional Fans
OrcaSlicer can use the M106 command to control any of the additional fans you have, if you add them into the custom G-Code sections. If you want to control your motor board for BBL printers, look below.

## Advanced
If you are writing custom G-Code, the `M106` command takes two inputs. the `P`, `S`, and optional `I` (Marlin 2.0.6 or higher), or `T` (Requires `EXTRA_FAN_SPEED`) value.  

> [!NOTE]
> Setting no speed, and only indicating the fan index will set the fan speed to its maximum.


> ### Marlin   
> `M106 I[X] P[Y] S[Z] T[A]-[B]`  
> `[X]` is your material preset index (preset fan speeds for certain materials)  
> `[Y]` is your fan index  
> `[Z]` is your fan speed if specified, determiend by fan duty cycle, controlled by your onboard pwm controller  
> `[A]` is your secondary speed for your fan index  
> `[B]` is your secondary speed like `[Z]`  
An example would be `M106 I0 P2 T3-255` which would set fan 2 (your third fan [0,1,2]) to the speed of preset 0 and set the secondary speed for the fan to 255. `M106 P[Y] T2` will use the set secondary speed, and `M106 P[Y] T1` will restore the previous speed for that fan.


> ### Bambu
> `M106 P[X] S[Y]`
> `[X]` is your fan index
>     1 is part-cooling, 2 is auxiliary, 3 is chamber fan
> `[Y]` is your fan speed if specified, determiend by fan duty cycle, controlled by your onboard pwm controller
>
> `M710 A1 S[X]`
> `[X]` is your speed
> This turns on the MC board fan.

> [!NOTE]  
> The AP board's fan turns on automatically, usually when the hotend is heated. 
