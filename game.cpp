#include <iostream>
#include <cstdlib>
#include <ctime>
#include "game.hpp"

void FixBonus::apply(Game& game) {
	if (amount == -1)
		game.fixAll();
	else {
		for(int i = 0; i < amount; ++i)
			game.fixOne();
	}
}

//FixBonus is render as block
//if FixBonus fixes everything, the block is overlayed by a semitransparent yellow rectangle
void FixBonus::render(sf::RenderWindow& window, sf::Vector2f position) const {
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
}

//GunBonus is rendered as the corresponding gun
void GunBonus::render(sf::RenderWindow& window, sf::Vector2f position) const { 
	gun->render(window, position);
}

//Player texture is 'bunny', has Pistol with interval 50 and infinity ammo
Player::Player(sf::Vector2f position, sf::Vector2f speed, ResourceManager& resources) : Body(resources.getHBTexture("bunny")), speed(speed), resources(resources), gun(std::make_unique<Pistol>(0, 50, -1, resources)), alive(true) {
	sprite.setOrigin(sprite.getGlobalBounds().width/2.0, sprite.getGlobalBounds().height);
	sprite.scale(0.25);
	sprite.setPosition(position);
}

void Player::tick() {
	gun->tick();
}

//shoots if current gun is not empty, otherwise falls back to the default and shoots it
void Player::shoot(Game& game) {
	if (gun->ammo == 0)
		defaultGun();
	gun->shoot(game, *this);
}

//Default gun is almost loaded pistol (10 ticks from loaded) with interval 50 and infinity ammo
void Player::defaultGun() {
	gun = std::make_unique<Pistol>(10, 50, -1, resources);
}


void Player::moveLimit(double left, double right, double multip) {
	double px = sprite.getPosition().x;
	//Calculates the desired position
	double newx = px + multip*speed.x;

	//If between left and right, move there, otherwise move to the corresponding end of interval
	if (newx >= left && newx < right) 
		move(multip);
	else if (newx >= right) 
		sprite.setPosition(right-1, sprite.getPosition().y);
	else if (newx < left) 
		sprite.setPosition(left, sprite.getPosition().y);
}

//Creates a bomb with random texture (apple, pear, orange or lemon)
Bomb::Bomb(sf::Vector2f position, sf::Vector2f speed, std::unique_ptr<Bonus> bonus, ResourceManager& resources) : speed(speed), bonus(move(bonus)), resources(resources), alive(true) {
	std::string textureNames[] = {"apple", "pear", "orange", "lemon"};
	sprite.setTexture(resources.getHBTexture(textureNames[rand()%4]));
	sprite.centerOrigin();
	sprite.setPosition(position);
}

//Projectile texture is carrot, sprite is rotated to be parralel with speed vector
Projectile::Projectile(sf::Vector2f position, sf::Vector2f speed, ResourceManager& resources) : Shot(resources.getHBTexture("carrot")), speed(speed){
	sprite.centerOrigin();
	sprite.setPosition(position);
	sprite.scale(0.2);
	sprite.rotate(atan2(speed.y, speed.x)/3.1415926535*180);
}

//RotatingGiant texture is carrot
RotatingGiant::RotatingGiant(sf::Vector2f position, int lifelength, ResourceManager& resources) : Shot(resources.getHBTexture("carrot")), lifelength(lifelength), lifetime(lifelength) {
	sprite.centerOrigin();
	sprite.setPosition(position);
}

//Laser texture is beam, beam is oriented to given angle
Laser::Laser(sf::Vector2f position, double angle, int lifetime, ResourceManager& resources) : Shot(resources.getHBTexture("beam")), lifetime(lifetime) {
	sprite.setOrigin(sprite.getGlobalBounds().width/2.0, sprite.getGlobalBounds().height);
	sprite.setPosition(position);
	sprite.rotate(angle);
}

void RotatingGiant::tick() {
	if (lifetime <= 0) {
		alive = false;
		return;
	}
	sprite.rotate(4);
	
	//sprite is scaled cubically with lifetime/lifelength ratio 
	double ratio = (double)lifetime/lifelength;
	sprite.setScale(ratio*ratio*ratio);
	lifetime--;
}

//Pistol spawns two Projectiles flying from player's headposition (bunny's ears)
//Makes 'pow' sound
void Pistol::shoot(Game& game, Player& player) {
	if (loaded == 0 && ammo != 0) {
		loaded = interval;
		sf::Vector2f headPosition(player.sprite.getPosition().x, player.sprite.getPosition().y - player.sprite.getGlobalBounds().height*0.6);
		game.shots.push_back(std::make_unique<Projectile>(headPosition, sf::Vector2f(-4, -8), resources));
		game.shots.push_back(std::make_unique<Projectile>(headPosition, sf::Vector2f( 4, -8), resources));
		resources.getSound("pow").play();
		if (ammo > 0)
			ammo--;
	}
}

