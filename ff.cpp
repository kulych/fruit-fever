#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <cmath>
#include <vector>
#include <iostream>
#include <fstream>
#include <list>
#include <memory>
#include <cstdlib>
#include <time.h>
#include <cmath>
#include "hitbox.hpp"
#include "resources.hpp"

using namespace std;

class Gun; 

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
	//virtual void tick() = 0;
};

class Particle;
class Bomb;
class Player;
class Shot;

class Space {
	unsigned int ground_num;
	vector<bool> ground_alive;
	vector<sf::Sprite> ground;
	//vector nebo list?
	list<Particle> particles;
	list<unique_ptr<Bomb>> bombs;
	int width, height;
	sf::Sprite background;
	int groundSize() { return width / ground_num; }
public:
	ResourceManager& resources;
	list<unique_ptr<Shot>> shots;
	vector<unique_ptr<Player>> players;
	Space(int, int, ResourceManager&);
	void tick();
	void rescale(const sf::RenderWindow&);
	void explosion(sf::Vector2f);
	void gunToPlayer(unique_ptr<Gun>);
	void movePlayer(double);
	void render(sf::RenderWindow&) const;
	void renderEnding(sf::RenderWindow&) const;
	void fixOne();
	void fixAll();
	bool running() const;
	int getWidth() const { return width; }
	int getHeight() const { return height; }
};

class Gun {
protected:
	ResourceManager& resources;
public:
	int loaded;
	int interval;
	int ammo;
	//Const player??
	//remove player?
	Gun(int loaded, int interval, int ammo, ResourceManager& resources) : loaded(loaded), interval(interval), ammo(ammo), resources(resources) {}
	virtual void shoot(Space&, Player&) = 0;
	virtual void tick() = 0;
	virtual void render(sf::RenderWindow& window, sf::Vector2f position) const = 0;
};



class Bonus {
public:
	ResourceManager& resources;
	Bonus(ResourceManager& resources) : resources(resources) {}
	virtual void apply(Space&) = 0;
	virtual void render(sf::RenderWindow&, sf::Vector2f) const = 0;
};

class DummyBonus : public Bonus {
public:
	DummyBonus(ResourceManager& resources) : Bonus(resources) {}
	void apply(Space& sp) override {}
	void render(sf::RenderWindow& window, sf::Vector2f position) const override {}
};

class GunBonus : public Bonus {
	unique_ptr<Gun> gun;
public:
	GunBonus(unique_ptr<Gun> gun, ResourceManager& resources) : Bonus(resources), gun(move(gun)) {}
	void apply(Space& space) override {
		space.gunToPlayer(move(gun));
	}
	void render(sf::RenderWindow& window, sf::Vector2f position) const override {
		gun->render(window, position);
	}
};

class FixBonus : public Bonus {
	int amount;
public:
	FixBonus(int amount, ResourceManager& resources) : amount(amount), Bonus(resources) {}
	FixBonus(ResourceManager& resources) : FixBonus(1, resources) {}
	void apply(Space& space) override {
		if (amount == -1)
			space.fixAll();
		else {
			for(int i = 0; i < amount; ++i)
				space.fixOne();
		}
	}
	void render(sf::RenderWindow& window, sf::Vector2f position) const override {
/*		sf::Text text(amount == -1 ? "ALL" : "", resources.getFont("cmu"), 20);
		text.setPosition(position);
		text.setOrigin(text.getGlobalBounds().width/2, text.getGlobalBounds().height/2);
*/
		sf::Sprite sprite(resources.getTexture("block"));
		sprite.setPosition(position);
		sprite.setOrigin(sprite.getGlobalBounds().width/2, sprite.getGlobalBounds().height/2);
		sprite.scale(0.4, 0.4);
		window.draw(sprite);
		
		if (amount == -1) {
			sf::Vector2f size(sprite.getGlobalBounds().width, sprite.getGlobalBounds().height);
			sf::RectangleShape cover(size);
			cover.setOrigin(size/2.0f);
			cover.setFillColor(sf::Color::Yellow * sf::Color(255,255,255,100));
			cover.setPosition(position);
			window.draw(cover);
		}

//		window.draw(text);
	}
};

