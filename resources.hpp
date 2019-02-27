#ifndef __RESOURCES_HPP
#define __RESOURCES_HPP

#include <SFML/Audio.hpp>
#include <unordered_map>
#include <string>
#include "hitbox.hpp"


struct ResourceManager {
	std::unordered_map<std::string, HBTexture> hbtextures;
	std::unordered_map<std::string, sf::Texture> textures;
	std::unordered_map<std::string, sf::Sound> sounds;
	std::unordered_map<std::string, sf::SoundBuffer> soundBuffers;
	std::unordered_map<std::string, sf::Font> fonts;
public:
	ResourceManager();
	const HBTexture& getHBTexture(const std::string&) const;
	const sf::Texture& getTexture(const std::string&) const;
	const sf::Font& getFont(const std::string&) const;
	sf::Sound& getSound(const std::string&);
	//loads and makes Smooth
	bool loadTexture(const std::string&, const std::string&);	
	bool loadHBTexture(const std::string&, const std::string&);	
	bool loadSound(const std::string&, const std::string&);	
	bool loadFont(const std::string&, const std::string&);
};

#endif
