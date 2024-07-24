#include "EntityManager.h"
#include <iostream>
void EntityManager::removeDeadEntities(EntityVec& vec)
{
	auto it = std::remove_if(m_entities.begin(), m_entities.end(), [](const std::shared_ptr<Entity>& e)
		{
			return !e->isAlive();
		});
	m_entities.erase(it, m_entities.end());

	for (auto& [key, vec] : m_entityMap)
	{
		auto it = std::remove_if(vec.begin(), vec.end(), [](const std::shared_ptr<Entity>& e) 
			{
				return !e->isAlive();
			});
		vec.erase(it, vec.end());
	}
	
}
	

EntityManager::EntityManager()
{
	
}

void EntityManager::update()
{

	for (std::shared_ptr<Entity> e : m_entitiesToAdd)
	{
		m_entities.push_back(e);
		m_entityMap[e->tag()].push_back(e);
	}
	m_entitiesToAdd.clear();
	removeDeadEntities(m_entities);
}

std::shared_ptr<Entity> EntityManager::addEntity(const std::string& tag)
{
	auto entity = std::shared_ptr<Entity>(new Entity(m_totalEntities++, tag));
	m_entitiesToAdd.push_back(entity);
	return entity;
}

const EntityVec& EntityManager::getEntities()
{
	return m_entities;
}

const EntityVec& EntityManager::getEntities(const std::string& tag) 
{
	auto find = m_entityMap.find(tag);
	if (find != m_entityMap.end()) {
		return find->second;
	}
	else
	{
		EntityVec EmptyVec;
		return EmptyVec;
	}
}

const EntityMap& EntityManager::getEntityMap()
{
	return m_entityMap;
}
