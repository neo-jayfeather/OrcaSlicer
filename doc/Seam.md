## Description
Seams are where extrusions start and stop, and may create a visible line or small blob. Seams are mostly inevitable, but OrcaSlicer has some settings that make it more configurable, and potentially make them almost invisible. Seams are completely invisible in vase mode/spiral vase. They can also be hidden using sharp corners, but are harder to hide on cylindrical surfaces. 


### Seam position


### Scarf joint seam
Scarf joint seams follow woodworking scarf joints and use a sloping interface to help hide the seam. [finish]

### Seam gap
Seam gap is the gap between the start and stop of an extrusion for a single wall. It can be expressed as a % of the nozzle width, or in mm. 
A higher number will result in a less visible seam but may cause gaps, and a lower number will result in a more visible seam but less gaps. 
10% is a good starting point for regular and scarf seams. 
![image](https://user-images.githubusercontent.com/103989404/215331359-236874c9-ca91-4dd4-b969-3cefc70ebe1b.png)

### Role-based wipe speed(auto)
### Wipe speed
### Wipe on loop(inward movement)
![image](https://user-images.githubusercontent.com/103989404/215334479-2687c518-1931-46a9-a363-7ae57a0aecf6.png)

Use outer wall speed and acceleration instead of travel speed and acceleration.
Added an option to disable this feature
### Support Cura style outer wall wipe(100% retract before wipe)
![image](https://user-images.githubusercontent.com/103989404/215334900-97c75010-4752-45f1-a57c-f0b723a8cb3f.png)
![image](https://user-images.githubusercontent.com/103989404/215335212-3253061d-bb7b-4853-a86c-10ff54cc8df5.png)

Extra length on restart
