The navmesh currently is 2048x2048

Steps for creating navmesh:
1. Create white/black jpeg - white is obstacles, black is walkable path

2. Convert the jpeg to ai file - https://convertio.co/jpg-ai/

3. Drag the ai file to 3ds max - navmesh.max

4. In the max file there are 4 objects - one 2048x2048 plane for reference
and the 3 parts of the navmesh - central mesh and 2 side meshes
Use this 4 objects for reference - size and positions. If the new navmesh differs from these 4 it may not work in the game - the z coordinate is very important

5. Use extrude with amount 0.3

6. The z of the shapes must be -90

7. Use boolean tool from compound objects as you have selected the main shape.
Select the small meshes as operand B and get rid of them 

8. Export as .obj. Flip YZ must be selected.