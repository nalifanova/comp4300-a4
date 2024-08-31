#include "Entity.hpp"

Entity::Entity(const size_t& id, std::string tag) :
    m_id(id), m_tag(std::move(tag)) {}

size_t Entity::id() const
{
    return m_id;
}

bool Entity::isActive() const
{
    return m_active;
}

const std::string& Entity::tag() const
{
    return m_tag;
}

void Entity::destroy()
{
    m_active = false;
}
