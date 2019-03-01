#ifndef __RESOURCES_HPP
#define __RESOURCES_HPP

#include <SFML/Audio.hpp>
#include <unordered_map>
#include <string>
#include "hitbox.hpp"

//ResourceManager encapsulates multimedia, allows loading and solves requests
struct ResourceManager {
	std::unordered_map<std::string, HBTexture> hbtextures;
	std::unordered_map<std::string, sf::Texture> textures;
	std::unordered_map<std::string, sf::Sound> sounds;
	std::unordered_map<std::string, sf::SoundBuffer> soundBuffers;
	std::unordered_map<std::string, sf::Font> fonts;
public:
	ResourceManager();
	//getSomething functions throw if asked for a nonexistent entry
	//getSomething("name")
	const HBTexture& getHBTexture(const std::string&) const;
	const sf::Texture& getTexture(const std::string&) const;
	const sf::Font& getFont(const std::string&) const;
	sf::Sound& getSound(const std::string&);

	//load functions return false if loading failed, true otherwise 
	//loadSomething("name", "path to file.png")
	bool loadTexture(const std::string&, const std::string&);	
	bool loadHBTexture(const std::string&, const std::string&);	
	bool loadSound(const std::string&, const std::string&);	
	bool loadFont(const std::string&, const std::string&);
};

#endif
