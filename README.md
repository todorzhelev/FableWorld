FableWorld
==========

FableWorld is 3D game, created with DirectX 9.0c and C++.<br />
![alt tag](http://i.imgur.com/AahcNAz.jpg)
<br/> <br />
Main features of the game<br />
====
  Battle system<br />
===
  The player can attack enemies in the game by walking close to the enemy and pressing left mouse button<br /><br />
  ![alt tag](http://i.imgur.com/VWn3NxV.jpg)
  ![alt tag](http://i.imgur.com/65Fix8w.jpg)
<br />
  Dialogue system<br />
===
  The player can communicate with entities, which have dialogue attached to them.<br />
  The dialogue consist of multiple choices for the player and each choice can lead to different result.<br />
  The dialogues are stored in xml files and are loaded in the game with tinyxml.<br /><br />
  ![alt tag](http://i.imgur.com/7IwLeUM.jpg)
  <br />
  Terrain<br />
===
  Different terrains can be created, based on the passed heightmap, which can be easily edited in Photoshop.<br />
  The player character moves according to the heights of the terrain.<br />
  The game supports terrains with up to 1 million vertices.<br /><br />
  Skinned and static models<br />
===
  For loading the models in the game is used the .x file format. <br />
  The game supports playing animations of the skinned models. <br /><br />
  Scripts<br />
===
  All the models in the scene are loaded from level file, which is written in lua.<br />
  The level file is exported from the Level editor. <br /><br />
  Custom UI system<br />
===
  The UI system in the game consists of buttons, labels, textboxes and checkboxes<br /><br />
  
