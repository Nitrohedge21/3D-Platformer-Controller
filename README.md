# 3D-Platformer-Controller
My poor attempt at recreating Sonic's 3D Gameplay in UE5.

18.02.2023 - Sonic Controller v0.3
- Jump dash v2, still needs adjustments but it works nicely.
- Jump dash is no longer able to be spammed.
- Fixed the camera clipping issue.
- Cleaned up the code by removing unused and unnecessary lines.
- Fixed the stomp being able to be used while mid air without jumping previously.
- Boost works the way as I intended but might need few minor adjustments.
- Added support for shaders(?), my personal project uses cel shading so it had to be done.
- Added a raycast line that checks if there is an object in front of Sonic, this will be used for 360 degree movement.
- Added a stamina system so that the boost can not be used infinitely.

16.12.2022 - Sonic Controller v0.2
- Added Jump Dash [WIP] (I can't figure out how to use multiple inputs for actions)
- Been trying to find how to check if the player jumped but no luck so far.
- Boost is still not good but the base is there.
- Tried to tweak a bunch of things in the code, based on the feedback I've received.

5.12.2022 - Sonic Controller v0.1
- Basic movement.
- Early boosting mechanic. Sonic is quite hard to control, gotta take a look at acceleration.
- Stomping mechanic.
- Will add jump dash as soon as I figure out how to use multiple inputs for actions.
