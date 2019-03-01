CC=g++
CFLAGS= -O2 -Wall -ggdb -lsfml-graphics -lsfml-window -lsfml-system

ff: game.o primitives.o hitbox.o ff.o resources.o
	$(CC) -o ff game.o hitbox.o primitives.o ff.o resources.o $(CFLAGS) -lsfml-audio
ff.o: ff.cpp
	$(CC) -c ff.cpp $(CFLAGS)
hitbox.o: hitbox.hpp hitbox.cpp
	$(CC) -c hitbox.cpp $(CFLAGS)
primitives.o: hitbox.hpp primitives.cpp
	$(CC) -c primitives.cpp $(CFLAGS)
resources.o: resources.hpp resources.cpp
	$(CC) -c resources.cpp $(CFLAGS)
game.o: game.hpp game.cpp
	$(CC) -c game.cpp $(CFLAGS) -lsfml-audio
clean:
	rm hitbox.o primitives.o ff.o resources.o game.o ff	
