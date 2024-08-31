#include "Physics.hpp"
#include "Components.hpp"

Vec2 Physics::getOverlap(const std::shared_ptr<Entity>& a, const std::shared_ptr<Entity>& b)
{
    // Student TODO
    return Vec2(0, 0);
}

Vec2 Physics::getPreviousOverlap(const std::shared_ptr<Entity>& a, const std::shared_ptr<Entity>& b)
{
    // Student TODO
    return Vec2(0, 0);
}

bool Physics::isInside(const Vec2& pos, const std::shared_ptr<Entity>& entity)
{
    // is a point inside of entity
    auto& anim = entity->get<CAnimation>().animation;
    auto& s = anim.getSize();
    Vec2 ePos = entity->get<CTransform>().pos;

    if (pos.x > ePos.x - s.x / 2
        && pos.x < ePos.x + s.x / 2
        && pos.y > ePos.y - s.y / 2
        && pos.y < ePos.y + s.y / 2)
    {
        // std::cout << anim.getName() << "\n";
        return true;
    }
    return false;
}

Intersect Physics::lineIntersect(const Vec2& a, const Vec2& b, const Vec2& c, const Vec2& d)
{
    // Student TODO
    return {false, Vec2(0, 0)};
}

bool Physics::entityIntersect(const Vec2& a, const Vec2& b, const std::shared_ptr<Entity>& entity)
{
    // You have to check if line a-b intersects with each life of a BBox of the entity
    // Student TODO
    return false;
}