class Bomb : public Body {
	sf::Vector2f speed;
	unique_ptr<Bonus> bonus;
	ResourceManager& resources;
public:
	bool alive;
	Bomb(sf::Vector2f, sf::Vector2f, unique_ptr<Bonus>, ResourceManager&);
	Bomb(sf::Vector2f position, sf::Vector2f speed, ResourceManager& resources) : Bomb(position, speed, make_unique<DummyBonus>(resources), resources) {}	
	virtual void tick();
	virtual void hit(Space&);
	virtual void render(sf::RenderWindow& window) override {
		sprite.render(window);
		bonus->render(window, sprite.position);
	}
};


class Shot : public Body {	
public:
	bool alive;
	Shot(const HBTexture& texture) : Body(texture), alive(true) {};
	virtual void tick() {} ;
	virtual void hit(Space &sp) { };

};

class Projectile : public Shot {
	sf::Vector2f speed;
public:
	Projectile(sf::Vector2f, sf::Vector2f, ResourceManager&);
	void tick() override;
	void hit(Space &sp) override { alive = false; }
};

class RotatingGiant : public Shot {
	static constexpr int lifelength = 1000;
	int lifetime;
public:
	RotatingGiant(sf::Vector2f, ResourceManager&);
	void tick() override {
		if (lifetime <= 0) {
			alive = false;
			return;
		}
		sprite.rotate(4);
		sprite.setScale(double(lifetime)/lifelength);
		lifetime--;
	}
	void move(double x, double y) {
		sprite.move(x,y);
	}
	void hit(Space &sp) override {}
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
	void hit(Space &sp) override {}
};

class Player : public Body{
	sf::Vector2f speed;
	ResourceManager& resources;
public:
	unique_ptr<Gun> gun;
	Player(sf::Vector2f, sf::Vector2f, ResourceManager&);
	bool alive;
	void tick();
	void moveLimit(double, double, double);
	void move(float);
	void defaultGun();
	void shoot(Space&);
	void die(Space&);
};

class Particle {
	sf::Vector2f position, velocity;
	int lifetime;
	sf::Color color;
public:
	bool alive;
	Particle(sf::Vector2f position, sf::Vector2f velocity, sf::Color color) : position(position), velocity(velocity), lifetime(rand()%100), alive(true),
	color(color) {}
	void tick() { position += velocity; lifetime = max(lifetime-1, 0); if (lifetime == 0) alive = false;}
	void render(sf::RenderWindow& window) const {
		sf::Vertex line[2];
		line[0].position = position;
		line[1].position = position + sf::Vector2f(rand()%9-4, rand()%9-4);
		double b = (200 + rand()%55);
		//line[0].color = sf::Color(rand()%255, rand()%255, rand()%255, 255);
		//line[1].color = sf::Color(rand()%255, rand()%255, rand()%255, 255);
		line[0].color = color*sf::Color(b,b,b,255);
		line[1].color = color*sf::Color(b,b,b,255);
		window.draw(line, 2, sf::Lines);
	}
};

class Pistol : public Gun {
public:
	Pistol(int loaded, int interval, ResourceManager& resources) : Gun(loaded, interval, -1, resources) {}
	Pistol(int interval, ResourceManager& resources) : Pistol(0, interval, resources) {}
	void shoot(Space& space, Player& player) override{
		if(loaded == 0) {
			loaded = interval;
			space.shots.push_back(make_unique<Projectile>(player.sprite.getPosition(), sf::Vector2f(-4, -8), resources));
			space.shots.push_back(make_unique<Projectile>(player.sprite.getPosition(), sf::Vector2f( 4, -8), resources));
			resources.getSound("pow").play();
		}
	}
	void tick() override {
		loaded = std::max(0, loaded - 1);
	}
	void render(sf::RenderWindow& window, sf::Vector2f position) const {
		sf::Sprite sprite(resources.getTexture("pistol"));
		sprite.setOrigin(sprite.getGlobalBounds().width/2, sprite.getGlobalBounds().height/2);
		sprite.setPosition(position);
		window.draw(sprite);
	}
};	

