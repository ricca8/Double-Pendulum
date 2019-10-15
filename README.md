Here’s a simulation of some double pendulums movement, using threads (one each pendulum, one for keyboard and one for graphic) synchronized with mutexes. It's been made as a project for my Real-Time sync project at University.



To run it, just execute ./main in a terminal window. 
If it doesn't work, probably you're missing some libraries (I guess libGL), so read below at point 5.




Remember:

1- The number of pendulums is up to you (max 9), just modify the Data.txt file as explained there.

2- It runs with allegro 5, so if you got version 4 you should install this one as well to try the simulator -> on ubuntu use, in order,

	"sudo add-apt-repository ppa:allegro/5.2"
	
	"sudo apt-get update"
	
	"sudo apt-get install liballegro5-dev" 
	
3- The INC.ttf file is a font file, if you delete it, you will surely have problems.

4- CMake stuff is created by CLion on Mac OS, so it’s useless on Linux, where….

5- … to use the simulator navigate in that directory through terminal and execute “make” and then "./main" (if it says it already compiled, execute "make clean" before)







Files description (open them to get details):



Data.txt = contains the modifiable initial data of pendulum, as well as the number we want to display

macros.h = contains all the shared structures needed by the program to run and the libraries that are to be 		imported by every other file are here as every file includes macros.h.

init.c = reads Data.txt and initializes things as the display and some variable

elaboration.c = makes calculations of pendulums' movements

keyboard_manager.c = just manages keyboard inputs

graphic.c = handles all graphic stuff, drawing pendulums, their trails and the grill to contain them, as well as
	the background

main.c = creates all threads and makes everything start to run








Dev: Riccardo Migliori