//Pistol renders as texture 'pistol'
void Pistol::render(sf::RenderWindow& window, sf::Vector2f position) const {
	sf::Sprite sprite(resources.getTexture("pistol"));
	sprite.setOrigin(sprite.getGlobalBounds().width/2, sprite.getGlobalBounds().height/2);
	sprite.setPosition(position);
	window.draw(sprite);
}

//Spawns a RotatingGiant with 1000 lifetime on a random position within game dimensions
//Makes 'buch' sound
void GiantGun::shoot(Game& game, Player& player) {
	if (loaded == 0 && ammo != 0) {
		game.shots.push_back(std::make_unique<RotatingGiant>(sf::Vector2f(rand()%game.getWidth(), rand()%game.getHeight()), 1000, resources));
		loaded = interval;
		if (ammo > 0)
			ammo--;
	}
	resources.getSound("buch").play();
}

//GiantGun renders as texture 'giantgun'
void GiantGun::render(sf::RenderWindow& window, sf::Vector2f position) const {
	sf::Sprite sprite(resources.getTexture("giantgun"));
	sprite.setOrigin(sprite.getGlobalBounds().width/2, sprite.getGlobalBounds().height/2);
	sprite.scale(0.15, 0.15);
	sprite.setPosition(position);
	window.draw(sprite);
}

//Spawns two laser beams originating at players head (from bunny's ears)
//Makes 'railgun' sound
void RailGun::shoot(Game& game, Player& player) {
	if (loaded == 0 && ammo != 0) {
		sf::Vector2f headPosition(player.sprite.getPosition().x, player.sprite.getPosition().y - player.sprite.getGlobalBounds().height*0.6);
		game.shots.push_back(std::make_unique<Laser>(headPosition, 30, 3, resources));
		game.shots.push_back(std::make_unique<Laser>(headPosition,-30, 3, resources));
		loaded = interval;
		resources.getSound("railgun").play();
		if (ammo > 0)
			ammo--;
	}
}

//RailGun renders as texture 'railgun'
void RailGun::render(sf::RenderWindow& window, sf::Vector2f position) const {
	sf::Sprite sprite(resources.getTexture("railgun"));
	sprite.setOrigin(sprite.getGlobalBounds().width/2, sprite.getGlobalBounds().height/2);
	sprite.setPosition(position);
	window.draw(sprite);
}


//Particle renders as a line with one end at position and the other at a random position
//((-4,4), (-4,4)) from that end
//rendered color is randomly fluctuating around 'color' (brightness is random)
void Particle::render(sf::RenderWindow& window) const {
	sf::Vertex line[2];
	line[0].position = position;
	line[1].position = position + sf::Vector2f(rand()%9-4, rand()%9-4);
	double b = (200 + rand()%55);
	line[0].color = color*sf::Color(b,b,b,255);
	line[1].color = color*sf::Color(b,b,b,255);
	window.draw(line, 2, sf::Lines);
}

bool Game::running() const {
	return players[0]->alive;
}

//Spawns num of particles flying from position
void Game::explosion(sf::Vector2f position, int num) {
	sf::Color clr(rand()%255, rand()%255, rand()%255);
	for (int i = 0; i < num; ++i) {
		//Particles should make a 'circle' - each one is randomly assigned
		//a speed and an angle, vx and vy are calculated from that
		double speed = rand()%500;
		double angle = rand()%360 / 180.0 * 3.14159265358979323;
		double vx = speed * cos(angle);
		double vy = speed * sin(angle);

		sf::Vector2f velocity(vx, vy);
		particles.push_back(Particle(position, velocity/100.0f, clr));
	}
}

void Game::gunToPlayer(std::unique_ptr<Gun> gun) {
	players[0]->gun = move(gun);
}

void Game::shoot() {
	players[0]->shoot(*this);
}

void Game::fixAll() {
	for (int i = 0; i < ground_num; i++)
		ground_alive[i] = true;
}

