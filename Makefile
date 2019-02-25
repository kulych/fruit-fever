ff: primitives.o hitbox.o ff.o
	g++ -o ff hitbox.o primitives.o ff.o -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio
ff.o: ff.cpp
	g++ -c ff.cpp -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio
hitbox.o: hitbox.hpp hitbox.cpp
	g++ -c hitbox.cpp -lsfml-graphics -lsfml-window -lsfml-system
primitives.o: hitbox.hpp primitives.cpp
	g++ -c primitives.cpp -lsfml-graphics -lsfml-window -lsfml-system 
clean:
	rm hitbox.o primitives.o ff.o ff

test: test.o primitives.o hitbox.o
	g++ -o test test.o primitives.o hitbox.o -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio
test.o: test.cpp
	g++ -c test.cpp -lsfml-graphics -lsfml-window -lsfml-system 
	
