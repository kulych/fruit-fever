//includy tady i v headru?
#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <string>
#include "resources.hpp"
#include "hitbox.hpp"

#include <iostream>

ResourceManager::ResourceManager() {}

const HBTexture& ResourceManager::getHBTexture(const std::string& name) const {
	try { 
		return hbtextures.at(name);
	}
	catch (std::out_of_range& x) {
		throw std::out_of_range("HBTexture " + name + " not loaded!");
	}
}

const sf::Texture& ResourceManager::getTexture(const std::string& name) const {
	try{
		return textures.at(name);
	}
	catch (std::out_of_range& x) {
		throw std::out_of_range("Texture " + name + " not loaded!");
	}
}

const sf::Font& ResourceManager::getFont(const std::string& name) const {
	try {
		return fonts.at(name);
	}
	catch (std::out_of_range& x) {
		throw std::out_of_range("Font " + name + " not loaded!");
	}
}

sf::Sound& ResourceManager::getSound(const std::string& name) {
	try {
		return sounds.at(name);
	}
	catch (std::out_of_range& x) {
		throw std::out_of_range("Sound " + name + " not loaded!");
	}
}

bool ResourceManager::loadHBTexture(const std::string& key, const std::string& path) {
	if (!hbtextures[key].loadFromFile(path))
		return false;
	hbtextures[key].setSmooth(true);
	return true;
}

bool ResourceManager::loadTexture(const std::string& key, const std::string& path) {
	if (!textures[key].loadFromFile(path))
		return false;
	hbtextures[key].setSmooth(true);
	return true;
}

bool ResourceManager::loadFont(const std::string& key, const std::string& path) {
	return fonts[key].loadFromFile(path);
}

bool ResourceManager::loadSound(const std::string& key, const std::string& path) {
	if (!soundBuffers[key].loadFromFile(path))
		return false;
	sounds[key] = sf::Sound(soundBuffers[key]);
	return true;
}
