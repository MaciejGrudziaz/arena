THE CONTENT OF THE CATALOGUES:
  HEADERS 			- header files
  SRC 				- source files
  LOGS 				- log files [if created in the game]
  MODELS 			- 3D models used in game
  RESOURCES 			- all remaining resources files used in the game
 
HOW TO BUILD:
  1. To build this project you'll need Windows 10 SDK toolkit [best if you're using Microsoft Visual Studio].
  2. In the programming environment:
	- you have to add the HEADERS catalogue to the search path for include files
	- you have to additionally include libraries 'opengl32.lib', 'glu32.lib', 'dinput8.lib' and 'dxguid.lib' when building this project
  3. LOGS, MODELS and RESOURCES catalogues have to be in the same catalogue as the output .exe file of the built game.
	
GAME CONTROLS:
  W, S, A, D 			- movement
  SPACE 			- jump
  MOUSE 			- orientation
  LEFT MOUSE BUTTON 		- shoot
  R 				- reload
  TAB 				- toggle between fullscreen and windowed version
  ESC				- quit the game
  ~ 				- toggle the ingame console on/off
	
CONSOLE COMMANDS:
  show.fps=1/0 			- show/hide FPS value 
  show.input.time=1/0 		- show/hide time of execution of the input module [in microseconds]
  show.physics.time=1/0		- show/hide time of execution of the physics module [in microseconds]
  show.graphics.time=1/0	- show/hide time of execution of the graphics module [in microseconds]
  show.colbox=1/0		- show/hide hit boxes ['collision boxes'] of all ingame characters
  show.timer=1/0		- start and show/stop and hide an ingame timer
	
