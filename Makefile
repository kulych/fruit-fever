ff: game.o primitives.o hitbox.o ff.o resources.o
	g++ -o ff game.o hitbox.o primitives.o ff.o resources.o -O2 -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio
ff.o: ff.cpp
	g++ -c ff.cpp -O2 -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio
hitbox.o: hitbox.hpp hitbox.cpp
	g++ -c hitbox.cpp -O2 -lsfml-graphics -lsfml-window -lsfml-system
primitives.o: hitbox.hpp primitives.cpp
	g++ -c primitives.cpp -O2 -lsfml-graphics -lsfml-window -lsfml-system 
resources.o: resources.hpp resources.cpp
	g++ -c resources.cpp -O2 -lsfml-graphics -lsfml-window -lsfml-system 
game.o: game.hpp game.cpp
	g++ -c game.cpp -O2 -lsfml-graphics -lsfml-window -lsfml-system 
	
clean:
	rm hitbox.o primitives.o ff.o resources.o game.o

test: test.o primitives.o hitbox.o resources.o
	g++ -o test test.o primitives.o hitbox.o resources.o -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio
test.o: test.cpp
	g++ -c test.cpp -lsfml-graphics -lsfml-window -lsfml-system 
	
