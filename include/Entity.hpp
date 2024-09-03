#ifndef ENTITY_H
#define ENTITY_H

#include <string>
#include <tuple>

#include "Components.hpp"

class EntityManager;

typedef std::tuple<
    CAnimation,
    CBoundingBox,
    CDamage,
    CDraggable,
    CFollowPlayer,
    CTransform,
    CHealth,
    CInput,
    CInvincibility,
    CLifespan,
    CPatrol,
    CState
> ComponentTuple;

class Entity
{
public:
    [[nodiscard]] size_t id() const;
    [[nodiscard]] bool isActive() const;
    [[nodiscard]] const std::string& tag() const;

    void destroy();

    template <class T>
    [[nodiscard]] bool has() const
    {
        return get<T>().has;
    }

    template <class T, typename... TArgs>
    T& add(TArgs&&... mArgs)
    {
        auto& component = get<T>();
        component = T(std::forward<TArgs>(mArgs)...);
        component.has = true;
        return component;
    }

    template <class T>
    T& get()
    {
        return std::get<T>(m_components);
    }

    template <class T>
    const T& get() const
    {
        return std::get<T>(m_components);
    }

    template <class T>
    void remove()
    {
        get<T>() = T();
    }

private:
    // constructor is private, so we can never create entities
    // outside the EntityManager which had friend access
    Entity(const size_t& id, std::string tag);

    bool m_active = true;
    const size_t m_id = 0;
    const std::string m_tag = "default";
    ComponentTuple m_components{};

    friend class EntityManager;
};

#endif //ENTITY_H
