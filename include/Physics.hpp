#ifndef PHYSICS_H
#define PHYSICS_H

#include "Entity.hpp"
#include "Vec2.hpp"

struct Intersect
{
    bool intersect = false;
    Vec2 position = {0.0f, 0.0f};
};

class Physics
{
public:
    static Vec2 getOverlap(const std::shared_ptr<Entity>& a, const std::shared_ptr<Entity>& b);
    static Vec2 getPreviousOverlap(const std::shared_ptr<Entity>& a, const std::shared_ptr<Entity>& b);

    static bool isInside(const Vec2& pos, const std::shared_ptr<Entity>& entity);
    static Intersect lineIntersect(const Vec2& a, const Vec2& b, const Vec2& c, const Vec2& d);
    static bool entityIntersect(const Vec2& a, const Vec2& b, const std::shared_ptr<Entity>& entity);
};

#endif //PHYSICS_H
