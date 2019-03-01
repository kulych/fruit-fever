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
	//particle lasts lifetime ticks, alive is set to false if lifetime==0
	int lifetime;
	sf::Color color;
public:
	bool alive;

	//lifetime is randomly set from 0 to 99
	Particle(sf::Vector2f position, sf::Vector2f velocity, sf::Color color) : position(position), velocity(velocity), lifetime(rand()%100), color(color), alive(true) {}

	//Tick moves particle in velocity direction and shortens lifetime by one
	void tick() {
		position += velocity;
		lifetime = std::max(lifetime-1, 0);
		if (lifetime == 0)
			alive = false;
	}
	void render(sf::RenderWindow&) const;
};

class Game {
	//The number of ground blocks
	int ground_num;
	//i-th ground block is rendered iff ground_alive[i] is true
	std::vector<bool> ground_alive;
	std::vector<sf::Sprite> ground;
	std::list<Particle> particles;
	std::list<std::unique_ptr<Bomb>> bombs;

	//Width and height of the game window
	int width, height;
	sf::Sprite background;

	//width (= active height) of one ground block
	int groundSize() { return width / ground_num; }

	//Score = number of hit shots
	//Probably needs to be reimplemented for multiplayer support
	int score;
	int level;

	//holdsthe previous-tick-level to detect level change
	int oldLevel;

public:
	ResourceManager& resources;
	std::list<std::unique_ptr<Shot>> shots;

	//Game is currently single-player, but can be simply extended, therefore vector<>
	std::vector<std::unique_ptr<Player>> players;

	//Game(width, height, resources)
	//initializes a new game with a given width and height
	Game(int, int, ResourceManager&);

	//Game is running (running() returns true) if player players[0] is alive
	//Probably needs to be reimplemented for multiplayer support
	bool running() const;

	//One game cycle
	//Spawns bombs, runs tick() of bombs, shots, particles, players
	//Calculates collisions, removes dead objects
	void tick();

	//explosion(position, number) spawns number of particles exploding from position
	void explosion(sf::Vector2f, int);

	//Fixes the closest broken block to player players[0]
	//Probably needs to be reimplemented for multiplayer support
	void fixOne();

	//Fixes all blocks
	void fixAll();

	//Player control interface (controls player players[0])
	//Needs to be reimplemented for multiplayer support
	void movePlayer(double);
	void shoot();
	//gunToPlayer(gun) gives gun to player
	void gunToPlayer(std::unique_ptr<Gun>);
	
	//Renders the gamestate to the given window
	//Used during the game is running()
	//Bombs, shots, player, background, ground blocks, score, gun info, explosions
	void render(sf::RenderWindow&) const;
	
	//Renders the ending screen to the given window
	void renderEnding(sf::RenderWindow&);

	//Game difficulty curve score->level
	int scoreToLevel() const;

	int getWidth() const { return width; }
	int getHeight() const { return height; }
};

class Bonus {
public:
	ResourceManager& resources;
	Bonus(ResourceManager& resources) : resources(resources) {}

	//apply(game) applies the bonus to game
	virtual void apply(Game&) = 0;

	//renders bonus visuals (this function is usually called by Bomb to render carried bonus over Bomb sprite)
	virtual void render(sf::RenderWindow&, sf::Vector2f) const = 0;
};

//DummyBonus does nothing and has no effect
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
	void render(sf::RenderWindow&, sf::Vector2f) const override;
};

class FixBonus : public Bonus {
	//number of blocks to be fixed, -1 means fix all of them
	int amount;
public:
	FixBonus(int amount, ResourceManager& resources) : Bonus(resources), amount(amount) {}
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
	//Renders bomb sprite and bonus over it
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

	//Player(position, speed, resources)
	//speed.y should be 0, game is designed only for horizontal player movement
	Player(sf::Vector2f, sf::Vector2f, ResourceManager&);
	bool alive;
	void tick();

	//moveLimit(left, right, multip) moves player position by as much as multip*speed 
	//so that the resulting position x-coordinate is still inbetween left and right
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
	//Projectile(position, speed, resources)
	Projectile(sf::Vector2f, sf::Vector2f, ResourceManager&);
	void tick() override {
		sprite.move(speed);
	}
	//Projectile dies on first hit
	void hit(Game &sp) override { alive = false; }
};

class RotatingGiant : public Shot {
	//lifelength - the total length of life in ticks
	int lifelength;
	//lifetime - the remaining number of ticks
	int lifetime;
public:
	//RotatingGiant(position, lifelength, resources)
	RotatingGiant(sf::Vector2f, int, ResourceManager&);
	
	//decreases lifetime, rotates by 4 degrees
	//sprite scale = (lifetime/lifelength)^3
	void tick() override;

	void move(double x, double y) {
		sprite.move(x,y);
	}

	//is immune to hits
	void hit(Game &sp) override {}
};

class Laser : public Shot {
	//lifetime - the remaining number of ticks
	int lifetime;
public:
	//Laser(position, angle, lifetime, resources)
	Laser(sf::Vector2f, double, int, ResourceManager&);

	void tick() override {
		if (lifetime <= 0)
			alive = false;
		lifetime--;
	}

	//is immune to hits
	void hit(Game &sp) override {}
};

class Gun {
protected:
	ResourceManager& resources;
public:
	//loaded == 0 means gun is loaded
	//loaded is decreased by one every tick
	int loaded;

	//loaded is set to interval after gunshot
	int interval;

	//ammo == -1 means infinite ammo
	int ammo;

	Gun(int loaded, int interval, int ammo, ResourceManager& resources) : resources(resources), loaded(loaded), interval(interval), ammo(ammo) {}

	//Spawns shot(s) into game (can use Player position)
	//Plays a sound
	virtual void shoot(Game&, Player&) = 0;

	virtual void tick() {
		loaded = std::max(0, loaded-1);
	}
	virtual void render(sf::RenderWindow& window, sf::Vector2f position) const = 0;
};

class Pistol : public Gun {
public:
	Pistol(int loaded, int interval, int ammo, ResourceManager& resources) : Gun(loaded, interval, ammo, resources) {}
	Pistol(int interval, ResourceManager& resources) : Pistol(0, interval, -1, resources) {}
	void shoot(Game&, Player&) override;
	void render(sf::RenderWindow&, sf::Vector2f) const override;
};	

class GiantGun : public Gun {
public:
	GiantGun(int loaded, int interval, int ammo, ResourceManager& resources) : Gun(loaded, interval, ammo,resources) {}
	GiantGun(int interval, int ammo, ResourceManager& resources) : GiantGun(0, interval, ammo,resources) {}
	GiantGun(ResourceManager& resources) : GiantGun(0, 10, 1, resources) {}
	void shoot(Game&, Player&) override;
	void render(sf::RenderWindow&, sf::Vector2f) const override;
};

class RailGun : public Gun {
public:
	RailGun(int loaded, int interval, int ammo, ResourceManager& resources) : Gun(loaded, interval, ammo, resources) {}
	void shoot(Game&, Player&) override;
	void render(sf::RenderWindow&, sf::Vector2f) const override;
};

#endif
