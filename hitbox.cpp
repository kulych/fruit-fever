#include "primitives.hpp"
#include "hitbox.hpp"
#include <SFML/Graphics.hpp>
#include <memory>
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
//#OTAZKA mam includovat znovu to, co je includovane uz v .hpp?
//je prehlednejsi, ze je na zacatku souboru videt, co se v nem pouziva
//ale je to zbytecne

//Loads separately from texture and hitbox (vector of primitives)
HBTexture::HBTexture(const sf::Texture& texture, const std::vector<std::unique_ptr<Primitive>>& hitbox) : texture(texture) {
	for (const std::unique_ptr<Primitive>& prim : hitbox) { 
		(this->hitbox).push_back(std::move(prim->clone()));
	}
}

bool HBTexture::loadFromFile(const std::string& path) {
	//Actual texture
	if (!texture.loadFromFile(path))
		return false;

	//Hitbox information 
	std::ifstream fil;
	fil.open(path + ".hb");
	if (!fil.good())
		return false;

	double x, y, x2, y2, r;
	std::string type;

	//First line contains two fields - width and height
	//These two must match the actual texture size, so they are skipped
	fil >> x >> y;
	
	//Each other line contains one hitbox primitev
	while(!fil.eof()) {
		fil >> type;
		//first field specifies the type of hitbox primitive C = circle, L = line
		if (type == "C") {
			fil >> x >> y >> r;
			hitbox.push_back(std::make_unique<Circle>(x,y,r));
		}
		else if (type == "L") {
			fil >> x >> y >> x2 >> y2;
			hitbox.push_back(std::make_unique<Line>(x,y,x2,y2));
		}
	}
	return true;
}

Figure::Figure() : position(sf::Vector2f(0,0)) { }

Figure::Figure(const HBTexture& texture) : position(sf::Vector2f(0,0)), sprite(texture.texture) {
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

//Sets origin to x,y (x,y is in object-local coordinate - ignores transformations)
void Figure::setOrigin(double x, double y) {
	sprite.setOrigin(x,y);
	sf::Vector2f new_origin = sprite.getOrigin()*sprite.getScale().x;
	sf::Vector2f dif = origin - new_origin;
	origin = new_origin;
	for (std::unique_ptr<Primitive>& prim : hitbox) 
		prim->shift(dif.x, dif.y);
}

//Sets origin to center
void Figure::centerOrigin() {
	setOrigin(getGlobalBounds().width/2, getGlobalBounds().height/2);
}

//Scales by ration in both axes (reflects previous scalings)
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

//Rotates by angle (given in degrees) around origin
void Figure::rotate(double angle) {
	for (std::unique_ptr<Primitive>& prim : hitbox)
		//origin is 0,0 in hitbox coordinate system (primitives are shift every time origin changes)
		prim->rotate(0, 0, angle);
	sprite.rotate(angle);
}

//Detects a collision between two Figures
//First checks bounding rectangles, if they intersect, calculates precise collision
//using hitbox primitives (tests every pair primA primB)
bool Figure::collides(const Figure& other) const {
	if (!sprite.getGlobalBounds().intersects(other.sprite.getGlobalBounds()))
		return false;
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
	//#DEBUG These two commented lines render hitbox
	//for (const std::unique_ptr<Primitive>& prim : hitbox)
	//	prim->render(window, position.x, position.y);
}

