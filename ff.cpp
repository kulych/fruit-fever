#include <SFML/Graphics.hpp>
#include <cmath>
#include <vector>
#include <iostream>
#include <list>
#include <memory>

using namespace std;

class Bomb {
	const sf::Texture& texture;
	sf::Vector2f position;
	sf::Vector2f speed;
	sf::Sprite sprite;
	public:
		Bomb(const sf::Texture&, sf::Vector2f, sf::Vector2f);
		void render(sf::RenderWindow&);	
		void tick();
};

Bomb::Bomb(const sf::Texture& texture, sf::Vector2f position, sf::Vector2f speed) : texture(texture), position(position), speed(speed), sprite(texture) {
	sprite.setPosition(position);
}

void Bomb::render(sf::RenderWindow& window) {
	sprite.setPosition(position);
	window.draw(sprite);
}

void Bomb::tick() {
	position += speed;
}

class Space {
	unsigned int ground_num;
	vector<bool> ground_alive;
	vector<sf::Sprite> ground;
	const sf::Texture& ground_texture;
	list<unique_ptr<Bomb>> bombs;
	
public:
	Space(const sf::Texture&);
	void tick();
	void render(sf::RenderWindow&) const;
};

void Space::render(sf::RenderWindow& window) const{
	//window dimensions
	for (int i = 0; i < ground_num; ++i) {
		if (ground_alive[i])
			window.draw(ground[i]);
	}
	for (const auto& bomb : bombs)
		bomb->render(window);
}

void Space::tick() {
	for (auto& bomb : bombs) 
		bomb->tick();
}

Space::Space(const sf::Texture& ground_texture) : ground_num(35), ground_texture(ground_texture) {	
	for (int i = 0; i < ground_num; ++i) {
		ground_alive.push_back(true);

		ground.emplace_back(ground_texture);
		ground[i].setPosition(sf::Vector2f(1400.0/ground_num * i, 400));
		ground[i].scale();
	}
	bombs.push_back(make_unique<Bomb>(ground_texture, sf::Vector2f(200, 200), sf::Vector2f(0, 3)));
}

int main(void) {
	sf::RenderWindow window(sf::VideoMode(1400, 800), "Gravity!", sf::Style::Close);
	window.setVerticalSyncEnabled(true);
	sf::View view(sf::FloatRect(0,0,1400,800));
	window.setView(view);

	sf::Sprite sprite;
	sf::Texture block;
	block.loadFromFile("block.png");
	Space sp(block);

	while (window.isOpen()) {
		sf::Event event;
		while (window.pollEvent(event)) {
			if (event.type == sf::Event::Closed)
				window.close();
		}
		window.clear();
		sp.render(window);
		window.display();
		sp.tick();
	}

	return 0;
}
