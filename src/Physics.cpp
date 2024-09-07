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
    const Vec2 r = b - a;
    const Vec2 s = d - c;
    const float rxs = r.cross2d(s); // 2d cross product
    const Vec2 cma = c - a;
    const float t = cma.cross2d(s) / rxs; // scalar value for s
    const float u = cma.cross2d(r) / rxs; // scalar value for r

    if (t >= 0 && t <= 1 && u >= 0 && u <= 1)
        return {true, a + r * t};

    return {false, Vec2(0.0f, 0.0f)};
}

bool Physics::entityIntersect(const Vec2& a, const Vec2& b, const std::shared_ptr<Entity>& entity)
{
    // Position is a center point of an entity thus we should find position of each vertex
    // Then we have to check if line a-b intersects with each life of a BBox of the entity
    const auto& halfSize = entity->get<CBoundingBox>().halfSize;
    const auto& pos = entity->get<CTransform>().pos;

    Vec2 topLeft(pos.x - halfSize.x, pos.y - halfSize.y);
    Vec2 topRight(pos.x + halfSize.x, pos.y - halfSize.y);
    Vec2 bottomLeft(pos.x - halfSize.x, pos.y + halfSize.y);
    Vec2 bottomRight(pos.x + halfSize.x, pos.y + halfSize.y);

    bool intersects = lineIntersect(a, b, topLeft, topRight).intersect;
    intersects = intersects || lineIntersect(a, b, topRight, bottomRight).intersect;
    intersects = intersects || lineIntersect(a, b, bottomRight, bottomLeft).intersect;
    intersects = intersects || lineIntersect(a, b, bottomLeft, topLeft).intersect;

    return intersects;
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