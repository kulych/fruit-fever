//includy tady i v headru?
#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <string>
#include "resources.hpp"
#include "hitbox.hpp"

#include <iostream>

ResourceManager::ResourceManager() {}

//nonexistent -> exception?
//at misto [] protoze, [] nevraci const ref.
const HBTexture& ResourceManager::getHBTexture(const std::string& name) const {
	try { 
		return hbtextures.at(name);
	}
	catch (...){
		std::cout << "nemam HBtexturu " << name << std::endl;
		return hbtextures.at("beam");
	}
}

const sf::Texture& ResourceManager::getTexture(const std::string& name) const {
	return textures.at(name);
}

const sf::Font& ResourceManager::getFont(const std::string& name) const {
	return fonts.at(name);
}

sf::Sound& ResourceManager::getSound(const std::string& name) {
	return sounds.at(name);
}

void ResourceManager::loadHBTexture(const std::string& key, const std::string& path) {
	hbtextures[key].loadFromFile(path);
	hbtextures[key].setSmooth(true);
}

void ResourceManager::loadTexture(const std::string& key, const std::string& path) {
	textures[key].loadFromFile(path);
	hbtextures[key].setSmooth(true);
}

void ResourceManager::loadFont(const std::string& key, const std::string& path) {
	fonts[key].loadFromFile(path);
}

void ResourceManager::loadSound(const std::string& key, const std::string& path) {
	soundBuffers[key].loadFromFile(path);
	sounds[key] = sf::Sound(soundBuffers[key]);
}
