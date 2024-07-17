## Bed Types
OrcaSlicer supports rectangular, circular, and custom shaped (via an imported `.stl`) beds. 

## Build Plate Types
Differnet biuld plate types can be enabled in printer settings. Once enabled, a build plate can be selectedin the drop-down menu, and corresponding bed temperature will be set automatically. You can set the bed temperature for each bed type in the filament settings as demonstrated in the following animation.  
![multi_bed](./images/bed-types.gif)  


OrcaSlicer also supports `curr_bed_type` variable in custom G-code. 
For example, the following sample G-codes can detect the selected bed type and adjust the G-code offset accordingly for Klipper:  
```
{if curr_bed_type=="Textured PEI Plate"}
  SET_GCODE_OFFSET Z=-0.05
{else}
  SET_GCODE_OFFSET Z=0.0
{endif}
```

available bed types are:
```
"Cool Plate"
"Engineering Plate"
"Smooth PEI Plate / High Temp Plate"
"Textured PEI Plate"
```
> [!TIP]
> If you are printing with an effect sheet/plate, you should select `Smooth PEI Plate / High Temp Plate`. 

## Nozzle Types
Nozzle widths can be changed using the dropdown under `Printer`. Supported nozzle widths are `0.2`, `0.4`, `0.6`, and `0.8` mm. Some printers may not have profiles for all of them, and will need a custom profile to print in different nozzle widths. `0.15` and `0.25` mm mozzle widths will also need custom profiles. 