void Game::fixOne() {
	//Index of the ground block that is under players[0]'s location
	int ground_index = players[0]->sprite.getPosition().x/groundSize();
	int left = ground_index;
	int right = ground_index;

	//Calculates the furthest not-broken block to the left and right
	while (left > 0 && ground_alive[left-1]) left--;
	while (right < ground_num - 1 && ground_alive[right + 1]) right++;

	//-1234 and 1234 means all left/right blocks are not-broken
	if (left == 0) left = -1234;
	if (right == ground_num - 1) right = 1234;

	//Nothing to fix
	if (left == -1234 && right == 1234)
		return;
	
	//Fixes the closest one
	if (ground_index - left <= right - ground_index)
		ground_alive[left - 1] = true;
	else
		ground_alive[right + 1] = true;
}

void Game::movePlayer(double multip) {
	//Calculates left and right player movement bounds (furthest not-broken blocks)
	int ground_index = players[0]->sprite.getPosition().x/groundSize();
	int left = ground_index;
	int right = ground_index;
	while (left > 0 && ground_alive[left-1]) left--;
	while (right < ground_num - 1 && ground_alive[right + 1]) right++;

	players[0]->moveLimit(left*groundSize(), (right+1)*groundSize(), multip);

}	

void Game::render(sf::RenderWindow& window) const {
	window.draw(background);

	//Not-broken ground blocks
	for (int i = 0; i < ground_num; ++i) {
		if (ground_alive[i])
			window.draw(ground[i]);
	}

	//Renders all primitives
	for (const auto& particle : particles)
		particle.render(window);

	for (const auto& bomb : bombs)
		bomb->render(window);

	for (const auto& shot : shots)
		shot->render(window);

	for (const auto& player : players) {
		if(player->alive)
			player->render(window);
	}

	//Gun information render
	int interval = players[0]->gun->interval;
	int current = interval - players[0]->gun->loaded;
	//Ratio of time-to-load/total-loadin-time
	double ratio = (double)current/interval;

	//Draws a rectangle that is ratio*100 wide at the bottom right corner
	//Goes from red to green (unloaded->loaded)
	sf::RectangleShape gunLoaded(sf::Vector2f(100*ratio, 10));
	gunLoaded.setFillColor(sf::Color((1-ratio)*255, ratio*255, 0, 180));
	gunLoaded.setPosition(width - 100 - 5, height - 15);
	window.draw(gunLoaded);

	//Magazine - ammo information, infinite magazine is 999
	//Renders a text at the bottom right corner
	int ammo = players[0]->gun->ammo;
	sf::Text ammoText(ammo == -1 ? "999" : std::to_string(ammo) , resources.getFont("cmu"), 20);
	ammoText.setPosition(width - 50 - 5 - 7, height - 38);
	window.draw(ammoText);


	//Score text, top middle
	sf::Text scoreText("Score: " + std::to_string(score), resources.getFont("cmu"), 60);
	scoreText.setOrigin(scoreText.getGlobalBounds().width/2.0, 0);
	scoreText.setPosition(width/2.0, 5);
	window.draw(scoreText);

}

//Renders gamestate and GAMEOVER text over that
//Particles are still moving
void Game::renderEnding(sf::RenderWindow& window) {
	//Space is no longer ticking, but explosions should still move
	for (auto& particle : particles) 
		particle.tick();
	
	render(window);

	sf::Text rip("GAME OVER :-(", resources.getFont("cmu"), 120);
	rip.setOrigin(rip.getGlobalBounds().width/2.0, rip.getGlobalBounds().height/2.0);
	rip.setPosition(width/2.0, height/2.0);
	window.draw(rip);
}

//Hand-crafted game difficulty curve, mapping from score to level
int Game::scoreToLevel() const {
	if (score < 10)
		return 1;
	if (score < 20)
		return 2;
	if (score < 30)
		return 3;
	if (score < 50)
		return 4;
	if (score < 80)
		return 6;
	if (score < 120)
		return 7;
	if (score < 160)
		return 8;
	return score/40 + 5;
}