class GiantGun : public Gun {
public:
	GiantGun(int loaded, int interval, int ammo, ResourceManager& resources) : Gun(loaded, interval, ammo,resources) {}
	GiantGun(int interval, int ammo, ResourceManager& resources) : GiantGun(0, interval, ammo,resources) {}
	GiantGun(ResourceManager& resources) : GiantGun(0, 0, 3,resources) {}
	void shoot(Space& space, Player& player) override {
		if (loaded == 0 && ammo != 0) {
			space.shots.push_back(make_unique<RotatingGiant>(sf::Vector2f(rand()%space.getWidth(), rand()%space.getHeight()),resources));
			loaded = interval;
			if (ammo > 0)
				ammo--;
		}
	}
	void tick() override {
		loaded = std::max(0, loaded-1);
	}
	void render(sf::RenderWindow& window, sf::Vector2f position) const {
		sf::Sprite sprite(resources.getTexture("giantgun"));
		sprite.scale(0.2, 0.2);
		sprite.setPosition(position);
		window.draw(sprite);
	}
};

class RailGun : public Gun {
public:
	RailGun(int loaded, int interval, int ammo, ResourceManager& resources) : Gun(loaded, interval, ammo, resources) {}
	void shoot(Space& space, Player& player) override {
		if (loaded == 0 && ammo != 0) {
			sf::Vector2f headPosition(player.sprite.getPosition().x, player.sprite.getPosition().y - player.sprite.getGlobalBounds().height*0.6);
			space.shots.push_back(make_unique<Laser>(headPosition, 30, 3, resources));
			space.shots.push_back(make_unique<Laser>(headPosition,-30, 3, resources));
			loaded = interval;
			if (ammo > 0)
				ammo--;
		}
	}
	void tick() override {
		loaded = std::max(0, loaded-1);
	}
	void render(sf::RenderWindow& window, sf::Vector2f position) const {
		sf::Sprite sprite(resources.getTexture("railgun"));
		sprite.setPosition(position);
		window.draw(sprite);
	}
};

Player::Player(sf::Vector2f position, sf::Vector2f speed, ResourceManager& resources) : Body(resources.getHBTexture("bunny")), speed(speed), resources(resources), gun(make_unique<Pistol>(50, resources)), alive(true) {
	sprite.setOrigin(sprite.getGlobalBounds().width/2.0, sprite.getGlobalBounds().height);
	sprite.scale(0.25);
	sprite.setPosition(position);
}

void Player::tick() {
	gun->tick();
}

void Player::shoot(Space& space) {
	if (gun->ammo == 0)
		defaultGun();
	gun->shoot(space, *this);
}

void Player::die(Space& space) {

	alive = false;
}


inline void Player::defaultGun() {
	gun = make_unique<Pistol>(10, 50, resources);
}
	

void Player::move(float multip) {
	sprite.move(multip*speed.x, 0);
}

void Player::moveLimit(double left, double right, double multip) {
	double px = sprite.getPosition().x;
	double newx = px + multip*speed.x;
	if (newx >= left && newx < right) 
		move(multip);
	else if (newx >= right) 
		sprite.setPosition(right-1, sprite.getPosition().y);
	else if (newx < left) 
		sprite.setPosition(left, sprite.getPosition().y);
}

Bomb::Bomb(sf::Vector2f position, sf::Vector2f speed, unique_ptr<Bonus> bonus, ResourceManager& resources) : speed(speed), bonus(move(bonus)), resources(resources), alive(true) {
	string textureNames[] = {"apple", "pear", "orange", "lemon"};
	sprite.setTexture(resources.getHBTexture(textureNames[rand()%4]));
	sprite.centerOrigin();
	//sprite.setOrigin(sprite.getGlobalBounds().width/2.0, sprite.getGlobalBounds().height/2.0);
	sprite.setPosition(position);
//	sprite.scale(0.2);
}

void Bomb::tick() {
	sprite.move(speed);
}

void Bomb::hit(Space& sp) {
	alive = false;
	bonus->apply(sp);
}


