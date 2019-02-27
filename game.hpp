#ifndef __GAME_HPP
#define __GAME_HPP

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <cmath>
#include <vector>
#include <list>
#include <memory>
#include <cstdlib>
#include <time.h>
#include <cmath>
#include "hitbox.hpp"
#include "resources.hpp"

//General container for one object
class Body;

//Falling bomb, dies on hit, can carry bonuses
class Bomb; // : Body

//General Shot functionality
class Shot; // : Body
//Projectile flying in one direction, dies on hit
class Projectile; // : Shot
//Rotating object, size scaling down in time, destroys bombs until scale = 0
class RotatingGiant; // : Shot;
//Laser beam, destroys everything in direction
class Laser; // : Shot;

//Abstract Gun functionality
class Gun;
//Shoots Projectiles
class Pistol; // : Gun
//Spawns RotatingGiant
class GiantGun; // : Gun
//Shoots Lasers
class RailGun; // : Gun

//Explosion particle, moves in given direction, flickers
class Particle;

//Controlable player, has gun, can move
class Player; // : Body

//Abstract base class for Bonuses
class Bonus; 
//Every bomb carries a bonus - this one is nothing
class DummyBonus; // : Bonus
//Fixes a number of destroyed ground blocks
class FixBonus; // : Bonus
//Carries a gun
class GunBonus; // : Bonus

//Encapsulates all game primitives, implements game logic
class Game;




class Body {
private:
public:
	Figure sprite;
	Body() { }
	Body(const HBTexture& texture) : sprite(texture) { }
	virtual void setTexture(const HBTexture& texture) {
		sprite.setTexture(texture);
	}
	virtual void render(sf::RenderWindow& window) {
			sprite.render(window);
	}
	virtual bool collides(const Body& other) {
		return sprite.collides(other.sprite);
	}
};

class Particle {
	sf::Vector2f position, velocity;
	int lifetime;
	sf::Color color;
public:
	bool alive;
	Particle(sf::Vector2f position, sf::Vector2f velocity, sf::Color color) : position(position), velocity(velocity), lifetime(rand()%100), alive(true),
	color(color) {}
	void tick() {
		position += velocity;
		lifetime = std::max(lifetime-1, 0);
		if (lifetime == 0)
			alive = false;
	}
	void render(sf::RenderWindow&) const;
};

class Game {
	unsigned int ground_num;
	std::vector<bool> ground_alive;
	std::vector<sf::Sprite> ground;
	std::list<Particle> particles;
	std::list<std::unique_ptr<Bomb>> bombs;
	int width, height;
	sf::Sprite background;
	int groundSize() { return width / ground_num; }
	int score;
	int level;
	int oldLevel;
public:
	ResourceManager& resources;
	std::list<std::unique_ptr<Shot>> shots;
	std::vector<std::unique_ptr<Player>> players;
	Game(int, int, ResourceManager&);
	bool running() const;
	void tick();
	void explosion(sf::Vector2f, int);
	void gunToPlayer(std::unique_ptr<Gun>);
	void fixOne();
	void fixAll();
	//Player controls interface
	void movePlayer(double);
	void shoot();
	
	void render(sf::RenderWindow&) const;
	void renderEnding(sf::RenderWindow&);
	int scoreToLevel() const;
	int getWidth() const { return width; }
	int getHeight() const { return height; }
};

class Bonus {
public:
	ResourceManager& resources;
	Bonus(ResourceManager& resources) : resources(resources) {}
	virtual void apply(Game&) = 0;
	virtual void render(sf::RenderWindow&, sf::Vector2f) const = 0;
};

class DummyBonus : public Bonus {
public:
	DummyBonus(ResourceManager& resources) : Bonus(resources) {}
	void apply(Game& sp) override {}
	void render(sf::RenderWindow& window, sf::Vector2f position) const override {}
};

class GunBonus : public Bonus {
	std::unique_ptr<Gun> gun;
public:
	GunBonus(std::unique_ptr<Gun> gun, ResourceManager& resources) : Bonus(resources), gun(std::move(gun)) {}
	void apply(Game& game) override {
		game.gunToPlayer(std::move(gun));
	}
	void render(sf::RenderWindow&, sf::Vector2f) const;
};

