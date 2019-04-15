FableWorld
==========

FableWorld is RTS game, created with DirectX 9.0c and C++.<br />
For the pathfinding Recast&Detour is used <br /> <br />
![alt tag](https://i.imgur.com/JpNOIw1.jpg)
<br/> <br />

  Battle system<br />
=======

  You can fight the enemies by first picking your unit and then move it to the enemy<br /><br />
  ![alt tag](https://i.imgur.com/zmYI1Z7.jpg)
  ![alt tag](https://i.imgur.com/0f2MEWX.jpg)
<br />

  Dialogue system<br />
=======
  The player can communicate with entities, which have dialogue attached to them.<br />
  The dialogue consist of multiple choices for the player and each choice can lead to different result.<br />
  The dialogues are stored in xml files and are loaded in the game with tinyxml.<br /><br />
  ![alt tag](http://i.imgur.com/7IwLeUM.jpg)
  <br />
  
   Particle system<br />
=======
  Currently it is used for the gun effect so you can shoot your opponents with spells<br /><br />
  ![alt tag](https://i.imgur.com/tf6L7k4.jpg)
  <br />
  
  Terrain<br />
=======

  Different terrains can be created, based on the passed heightmap, which can be easily edited in Photoshop.<br />
  The player character moves according to the heights of the terrain.<br />
  The game supports terrains with up to 1 million vertices.<br /><br />
  
  Skinned and static models<br />
=======

  For loading the models in the game is used the .x file format. <br />
  The game supports playing animations of the skinned models. <br /><br />
  
  Scripts<br />
=======
  All the models in the scene are loaded from level file, which is written in lua.<br />
  The level file is exported from the Level editor. <br /><br />
  
  Custom UI system<br />
=======
The UI system in the game consists of buttons, labels, textboxes and checkboxes<br /><br />
  