void Game::tick() {
	level = scoreToLevel();

	//transposes the background music for the new level
	if (level != oldLevel)
		resources.getSound("background").setPitch(resources.getSound("background").getPitch() * 1.0595);

	oldLevel = level;

	//Chance of spawnig a bomb in a tick is proportional to level
	if (rand() % 100 < level) {
		//Bomb spawns over a random ground block
		double x = (rand()%ground_num + 0.5) * groundSize();
		
		std::unique_ptr<Bonus> bonus = std::make_unique<DummyBonus>(resources);
		//Carefully hand-crafted various bonuses spawnrates 
		if (rand() % 100 < 4) 
			bonus = std::make_unique<GunBonus>(std::make_unique<RailGun>(0, 20, 10, resources), resources);
		else if (rand() % 100 < 5)
			bonus = std::make_unique<FixBonus>(1, resources);
		else if (rand() % 100 < 1)
			bonus = std::make_unique<FixBonus>(-1, resources);
		else if (rand() % 100 < 2)
			bonus = std::make_unique<GunBonus>(std::make_unique<GiantGun>(10, 1 , resources), resources);
		else if (rand() % 100 < 2)
			bonus = std::make_unique<GunBonus>(std::make_unique<Pistol>(0, 5, 50, resources), resources);
		
		//Bomb speed is proportional to level
		bombs.push_back(std::make_unique<Bomb>(sf::Vector2f(x, 0), sf::Vector2f(0, 3 + level/5.0), move(bonus), resources));
	}

	//Individual primitives' ticks
	for (auto& bomb : bombs) 
		bomb->tick();
	for (auto& shot : shots) 
		shot->tick();
	for (auto& particle : particles) 
		particle.tick();
	for (auto& player : players)
		player->tick();


	//Bomb x (Shot/Player) collision resolution
	auto bomb = bombs.begin();
	while (bomb != bombs.end()) {
		//Every bomb with every shot
		auto shot = shots.begin();
		bool destroyed = false;
		while (shot != shots.end()) {
			if(!(*shot)->alive) {
				shot++;
				continue;
			}
			//If bomb collides with shot
			if((*bomb)->collides(*(*shot))) {
				explosion((*bomb)->sprite.getPosition(), 100);

				(*bomb)->hit(*this);
				bombs.erase(bomb++);

				(*shot)->hit(*this);

				score++;
				destroyed = true;
				break;
			}
			else 
				shot++;
		}
		
		if (!destroyed) {
			//Not-destroyed bombs with every player
			auto player = players.begin();
			while (player != players.end()) {
				if((*bomb)->collides(*(*player))) {
					//Change music and trigger player die()
					resources.getSound("background").stop();
					resources.getSound("gameover").play();
					(*player)->die(*this);
				}
				player++;
			}
			bomb++;
		}
	}

	//Bomb hitting ground (bottom end of the game window) resolution 
	auto it = bombs.begin();
	while (it != bombs.end()) {
		//Calculates the corresponding block index
		int ground_index = (*it)->sprite.getPosition().x/groundSize();

		//represents either bombtouching a not-destroyed block or bomb touching the bottom end of the game window
		bool touch_ground = (*it)->sprite.getPosition().y + (*it)->sprite.getGlobalBounds().height - (*it)->sprite.getOrigin().y > height - groundSize()*ground_alive[ground_index];

		if (touch_ground && ground_alive[ground_index]) 
			ground_alive[ground_index] = false;

		if (!(*it)->alive || touch_ground)
			bombs.erase(it++);
		else
			it++;
	}

	//Destroys out of screen shots
	auto it2 = shots.begin();
	while (it2 != shots.end()) {
		if (!(*it2)->alive || (*it2)->sprite.getPosition().y < 0 || (*it2)->sprite.getPosition().x < 0 || (*it2)->sprite.getPosition().x > width)
			shots.erase(it2++);
		else
			it2++;
	}

	//Removes dead particles
	auto it3 = particles.begin();
	while (it3 != particles.end()) {
		if ((*it3).alive)
			it3++;
		else
			particles.erase(it3++);
	}

}

//Initializes a new game with width x height dimensions
//background texture is "background', number of blocks is 30, level is 1
Game::Game(int width, int height, ResourceManager& resources) : ground_num(30), width(width), height(height), background(resources.getTexture("background")), score(0), level(1), oldLevel(1), resources(resources) {	
	srand(time(NULL));
	
	//Scales background to fit height
	double rat = (double)height/background.getGlobalBounds().height;
	background.setScale(rat,rat);

	//Scales and positions ground blocks
	double ratio = (double)groundSize()/resources.getTexture("block").getSize().x;
	for (int i = 0; i < ground_num; ++i) {
		ground_alive.push_back(true);
		ground.emplace_back(resources.getTexture("block"));
		ground[i].setScale(ratio, ratio);
		ground[i].setOrigin(0, ground[i].getGlobalBounds().height-ground[i].getGlobalBounds().width);
		ground[i].setPosition(i*groundSize(), height-groundSize());
	}

	//Only one player is spawned. In the middle and with speed 10
	players.push_back(std::make_unique<Player>(sf::Vector2f(width/2, height-groundSize()), sf::Vector2f(10, -1),resources));

	resources.getSound("background").setLoop(true);
	resources.getSound("background").setVolume(80);
	resources.getSound("background").play();
}

