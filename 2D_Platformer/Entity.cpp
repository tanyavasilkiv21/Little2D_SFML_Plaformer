#include "Entity.h"

bool Entity::isAlive() const
{
	return m_active;
}

const std::string& Entity::tag() const
{
	return m_tag;
}

const size_t Entity::id() const
{
	return m_id;
}

void Entity::destroy()
{
	m_active = false;
}
