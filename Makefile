all: primitives.o hitbox.o ff.o
	g++ -o ff hitbox.o primitives.o ff.o -lsfml-graphics -lsfml-window -lsfml-system
ff.o: ff.cpp
	g++ -c ff.cpp -lsfml-graphics -lsfml-window -lsfml-system
hitbox.o: hitbox.hpp hitbox.cpp
	g++ -c hitbox.cpp -lsfml-graphics -lsfml-window -lsfml-system
primitives.o: hitbox.hpp primitives.cpp
	g++ -c primitives.cpp -lsfml-graphics -lsfml-window -lsfml-system
