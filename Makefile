all:
	gcc main.c -o dijkstra `sdl2-config --cflags --libs` -lSDL2_ttf

clean:
	rm dijkstra