Projectile::Projectile(sf::Vector2f position, sf::Vector2f speed, ResourceManager& resources) : Shot(resources.getHBTexture("carrot")), speed(speed){
	sprite.centerOrigin();
	//sprite.setOrigin(sprite.getGlobalBounds().width/2.0, sprite.getGlobalBounds().height/2.0);
	sprite.setPosition(position);
//	sprite.setScale(0.2, 0.2);
	sprite.scale(0.2);
	sprite.rotate(atan2(speed.y, speed.x)/3.1415926535*180);
}

void Projectile::tick() {
	//mozna relativne s woknem
	sprite.move(speed);
}

RotatingGiant::RotatingGiant(sf::Vector2f position, ResourceManager& resources) : Shot(resources.getHBTexture("carrot")), lifetime(lifelength) {
	sprite.centerOrigin();
	//sprite.setOrigin(sprite.getGlobalBounds().width/2.0, sprite.getGlobalBounds().height/2.0);
	sprite.setPosition(position);
}

Laser::Laser(sf::Vector2f position, double angle, int lifetime, ResourceManager& resources) : Shot(resources.getHBTexture("beam")), lifetime(lifetime) {
	sprite.setOrigin(sprite.getGlobalBounds().width/2.0, sprite.getGlobalBounds().height);
	sprite.setPosition(position);
	sprite.rotate(angle);
}

bool Space::running() const {
	return players[0]->alive;
}

void Space::rescale(const sf::RenderWindow& window) {
/*	float wwidth = window.getView().getSize().y;
	float wheight = window.getView().getSize().x;
	for (int i = 0; i < ground_num; ++i){
		float fct = wwidth/(ground_num*ground[i].getGlobalBounds().width);
		ground[i].scale(fct,fct);
		ground[i].setPosition(sf::Vector2f(wwidth/ground_num * i, wheight));
	}*/
}

void Space::explosion(sf::Vector2f position) {
	sf::Color clr(rand()%255, rand()%255, rand()%255);
	for (int i = 0; i < 100; ++i) {
		double speed = rand()%500;
		double angle = rand()%360 / 180.0 * 3.14159265358979323;
		double vx = speed * cos(angle);
		double vy = speed * sin(angle);
		sf::Vector2f velocity(vx, vy);
		particles.push_back(Particle(position, velocity/100.0f, clr));
	}
}

inline void Space::gunToPlayer(unique_ptr<Gun> gun) {
	players[0]->gun = move(gun);
}


void Space::fixAll() {
	for (int i = 0; i < ground_num; i++)
		ground_alive[i] = true;
}

void Space::fixOne() {
	int ground_index = players[0]->sprite.getPosition().x/groundSize();
	int left = ground_index;
	int right = ground_index;

	while (left > 0 && ground_alive[left-1]) left--;
	while (right < ground_num - 1 && ground_alive[right + 1]) right++;

	if (left == 0) left = -1234;
	if (right == ground_num - 1) right = 1234;

	if (left == -1234 && right == 1234)
		return;
	
	if (ground_index - left <= right - ground_index)
		ground_alive[left - 1] = true;
	else
		ground_alive[right + 1] = true;
}

void Space::movePlayer(double multip) {
	int ground_index = players[0]->sprite.getPosition().x/groundSize();
	int left = ground_index;
	int right = ground_index;
	while (left > 0 && ground_alive[left-1]) left--;
	while (right < ground_num - 1 && ground_alive[right + 1]) right++;
	
	players[0]->moveLimit(left*groundSize(), (right+1)*groundSize(), multip);

}	

void Space::render(sf::RenderWindow& window) const {
	//window dimensions
	window.draw(background);
	for (int i = 0; i < ground_num; ++i) {
		if (ground_alive[i])
			window.draw(ground[i]);
	}
	for (const auto& particle : particles)
		particle.render(window);
	for (const auto& bomb : bombs)
		bomb->render(window);
	for (const auto& shot : shots)
		shot->render(window);
	for (const auto& player : players)
		player->render(window);

	int interval = players[0]->gun->interval;
	int current = interval - players[0]->gun->loaded;
	double ratio = (double)current/interval;
	sf::RectangleShape gunLoaded(sf::Vector2f(100*ratio, 10));
	gunLoaded.setFillColor(sf::Color((1-ratio)*255, ratio*255, 0, 180));
	gunLoaded.setPosition(width - 100 - 5, height - 15);
	window.draw(gunLoaded);
	int ammo = players[0]->gun->ammo;
	sf::Text ammoText(ammo == -1 ? "999" : to_string(ammo) , resources.getFont("cmu"), 20);
	ammoText.setPosition(width - 50 - 5 - 7, height - 38);
	window.draw(ammoText);

}

