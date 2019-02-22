all: hitbox ff
ff: ff.cpp
	g++ hitbox.cpp ff.cpp -o ff -lsfml-graphics -lsfml-window -lsfml-system
hitbox: hitbox.cpp
	g++ hitbox.cpp -o ff -lsfml-graphics -lsfml-window -lsfml-system
