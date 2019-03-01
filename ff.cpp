#include <SFML/Graphics.hpp>
#include "iostream"
#include "game.hpp"
#include "resources.hpp"

using namespace std;

//For debugging mode look for #DEBUG tags and follow given instructions


//Fills resource manager with multimedial data
//Outputs error if an loading error occures
//#OTAZKA Tenhle typ relativnich cest vyzaduje, aby se vysledna binarka
//spoustela z korenovy slozky projektu, je to problem, pripadne co s tim,
//pokud nechci balit vsechna do binarky?
bool loadData(ResourceManager& resources) {
#define cload(type, name, path) if (!resources.load ##type(name, path)) { cerr<< "Failed to load " << #type << ", path: " << path << endl; return false; }
	cload(HBTexture, "apple", "images/apple.png");
	cload(HBTexture, "lemon", "images/lemon.png");
	cload(HBTexture, "orange", "images/orange.png");
	cload(HBTexture, "pear", "images/pear.png");
	cload(HBTexture, "bunny", "images/bunny.png");
	cload(HBTexture, "beam", "images/beam.png");
	cload(HBTexture, "carrot", "images/carrot.png");
	cload(Texture, "carrot", "images/carrot.png");
	cload(Texture, "background", "images/background.png");
	cload(Texture, "block", "images/block.png");
	cload(Texture, "pistol", "images/pistol.png");
	cload(Texture, "giantgun", "images/giantgun.png");
	cload(Texture, "railgun", "images/railgun.png");
	cload(Sound, "pow", "sounds/pow.wav");
	cload(Sound, "railgun", "sounds/railgun.wav");
	cload(Sound, "background", "sounds/bceq.wav");
	cload(Sound, "gameover", "sounds/gameover.wav");
	cload(Sound, "buch", "sounds/buch.wav");
	cload(Font, "cmu", "cmunrm.otf");
	return true;
#undef cload
}

int main(void) {
	srand(time(NULL));

	ResourceManager resources;
	if (!loadData(resources)) {
		cout << "Exiting." << endl;
		return -1;
	}


#define ws 90
	Game sp(16*ws, 9*ws, resources);
	sf::RenderWindow window(sf::VideoMode(16*ws, 9*ws), "Fruit fever", sf::Style::Close);
	window.setVerticalSyncEnabled(true);
	sf::View view(sf::FloatRect(0,0,16*ws,9*ws));
	window.setView(view);
#undef ws

	//Main game loop
	while (window.isOpen() && sp.running()) {
		sf::Event event;
		while (window.pollEvent(event)) {
			if (event.type == sf::Event::Closed)
				window.close();
		}

		//Player control
		if(sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
			sp.movePlayer(-1);
                if(sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
			sp.movePlayer(1);
                if(sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
			sp.shoot();

		//#DEBUG
		//These are cheats - give player a desired gun, for testing porposes :-)
		if(sf::Keyboard::isKeyPressed(sf::Keyboard::Q))
			sp.gunToPlayer(make_unique<GiantGun>(resources));
                if(sf::Keyboard::isKeyPressed(sf::Keyboard::W))
			sp.gunToPlayer(make_unique<RailGun>(0, 10, 10, resources));
                if(sf::Keyboard::isKeyPressed(sf::Keyboard::E))
			sp.gunToPlayer(make_unique<Pistol>(0, 10, 50, resources));

		window.clear();
		sp.render(window);
		window.display();

		sp.tick();
	}

	//Game over ending screen
	while (window.isOpen() ) {
		sf::Event event;
		while (window.pollEvent(event)) {
			if (event.type == sf::Event::Closed)
				window.close();
		}
		window.clear();
		sp.renderEnding(window);
		window.display();
	}
	return 0;
}

