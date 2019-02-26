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
	HBTexture(const std::string& path) { loadFromFile(path); }
	HBTexture(const sf::Texture& texture, std::vector<std::unique_ptr<Primitive>>&& hitbox) : texture(texture), hitbox(move(hitbox)) {}
	HBTexture(const sf::Texture&, const std::vector<std::unique_ptr<Primitive>>&);
	void loadFromFile(const std::string&);
	void setSmooth(bool set) { texture.setSmooth(set); }
};

struct Figure {
	sf::Vector2f position;
	sf::Vector2f origin;
	std::vector<std::unique_ptr<Primitive>> hitbox;
public:
	sf::Sprite sprite;
	Figure();
	Figure(const HBTexture&);
	void setTexture(const HBTexture& texture);
	void setOrigin(double, double);
	void centerOrigin();
	sf::Vector2f getOrigin() { return origin; }
	void scale(double);
	void setScale(double);
	void rotate(double);
	void setPosition(sf::Vector2f pos) { position = pos; sprite.setPosition(pos); }
	void setPosition(double px, double py) { setPosition(sf::Vector2f(px,py)); }
	sf::FloatRect getGlobalBounds() const { return sprite.getGlobalBounds(); }
	sf::Vector2f getPosition() const { return position; }
	void move(double x, double y) { position += sf::Vector2f(x,y); sprite.move(x,y); }
	void move(sf::Vector2f d) {position += d; sprite.move(d); }
	bool collides(const Figure&) const;
	void render(sf::RenderWindow&) const;
};

#endif
