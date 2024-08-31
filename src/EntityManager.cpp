#include "EntityManager.hpp"

EntityManager::EntityManager() = default;

// called at beginning of each frame by game engine
// entities added will now be available to use this frame
void EntityManager::update()
{
    for (const auto& entity: m_entitiesToAdd)
    {
        m_entities.push_back(entity);
        m_entityMap[entity->tag()].push_back(entity);
    }
    m_entitiesToAdd.clear();

    removeDeadEntities(m_entities);

    // remove dead entities from each vector in the entity map
    for (auto& [tag, entityVec]: m_entityMap)
    {
        removeDeadEntities(entityVec);
    }
}

std::shared_ptr<Entity> EntityManager::addEntity(const std::string& tag)
{
    auto entity = std::shared_ptr<Entity>(new Entity(m_totalEntities++, tag));
    m_entitiesToAdd.push_back(entity);

    return entity;
}

EntityVec& EntityManager::getEntities()
{
    return m_entities;
}

EntityVec& EntityManager::getEntities(const std::string& tag)
{
    return m_entityMap[tag];
}

const EntityMap& EntityManager::getEntityMap()
{
    return m_entityMap;
}

// private
void EntityManager::removeDeadEntities(EntityVec& vec)
{
    vec.erase(
        std::remove_if(vec.begin(), vec.end(),
                       [](const std::shared_ptr<Entity>& entity)
                       {
                           return !entity->isActive();
                       }),
        vec.end()
        );
}
