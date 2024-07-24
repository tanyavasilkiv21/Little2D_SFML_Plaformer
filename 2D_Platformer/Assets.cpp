#include "Assets.h"

void Assets::addTexture(std::string name, std::string path)
{
	sf::Texture texture;
	texture.loadFromFile(path);
	m_textures[name] = texture;
}

void Assets::addAnimation(std::string name, std::string t, std::string path, int countFrames)
{
	Animation animation(name, getTexture(t), countFrames, 0);
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
	addFont("georgia", "fonts/georgia.ttf");

	addTexture("player", "assets/Meow-Knight_Idle_1.png");
	addAnimation("idle", "player", "assets/Meow-Knight_Idle_1.png", 1);
	addTexture("player_jump", "assets/Meow-Knight_Jump.png");
	addAnimation("jump", "player", "assets/Meow-Knight_Jump.png", 1);

	addTexture("tile_one", "assets/tile_one.png");
	addAnimation("tile_one", "tile_one", "assets/tile_one.png", 1);

	addTexture("tree_one", "assets/tree_one.png");
	addAnimation("tree_one", "tree_one", "assets/tree_one.png", 1);
}
