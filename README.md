FableWorld
==========

FableWorld is 3D game, created with DirecX 9.0 and C++.
The main features of the game are:
1.Battle system
- the player can attack enemies in the game by walking close to the enemy and pressing left mouse button<br />
2.Dialogue system
- the player can communicate with entities, which have dialogue attached to them. 
The dialogue consist of multiple choices for the player and each choice can lead to different result.
The dialogues are stored in xml files and are loaded in the game with tinyxml.<br />
3.Terrain
-different terrains can be created, based on the passed heightmap, which can be easily edited in photoshop
The player character moves according to the heights of the terrain.
4.Scripts
-models, which must be loaded in the game are described in lua files.<br /> 
Every model attribute can be easily modified in the corresponding script file
5.Custom UI system
-the UI system in the game consists of Buttons and Labels
6.Skinned and static models
-for loading the models in the game is used the .x file format.
