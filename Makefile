http:
	gcc -Wall http.c -o http
clean:
	gcc -g -Wall http-clean.c `pkg-config --cflags --libs gumbo` `pkg-config --cflags --libs cairo` `sdl2-config --cflags` `sdl2-config --libs` `pkg-config --cflags --libs pangocairo` -o cleantext
