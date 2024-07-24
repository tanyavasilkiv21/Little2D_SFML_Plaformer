#pragma once

#include "SFML/Graphics.hpp"
#include "Animation.h"

class Assets
{
	std::map<std::string, sf::Texture> m_textures;
	std::map<std::string, Animation> m_animation;
	//std::map<std::string, sf::Sound> m_sounds;
	std::map<std::string, sf::Font> m_fonts;
public:

	void addTexture(std::string name, std::string path);
	void addAnimation(std::string name, std::string t, std::string path, int countFrames);
	//void addSound(std::string name, std::string path);
	void addFont(std::string name, std::string path);

	const sf::Texture& getTexture(std::string name) const;
	const Animation& getAnimation(std::string name) const;
	//const sf::Sound& getSound(std::string name) const;
	const sf::Font& getFont(std::string name) const;

	void loadFromFile(const std::string& path);
};

