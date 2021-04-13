# Sailing
![Alt text](screenshot.png?raw=true "Screenshot")

Compile & Run:
    cs-488 framework is modified, so
    
    $ cd cs488
    $ premake4 gmake
    $ make
    
    $ cd SAILING
    $ premake4 gmake
    $ make
    
    $ ./SAILING Assets/scene.lua
    
Note:  
    Requries cs488 framework to compile (provided).  
    
Objectives:
    
  Completed:
    
    1) Scene modeling
    
    2) Texture mapping
    
    4) Water transparency:
        Actually a bit harder than a single alpha value,
        because there are multiple layers rendered onto water surface.
        
    5) Water reflection
    
    7) Water wave: 
        Distortion effect only; couldn't get dynamic normal map.
        
  Failed:
    
    3) Shadow mapping:
        Couldn't solve the precision problem of depth.
        
    6) Refraction:
        Tried multiple approaches, and none of them works.
        
        Adjusting the camera angle makes the scene fractured from the water surface.
        
        Actually this layer is still rendered onto water surface, it just doesn't
          redirect the ray.
        
    8) Smoke:
        No time left.
        
    9) Blur effect & 
    
    10 ) Collision detection:
        Easy, but at this moment my brain is already damaged.
        
User Manual:

  Control:
    
    Q: quit
    M: hide/show UI
    
    (keep pressing)
    W: accelerate the ship
    S: accelerate in the reverse direction
    A: turn left
    D: turn right
    
    Right-click the mouse and drag it to rotate the camera.
    Scroll the mouse wheel to zoom in/out.
    
    Basically controlling the ship is like driving a vehicle.
    
    Sliders to control the level of transparency, reflection and wave.
    I couldn't make transparency and reflection independent though....
    
    
Implementation
    
Code  ( with code reuse from assignment 3 ):
  
    Sailing:
        Main control loop. Extends cs488 framework.        
        
    Loader:
        Load the Assets when the program starts.
        
    Renderer:
        Render the scene at each frame. 
        Multiple passes: 
        reflection => refraction (rendered but rays are not directed)
        => scene ( transparency, distortion )
        
        
    FrameBuffers:
        Prepare various framebufferobjects, textures, as intermediate
        rendering targets.
        
    Ship:
        Control the modelling logic of the ship, like turning and movement.
        
    Camera:
        Control the camera and provide relevant view matrix.
        
    cs488-framework:
        Modified meshConsolidator to import UV maps for texture mapping.
        
Tools & materials:
    
    All modellings, and corresponding .obj files are prepared in Blender 2.78a. 
    (see https://www.blender.org/ )
    
    Terrain texture is also painted in Blender.
    
    Material images come from https://freestocktextures.com/texture/
    
