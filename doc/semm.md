# Single Extruder Multi-Material Printing in OrcaSlicer

## Introduction
This guide explains the parameters for single extruder multimaterial printing (SEMM) in OrcaSlicer. The whole process and parameters are same as PrusaSlicer, as OrcaSlicer is based on it and other slicers. OrcaSlicer has some additional features and G-Code for Bambu Lab machines. 

## Ramming
> [!NOTE] 
> This section does **not** apply to BBL printers, unless you are trying to use the AMS like an MMU.

Ramming is forming the tip of the filament by purging filament. Less ramming creates a short tip and more stringing, while more ramming creates a longer and thicker tip with less stringing. 

## Manual Filament Change
If your printer does not have multi-material/filament switching capabilities, you can still print multicolor prints using manual filament changes, where pauses will be added for manual filament changes.
To add a manual filament change, preview the gcode, and select the layer you would like it to occur on. 
Then, right click on the circle with a plus, and click "Add Pause."
If the pause G-Code is not what your printer accepts, use "Add Custom G-Code" to insert your own G-Code. 

### Bambu Printers
This is done with the `M400 U1` command which requires user intervention to continue. 
### Marlin
This is done with the `M600` command which automatically pauses, moves the print head away, and retracts the filament. 
> [!IMPORTANT]
> This is **only** available if you have ``ADVANCED_PAUSE_FEATURE`` enabled in your ``Configuration_adv.h`` file when compiling, and requires an LCD controller for user input. 
### Klipper
There are no native filament change/pause commands, but you can add a G-Code macro.  
<details>
<summary>M600 Pause Macro</summary>
  
```
[gcode_macro M600]
  gcode:
    {% set default_x = printer.toolhead.axis_minimum.x + 20 %}
    {% set default_y = printer.toolhead.axis_minimum.y + 20 %}
    {% set default_e = 100 %}
  
    {% if 'save_variables' in printer %}
      {% set svv = printer.save_variables.variables %}
      {% if 'park_x' in svv %}
        {% set default_x = svv.park_x %}
      {% endif %}
      {% if 'park_y' in svv %}
        {% set default_y = svv.park_y %}
      {% endif %}
      {% if 'bowden_len' in svv %}
        {% set default_e = svv.bowden_len %}
      {% endif %}
    {% endif %}
  
    {% set x = params.X|default(default_x)|float %}
    {% set y = params.Y|default(default_y)|float %}
    {% set z = params.Z|default(20)|float %}
    {% set e = params.E|default(default_e)|float %}
  
    {% if printer.pause_resume.is_paused %}
      {action_respond_info("Already paused")}
    {% elif printer.toolhead.homed_axes != "xyz" %}
      {action_respond_info("Please home XYZ first")}
    {% else %}
      PAUSE_PARK X={x} Y={y} Z={z}
      M702 U{e}
    {% endif %}
```
</details>

## Automatic Filament Change 
### Bambu Printers
This is done with the AMS (Lite) for Bambu printers, and is handled well in OrcaSlicer. Make sure you select the right Bambu printer and turn your prime tower on. 
### Marlin 
[wip]
### Klipper
[wip]
