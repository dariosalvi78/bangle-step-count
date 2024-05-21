all:
	gcc -std=c99 main.c -I . -o main

tuneEspruino:
	gcc -std=c99 tuneEspruino.c -I . -o tuneEspruino
