#ifndef __HITBOX_HPP
#define __HITBOX_HPP

#include <SFML/Graphics.hpp>
#include <memory>
#include <vector>
#include <string>
#include "primitives.hpp"

struct HBTexture {
public:
	sf::Texture texture;
	std::vector<std::unique_ptr<Primitive>> hitbox;
	HBTexture() {}

	//Loads the hitbox and texture from file 'path'
	//throws if unsuccessful
	HBTexture(const std::string& path) {
		if (!loadFromFile(path))
			throw std::runtime_error("Could not open file " + path);
	}
	//Loads texture and hitbox separately from given parameters, moves hitbox
	HBTexture(const sf::Texture& texture, std::vector<std::unique_ptr<Primitive>>&& hitbox) : texture(texture), hitbox(move(hitbox)) {}

	//Loads texture and hitbox separately from given parameters, copies hitbox
	HBTexture(const sf::Texture&, const std::vector<std::unique_ptr<Primitive>>&);

	//Loads the hitbox and texture from file
	//returns false if unsuccussful
	//loadFromFile("pathtofile.png");
	//expect as well existence of "pathtofile.png.hb" that conaints hitbox information
	bool loadFromFile(const std::string&);

	void setSmooth(bool set) { texture.setSmooth(set); }
};

struct Figure {
	sf::Vector2f position;
	sf::Vector2f origin;
	std::vector<std::unique_ptr<Primitive>> hitbox;
public:
	sf::Sprite sprite;
	Figure();

	//Creates figure from given HBTexture
	Figure(const HBTexture&);

	//Overwrites current HBTexture with new one
	void setTexture(const HBTexture& texture);

	//setOrigin(x,y) sets origin to x,y in local Figure coordinates
	//(ignores trasformations)
	void setOrigin(double, double);

	//sets origin to center - width/2, height/2
	void centerOrigin();

	sf::Vector2f getOrigin() { return origin; }

	//scale(ratio) scales figure by ratio in both directions
	//respects previous scalings
	void scale(double);

	//scales with respect to the original size
	void setScale(double);
	void rotate(double);
	void setPosition(sf::Vector2f pos) { position = pos; sprite.setPosition(pos); }
	void setPosition(double px, double py) { setPosition(sf::Vector2f(px,py)); }
	sf::FloatRect getGlobalBounds() const { return sprite.getGlobalBounds(); }
	sf::Vector2f getPosition() const { return position; }
	void move(double x, double y) { position += sf::Vector2f(x,y); sprite.move(x,y); }
	void move(sf::Vector2f d) {position += d; sprite.move(d); }

	//Detects collision with other Figure object
	//Collision = bounding rectangles intersection & hitbox intersectiion
	bool collides(const Figure&) const;
	void render(sf::RenderWindow&) const;
};

#endif
