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
	double x, y, r;
	fil >> x >> y;
	while(!fil.eof()) {
		fil >> x >> y >> r;
		hitbox.push_back(std::make_unique<Circle>(x,y,r));
		std::cout << "add circ " << x << " " << y << " " << r << std::endl;
	}
}

Figure::Figure(const HBTexture& texture) : sprite(texture.texture) {
	for (const std::unique_ptr<Primitive>& prim : texture.hitbox) { 
		hitbox.push_back(std::move(prim->clone()));
	}
}

void Figure::setOrigin(double x, double y) {
	sf::Vector2f dif = origin - sf::Vector2f(x,y);
	origin = sf::Vector2f(x,y);
	sprite.setOrigin(x,y);
	for (std::unique_ptr<Primitive>& prim : hitbox) 
		prim->shift(dif.x, dif.y);
}

void Figure::scale(double ratio) {
	sprite.scale(ratio, ratio);
	origin = (float)ratio*origin;
	for (std::unique_ptr<Primitive>& prim : hitbox)
		prim->scale(ratio);
}

void Figure::rotate(double angle) {
	for (std::unique_ptr<Primitive>& prim : hitbox)
		prim->rotate(0, 0, angle);
	sprite.rotate(angle);
}

bool Figure::collides(const Figure& other) const {
	for (const std::unique_ptr<Primitive>& primA : hitbox) {
		for (const std::unique_ptr<Primitive>& primB : other.hitbox) {
			sf::Vector2f sh = position - other.position;
			if (primA->intersects(*primB, sh.x, sh.y)) 
				return true;
		}
	}
	return false;
}

void Figure::render(sf::RenderWindow& window) const {
	window.draw(sprite);
	for (const std::unique_ptr<Primitive>& prim : hitbox)
		prim->render(window, position.x, position.y);
}
