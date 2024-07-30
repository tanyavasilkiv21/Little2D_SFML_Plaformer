#include "Assets.h"
#include <fstream>
#include <sstream>
void Assets::addTexture(std::string name, std::string path)
{
	sf::Texture texture;
	texture.loadFromFile(path);
	m_textures[name] = texture;
}

void Assets::addAnimation(std::string name, std::string t, int countFrames, int speed)
{
	Animation animation(name, getTexture(t), countFrames, speed);
	m_animation[name] = animation;
}

void Assets::addFont(std::string name, std::string path)
{
	sf::Font f;
	f.loadFromFile(path);
	m_fonts[name] = f;
}

const sf::Texture& Assets::getTexture(std::string name) const
{
	auto find = m_textures.find(name);
	if (find != m_textures.end()) {
		return find->second;
	}
	else
	{
		sf::Texture emptyTexture;
		return emptyTexture;
	}
}

const Animation& Assets::getAnimation(std::string name) const
{
	auto find = m_animation.find(name);
	if (find != m_animation.end()) {
		return find->second;
	}
	else
	{
		Animation emptyAnimation;
		return emptyAnimation;
	}
}

const sf::Font& Assets::getFont(std::string name) const
{
	auto find = m_fonts.find(name);
	if (find != m_fonts.end()) {
		return find->second;
	}
	else
	{
		sf::Font emptyFont;
		return emptyFont;
	}
}

void Assets::loadFromFile(const std::string& path)
{
	std::ifstream fileIn(path);
	if (fileIn.is_open())
	{
		std::string line;
		while (std::getline(fileIn, line))
		{
			std::istringstream iss(line);
			std::string assetType;
			if ((iss >> assetType))
			{
				if (assetType == "Font")
				{
					std::string name;
					std::string path;
					iss >> name >> path;
					addFont(name, path);
				}
				if (assetType == "Texture")
				{
					std::string name;
					std::string path;
					iss >> name >> path;
					addTexture(name, path);
				}
				if (assetType == "Animation")
				{
					std::string name;
					std::string textureName;
					int countFrames;
					int speed;
					iss >> name >> textureName >> countFrames >> speed;
					addAnimation(name, textureName, countFrames, speed);
				}
			}
		}
	}
}
