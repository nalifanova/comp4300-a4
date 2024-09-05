#include "Physics.hpp"

#include <random>

#include "Components.hpp"

Vec2 Physics::overlap(const Vec2& aPos, const Vec2& bPos, const Vec2& aSize, const Vec2& bSize)
{
    const Vec2 d = {abs(aPos.x - bPos.x), abs(aPos.y - bPos.y)};
    return {aSize.x + bSize.x - d.x, aSize.y + bSize.y - d.y};
}


Vec2 Physics::getOverlap(const std::shared_ptr<Entity>& a, const std::shared_ptr<Entity>& b)
{
    const auto& aPos = a->get<CTransform>().pos;
    const auto& bPos = b->get<CTransform>().pos;
    const auto& aSize = a->get<CBoundingBox>().halfSize;
    const auto& bSize = b->get<CBoundingBox>().halfSize;
    return overlap(aPos, bPos, aSize, bSize);
}

Vec2 Physics::getPreviousOverlap(const std::shared_ptr<Entity>& a, const std::shared_ptr<Entity>& b)
{
    const auto& aPos = a->get<CTransform>().prevPos;
    const auto& bPos = b->get<CTransform>().prevPos;
    const auto& aSize = a->get<CBoundingBox>().halfSize;
    const auto& bSize = b->get<CBoundingBox>().halfSize;
    return overlap(aPos, bPos, aSize, bSize);
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

sf::Vector2f Physics::getRandomOffset(float size)
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dist(0, 3);

    if (size < 64.0f) { size = 64.0f; } // workaround
    switch (dist(gen)) {
        case 0: return { size, 0 };   // right
        case 1: return { -size, 0 };  // left
        case 2: return { 0, size };   // down
        case 3: return { 0, -size };  // up
        default:
            return {0, 0};
    }
}

Vec2 Physics::getRandomCoordinates(const std::vector<Vec2>& coords)
{
    if (coords.size() == 0) { return Vec2{0.0, 0.0}; }
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dist(0, static_cast<int>(coords.size()) - 1);

    return coords[dist(gen)];
}