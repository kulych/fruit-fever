#include <SFML/Graphics.hpp>
#include <cmath>
#include <vector>
#include <iostream>
#include <list>
#include <memory>
#include <cstdlib>
#include <time.h>
#include <cmath>

using namespace std;

class Space;

class Bomb {
	sf::Vector2f speed;
public:
	sf::Sprite sprite;
	static sf::Texture textures[4];
	bool alive;
	Bomb(sf::Vector2f, sf::Vector2f);
	void render(sf::RenderWindow&);	
	void tick();

};

class Shot {
	sf::Vector2f speed;
public:
	sf::Sprite sprite;
	static sf::Texture texture;
	Shot(sf::Vector2f, sf::Vector2f);
	void render(sf::RenderWindow&);	
	void tick();

};


class Player {
	sf::Vector2f speed;
	sf::Sprite sprite;
public:
	static sf::Texture texture;
	Player(sf::Vector2f, sf::Vector2f);
	void move(float);
	void shoot(Space&);
	void render(sf::RenderWindow&);
	void tick();
};

class Space {
	unsigned int ground_num;
	vector<bool> ground_alive;
	vector<sf::Sprite> ground;
	list<unique_ptr<Bomb>> bombs;
public:
	list<unique_ptr<Shot>> shots;
	static sf::Texture ground_texture;
	vector<unique_ptr<Player>> players;
	Space();
	void tick();
	void rescale(const sf::RenderWindow&);
	void render(sf::RenderWindow&) const;
};

Player::Player(sf::Vector2f position, sf::Vector2f speed) : speed(speed), sprite(texture) {
	sprite.setOrigin(sprite.getGlobalBounds().width/2.0, sprite.getGlobalBounds().height);
	sprite.scale(0.2, 0.2);
	sprite.setPosition(position.x, 800);
}

void Player::shoot(Space& space) {
	space.shots.push_front(make_unique<Shot>(sprite.getPosition(), sf::Vector2f(-4, -8)));
	space.shots.push_front(make_unique<Shot>(sprite.getPosition(), sf::Vector2f( 4, -8)));
}

void Player::move(float multip) {
	sprite.move(multip*speed.x, 0);
}

void Player::render(sf::RenderWindow& window) {
	window.draw(sprite);
}

Bomb::Bomb(sf::Vector2f position, sf::Vector2f speed) : speed(speed), sprite(textures[rand()%4]), alive(true) {
	sprite.setPosition(position);
	sprite.scale(0.2,0.2);
}

void Bomb::render(sf::RenderWindow& window) {
	window.draw(sprite);
}

void Bomb::tick() {
	sprite.move(speed);
	if (sprite.getPosition().y > 800)
		alive = false;
}

Shot::Shot(sf::Vector2f position, sf::Vector2f speed) : speed(speed), sprite(texture){
	sprite.setOrigin(sprite.getGlobalBounds().width/2.0, sprite.getGlobalBounds().height/2.0);
	sprite.setPosition(position);
	sprite.setScale(0.2, 0.2);
	sprite.rotate(atan2(speed.y, speed.x)/3.1415926535*180);
}

void Shot::render(sf::RenderWindow& window) {
	window.draw(sprite);
}

void Shot::tick() {
	sprite.move(speed);
}


void Space::rescale(const sf::RenderWindow& window) {
	float wwidth = window.getView().getSize().y;
	float wheight = window.getView().getSize().x;
	for (int i = 0; i < ground_num; ++i){
		float fct = wwidth/(ground_num*ground[i].getGlobalBounds().width);
		ground[i].scale(fct,fct);
		ground[i].setPosition(sf::Vector2f(wwidth/ground_num * i, wheight));
	}
}

void Space::render(sf::RenderWindow& window) const{
	//window dimensions
	for (int i = 0; i < ground_num; ++i) {
		if (ground_alive[i])
			window.draw(ground[i]);
	}
	for (const auto& bomb : bombs)
		bomb->render(window);
	for (const auto& shot : shots)
		shot->render(window);
	for (const auto& player : players)
		player->render(window);
}

void Space::tick() {
	if (rand() % 100 < 3) 
		bombs.push_back(make_unique<Bomb>(sf::Vector2f(rand()%800, 0), sf::Vector2f(0, 5)));
	for (auto& bomb : bombs) 
		bomb->tick();
	for (auto& shot : shots) 
		shot->tick();

	auto bomb = bombs.begin();
	while (bomb != bombs.end()) {
		auto shot = shots.begin();
		bool destroyed = false;
		while (shot != shots.end()) {
			if ((*bomb)->sprite.getGlobalBounds().intersects((*shot)->sprite.getGlobalBounds())) {
				bombs.erase(bomb++);
				shots.erase(shot);
				destroyed = true;
				break;
			}
			else 
				shot++;
		}
		if (!destroyed)
			bomb++;
	}
				

	auto it = bombs.begin();
	while (it != bombs.end()) {
		if ((*it)->alive)
			it++;
		else
			bombs.erase(it++);
	}
}

Space::Space() : ground_num(50) {	
	for (int i = 0; i < ground_num; ++i) {
		ground_alive.push_back(true);
		ground.emplace_back(ground_texture);
	}
	players.push_back(make_unique<Player>(sf::Vector2f(400,0), sf::Vector2f(10, 0)));
	
}

sf::Texture Space::ground_texture;
sf::Texture Bomb::textures[4];
sf::Texture Player::texture;
sf::Texture Shot::texture;

int main(void) {
	srand(time(NULL));


	Space::ground_texture.loadFromFile("svg/block.png");
	Space::ground_texture.setSmooth(true);

	Bomb::textures[0].loadFromFile("svg/apple.png");
	Bomb::textures[0].setSmooth(true);
	Bomb::textures[1].loadFromFile("svg/pear.png");
	Bomb::textures[1].setSmooth(true);
	Bomb::textures[2].loadFromFile("svg/orange.png");
	Bomb::textures[2].setSmooth(true);
	Bomb::textures[3].loadFromFile("svg/lemon.png");
	Bomb::textures[3].setSmooth(true);

	Player::texture.loadFromFile("svg/bunny.png");
	Player::texture.setSmooth(true);

	Shot::texture.loadFromFile("svg/carrot.png");
	Shot::texture.setSmooth(true);

	Space sp;
	sf::RenderWindow window(sf::VideoMode(800, 800), "Gravity!", sf::Style::Close);
	window.setVerticalSyncEnabled(true);
	sf::View view(sf::FloatRect(0,16,800,800));

	window.setView(view);
	sp.rescale(window);



	while (window.isOpen()) {
		sf::Event event;
		while (window.pollEvent(event)) {
			if (event.type == sf::Event::Closed)
				window.close();
		}
		if(sf::Keyboard::isKeyPressed(sf::Keyboard::Left)){
			sp.players[0]->move(-1);
                }       
                if(sf::Keyboard::isKeyPressed(sf::Keyboard::Right)){
			sp.players[0]->move(1);
                }       
                if(sf::Keyboard::isKeyPressed(sf::Keyboard::Space)){
                        sp.players[0]->shoot(sp);
                }
		/*
                if(sf::Keyboard::isKeyPressed(sf::Keyboard::Down)){
                        ship->boost(-1000, 0.01);
                }       
                if(sf::Keyboard::isKeyPressed(sf::Keyboard::C)){
                        view.setCenter(ship->pos.x, ship->pos.y);
                        window.setView(view);
                } */      



		window.clear();
		sp.render(window);
		window.display();
		sp.tick();
	}
	return 0;
}