void Space::renderEnding(sf::RenderWindow& window) const {
	render(window);
	sf::Text rip("RIP :-(", resources.getFont("cmu"), 120);
	rip.setOrigin(rip.getGlobalBounds().width/2.0, rip.getGlobalBounds().height/2.0);
	rip.setPosition(width/2.0, height/2.0);
	window.draw(rip);
}

void Space::tick() {
	if (rand() % 100 < 4) {
		//bombs have origin in width/2, height/2, therefore + 1/2 ground_block so that they are in the middle
		double x = (rand()%ground_num + 0.5) * groundSize();
		//bombs.push_back(make_unique<Bomb>(sf::Vector2f(x, 0), sf::Vector2f(0, 2), make_unique<DummyBonus>(resources), resources));
		//bombs.push_back(make_unique<Bomb>(sf::Vector2f(x, 0), sf::Vector2f(0, 2), make_unique<GunBonus>(make_unique<Pistol>(20, resources), resources), resources));
		unique_ptr<Bonus> bonus = make_unique<DummyBonus>(resources);
		if (rand() % 100 < 5) 
			bonus = make_unique<GunBonus>(make_unique<RailGun>(0, 20, 10, resources), resources);
		else if (rand() % 100 < 5)
			bonus = make_unique<FixBonus>(1, resources);
		else if (rand() % 100 < 1)
			bonus = make_unique<FixBonus>(-1, resources);
		else if (rand() % 100 < 2)
			bonus = make_unique<GunBonus>(make_unique<GiantGun>(10, 3, resources), resources);

		bombs.push_back(make_unique<Bomb>(sf::Vector2f(x, 0), sf::Vector2f(0, 3), move(bonus), resources));
	}
	for (auto& bomb : bombs) 
		bomb->tick();
	for (auto& shot : shots) 
		shot->tick();
	for (auto& particle : particles) 
		particle.tick();
	for (auto& player : players)
		player->tick();


	auto bomb = bombs.begin();
	while (bomb != bombs.end()) {
		auto shot = shots.begin();
		bool destroyed = false;
		while (shot != shots.end()) {
			if(!(*shot)->alive) {
				shot++;
				continue;
			}

			if((*bomb)->collides(*(*shot))) {
//			if ((*bomb)->sprite.getGlobalBounds().intersects((*shot)->sprite.getGlobalBounds())) {
				explosion((*bomb)->sprite.getPosition());
				(*bomb)->hit(*this);
				bombs.erase(bomb++);
				(*shot)->hit(*this);
//				shots.erase(shot);
				destroyed = true;
				break;
			}
			else 
				shot++;
		}
		if (!destroyed) {
			auto player = players.begin();
			while (player != players.end()) {
				if((*bomb)->collides(*(*player))) {
					cout << "DEAD" << endl;
					(*player)->die(*this);
				}
				player++;
			}
			bomb++;
		}
	}


	auto it = bombs.begin();
	while (it != bombs.end()) {
		int ground_index = (*it)->sprite.getPosition().x/groundSize();
		bool touch_ground = (*it)->sprite.getPosition().y + (*it)->sprite.getGlobalBounds().height - (*it)->sprite.getOrigin().y > height - groundSize()*ground_alive[ground_index];
		if (touch_ground && ground_alive[ground_index]) 
			ground_alive[ground_index] = false;
		if (!(*it)->alive || touch_ground)
			bombs.erase(it++);
		else
			it++;
	}

	auto it2 = shots.begin();
	while (it2 != shots.end()) {
		if (!(*it2)->alive || (*it2)->sprite.getPosition().y < 0 || (*it2)->sprite.getPosition().x < 0 || (*it2)->sprite.getPosition().x > width)
			shots.erase(it2++);
		else
			it2++;
	}

	auto it3 = particles.begin();
	while (it3 != particles.end()) {
		if ((*it3).alive)
			it3++;
		else
			particles.erase(it3++);
	}

}

