#include "primitives.hpp"
#include "hitbox.hpp"
#include <SFML/Graphics.hpp>
#include <memory>
#include <vector>
#include <string>
#include <iostream>
#include <fstream>

HBTexture::HBTexture(const sf::Texture& texture, const std::vector<std::unique_ptr<Primitive>>& hitbox) : texture(texture) {
	for (const std::unique_ptr<Primitive>& prim : hitbox) { 
		(this->hitbox).push_back(std::move(prim->clone()));
	}
}

void HBTexture::loadFromFile(const std::string& path) {
	texture.loadFromFile(path);
	std::cout << "loaded" << std::endl;
	std::ifstream fil;
	fil.open(path + ".hb");
	std::cout << path << std::endl;
	double x, y, x2, y2, r;
	std::string type;
	fil >> x >> y;
	while(!fil.eof()) {
		fil >> type;
		if (type == "C") {
			fil >> x >> y >> r;
			hitbox.push_back(std::make_unique<Circle>(x,y,r));
			std::cout << "add circ " << x << " " << y << " " << r << std::endl;
		}
		else if (type == "L") {
			fil >> x >> y >> x2 >> y2;
			hitbox.push_back(std::make_unique<Line>(x,y,x2,y2));
			std::cout << "add line " << path <<std::endl;
		}
	}
}

Figure::Figure() : position(sf::Vector2f(0,0)) {
}

Figure::Figure(const HBTexture& texture) : sprite(texture.texture), position(sf::Vector2f(0,0)) {
	for (const std::unique_ptr<Primitive>& prim : texture.hitbox) { 
		hitbox.push_back(std::move(prim->clone()));
	}
}

void Figure::setTexture(const HBTexture& texture) {
	sprite.setTexture(texture.texture);
	for (const std::unique_ptr<Primitive>& prim : texture.hitbox) { 
		hitbox.push_back(std::move(prim->clone()));
	}
}

void Figure::setOrigin(double x, double y) {
	sprite.setOrigin(x,y);
	sf::Vector2f new_origin = sprite.getOrigin()*sprite.getScale().x;
	sf::Vector2f dif = origin - new_origin;
	origin = new_origin;
	for (std::unique_ptr<Primitive>& prim : hitbox) 
		prim->shift(dif.x, dif.y);
}

void Figure::scale(double ratio) {
	sprite.scale(ratio, ratio);
	origin = (float)ratio*sprite.getOrigin();
	for (std::unique_ptr<Primitive>& prim : hitbox)
		prim->scale(ratio);
}

void Figure::setScale(double ratio) {
	double compensation = ratio/sprite.getScale().x;
	scale(compensation);
}

void Figure::rotate(double angle) {
	for (std::unique_ptr<Primitive>& prim : hitbox)
		prim->rotate(0, 0, angle);
	sprite.rotate(angle);
}

bool Figure::collides(const Figure& other) const {
	for (const std::unique_ptr<Primitive>& primA : hitbox) {
		for (const std::unique_ptr<Primitive>& primB : other.hitbox) {
			//shift him to me
			sf::Vector2f sh = other.position - position;
			if (primA->intersects(*primB, sh.x, sh.y))
				return true;
		}
	}
	return false;
}

void Figure::render(sf::RenderWindow& window) const {
	window.draw(sprite);
	//for (const std::unique_ptr<Primitive>& prim : hitbox)
	//	prim->render(window, position.x, position.y);
}
