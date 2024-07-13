# Single Extruder Multi-Material Printing in OrcaSlicer

## Introduction
In this guide, we will explain the parameters for single extruder multimaterial printing in OrcaSlicer. The whole process and parameters are same as PrusaSlicer as OrcaSlicer is based on PrusaSlicer (with features ported from Bambu Studio and Cura). OrcaSlicer has some additional features ande G-Code for BBL machines. 

## Ramming
*Note: This section does **not** apply to BBL printers, unless you are trying to use the AMS like an MMU.*  
Ramming is forming the tip of the filament by purging filament. Less ramming creates a short tip and more stringing, while more ramming creates a longer and thicker tip with less stringing. 

## Manual filament change
If your printer does not have multi-material/filament switching capabilities, you can still print multicolor prints using manual filament changes, where pauses will be added for manual filament changes. 
### Bambu Printers
This is done with the `M400 U1` command which requires user intervention to continue. 
### Marlin
This is done with the `M600` command which automatically pauses, moves the print head away, and retracts the filament. 
Note: this is **only** available if you have ``ADVANCED_PAUSE_FEATURE`` enabled in your ``Configuration_adv.h`` file when compiling, and requires an LCD controller for user input. 
### Klipper