class FixBonus : public Bonus {
	int amount;
public:
	FixBonus(int amount, ResourceManager& resources) : amount(amount), Bonus(resources) {}
	FixBonus(ResourceManager& resources) : FixBonus(1, resources) {}
	void apply(Game&) override;
	void render(sf::RenderWindow&, sf::Vector2f) const override;
};

class Bomb : public Body {
	sf::Vector2f speed;
	std::unique_ptr<Bonus> bonus;
	ResourceManager& resources;
public:
	bool alive;
	Bomb(sf::Vector2f, sf::Vector2f, std::unique_ptr<Bonus>, ResourceManager&);
	Bomb(sf::Vector2f position, sf::Vector2f speed, ResourceManager& resources) : Bomb(position, speed, std::make_unique<DummyBonus>(resources), resources) {}	
	virtual void tick() {
		sprite.move(speed);
	}
	virtual void hit(Game& game) {
		alive = false;
		bonus->apply(game);
	}
	virtual void render(sf::RenderWindow& window) override {
		sprite.render(window);
		bonus->render(window, sprite.position);
	}
};


class Player : public Body{
	sf::Vector2f speed;
	ResourceManager& resources;
public:
	std::unique_ptr<Gun> gun;
	Player(sf::Vector2f, sf::Vector2f, ResourceManager&);
	bool alive;
	void tick();
	void moveLimit(double, double, double);
	void move(float multip) {
		sprite.move(multip*speed.x, 0);
	}
	void defaultGun();
	void shoot(Game&);
	void die(Game& game) {
		game.explosion(sprite.getPosition(), 10000);
		alive = false;
	}
};

class Shot : public Body {	
public:
	bool alive;
	Shot(const HBTexture& texture) : Body(texture), alive(true) {};
	virtual void tick() {} ;
	virtual void hit(Game &sp) { };

};

class Projectile : public Shot {
	sf::Vector2f speed;
public:
	Projectile(sf::Vector2f, sf::Vector2f, ResourceManager&);
	void tick() override {
		sprite.move(speed);
	}
	void hit(Game &sp) override { alive = false; }
};

class RotatingGiant : public Shot {
	int lifelength;
	int lifetime;
public:
	RotatingGiant(sf::Vector2f, int, ResourceManager&);
	void tick() override;
	void move(double x, double y) {
		sprite.move(x,y);
	}
	void hit(Game &sp) override {}
};

class Laser : public Shot {
	int lifetime;
public:
	Laser(sf::Vector2f, double, int, ResourceManager&);
	void tick() override {
		if (lifetime <= 0)
			alive = false;
		lifetime--;
	}
	void hit(Game &sp) override {}
};

class Gun {
protected:
	ResourceManager& resources;
public:
	int loaded;
	int interval;
	int ammo;
	Gun(int loaded, int interval, int ammo, ResourceManager& resources) : loaded(loaded), interval(interval), ammo(ammo), resources(resources) {}
	virtual void shoot(Game&, Player&) = 0;
	virtual void tick() = 0;
	virtual void render(sf::RenderWindow& window, sf::Vector2f position) const = 0;
};

class Pistol : public Gun {
public:
	Pistol(int loaded, int interval, int ammo, ResourceManager& resources) : Gun(loaded, interval, ammo, resources) {}
	Pistol(int interval, ResourceManager& resources) : Pistol(0, interval, -1, resources) {}
	void shoot(Game&, Player&) override;
	void tick() override {
		loaded = std::max(0, loaded - 1);
	}
	void render(sf::RenderWindow&, sf::Vector2f) const;
};	

class GiantGun : public Gun {
public:
	GiantGun(int loaded, int interval, int ammo, ResourceManager& resources) : Gun(loaded, interval, ammo,resources) {}
	GiantGun(int interval, int ammo, ResourceManager& resources) : GiantGun(0, interval, ammo,resources) {}
	GiantGun(ResourceManager& resources) : GiantGun(0, 10, 1, resources) {}
	void shoot(Game&, Player&) override;
	void tick() override {
		loaded = std::max(0, loaded-1);
	}
	void render(sf::RenderWindow&, sf::Vector2f) const;
};

class RailGun : public Gun {
public:
	RailGun(int loaded, int interval, int ammo, ResourceManager& resources) : Gun(loaded, interval, ammo, resources) {}
	void shoot(Game&, Player&) override;
	void tick() override {
		loaded = std::max(0, loaded-1);
	}
	void render(sf::RenderWindow&, sf::Vector2f) const;
};

#endif
