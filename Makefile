http:
	gcc -Wall http.c -o http
clean:
	gcc -Wall http-clean.c `pkg-config --cflags --libs gumbo` -o cleantext
