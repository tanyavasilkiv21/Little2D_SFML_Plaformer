#include "Animation.h"
#include <iostream>
Animation::Animation()
{
}

Animation::Animation(const std::string& name, const sf::Texture& t)
	:Animation(name, t, 1, 0)
{
	m_elapsedTime = sf::Time::Zero;
}

Animation::Animation(const std::string& name, const sf::Texture& t, size_t frameCount, size_t speed)
	:m_name(name), m_sprite(t), m_frameCount(frameCount), m_speed(speed)
{
	m_size = Vec2((float)t.getSize().x, (float)t.getSize().y / frameCount);
	m_sprite.setOrigin(m_size.x / 2.f, m_size.y / 2.f);
	m_sprite.setTextureRect(sf::IntRect( 0, std::floor(m_currentFrame) * m_size.y, m_size.x, m_size.y));

}

void Animation::update()
{
	if (m_speed == 0)
	{
		return;
	}
	sf::Time deltaTime = m_clock.restart();
	m_elapsedTime += deltaTime;
	float timeAsSecond = m_elapsedTime.asSeconds();
	m_currentFrame = static_cast<int>((timeAsSecond/m_speed) * static_cast<float>(m_frameCount)) % m_frameCount;
	m_sprite.setTextureRect(sf::IntRect(0, m_currentFrame * m_size.y, m_size.x, m_size.y));
}

bool Animation::hasEnded() const
{
	if (m_frameCount == m_currentFrame)
	{
		return true;
	}
	else if (m_currentFrame < m_frameCount)
	{
		return false;
	}
}

const std::string& Animation::getName() const
{
	return m_name;
}

const Vec2& Animation::getSize() const
{
	return m_size;
}

sf::Sprite& Animation::getSprite()
{
	return m_sprite;
}

const int Animation::getCountFrame() const
{
	return m_frameCount;
}
