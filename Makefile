all:
	gcc -g -Wall utbrowser.c main.c `pkg-config --cflags --libs gumbo` `pkg-config --cflags --libs cairo` `sdl2-config --cflags` `sdl2-config --libs` `pkg-config --cflags --libs pangocairo` -o utbrowser