Space::Space(int width, int height, ResourceManager& resources) : width(width), height(height), ground_num(30), resources(resources), background(resources.getTexture("background")) {	
	double rat = (double)height/background.getGlobalBounds().height;
	background.setScale(rat,rat);
	for (int i = 0; i < ground_num; ++i) {
		ground_alive.push_back(true);
		ground.emplace_back(resources.getTexture("block"));
		double ratio = (double)groundSize()/resources.getTexture("block").getSize().x;
		ground[i].setScale(ratio, ratio);
		ground[i].setOrigin(0, ground[i].getGlobalBounds().height-ground[i].getGlobalBounds().width);
		ground[i].setPosition(i*groundSize(), height-groundSize());
	}
	players.push_back(make_unique<Player>(sf::Vector2f(width/2, height-groundSize()), sf::Vector2f(10, 0),resources));
	resources.getSound("background").setLoop(true);
	resources.getSound("background").play();
}

bool loadData(ResourceManager& resources) {
#define cload(type, name, path) if (!resources.load ##type(name, path)) { cout << "Failed to load " << #type << ", path: " << path << endl; return false; }
	cload(HBTexture, "apple", "svg/apple.png");
	cload(HBTexture, "lemon", "svg/lemon.png");
	cload(HBTexture, "orange", "svg/orange.png");
	cload(HBTexture, "pear", "svg/pear.png");
	cload(HBTexture, "bunny", "svg/bunny.png");
	cload(HBTexture, "beam", "svg/beam.png");
	cload(HBTexture, "carrot", "svg/carrot.png");
	cload(Texture, "carrot", "svg/carrot.png");
	cload(Texture, "background", "svg/background.png");
	cload(Texture, "block", "svg/block.png");
	cload(Texture, "pistol", "svg/pistol.png");
	cload(Texture, "giantgun", "svg/giantgun.png");
	cload(Texture, "railgun", "svg/railgun.png");
	cload(Sound, "pow", "sounds/pow.wav");
	cload(Sound, "background", "sounds/bceq.wav");
	cload(Font, "cmu", "cmunrm.otf");

	return true;
#undef cload
}

int main(void) {
//	sf::err().rdbuf(NULL);
	srand(time(NULL));

	ResourceManager resources;
	if (!loadData(resources)) {
		cout << "Exiting." << endl;
		return -1;
	}



//...!
#define ws 90

	Space sp(16*ws, 9*ws, resources);

	sf::RenderWindow window(sf::VideoMode(16*ws, 9*ws), "Fruit fever", sf::Style::Close);
	window.setVerticalSyncEnabled(true);
	sf::View view(sf::FloatRect(0,0,16*ws,9*ws));
	
	//bg.setScale(0.5, 0.5);

		
	window.setView(view);
	sp.rescale(window);

	

	while (window.isOpen() && sp.running()) {
		sf::Event event;
		while (window.pollEvent(event)) {
			if (event.type == sf::Event::Closed)
				window.close();
		}
		if(sf::Keyboard::isKeyPressed(sf::Keyboard::Left)){
			sp.movePlayer(-1);
                }       
                if(sf::Keyboard::isKeyPressed(sf::Keyboard::Right)){
			sp.movePlayer(1);
                }       
                if(sf::Keyboard::isKeyPressed(sf::Keyboard::Space)){
                        sp.players[0]->shoot(sp);
			//sp.shots.push_back(make_unique<RotatingGiant>(sf::Vector2f(400, 400)));
			//backs.setPitch(backs.getPitch()*1.0595);
			//pow.play();
                }
		if(sf::Keyboard::isKeyPressed(sf::Keyboard::S)){
			//sp.gunToPlayer(make_unique<GiantGun>(10,3));
                }       
                if(sf::Keyboard::isKeyPressed(sf::Keyboard::R)){
			//sp.gunToPlayer(make_unique<RailGun>(0,20, -1));
                }       
                if(sf::Keyboard::isKeyPressed(sf::Keyboard::A)){
                }
                if(sf::Keyboard::isKeyPressed(sf::Keyboard::W)){
                }

		window.clear();
		sp.render(window);
		window.display();

		sp.tick();
	}

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

