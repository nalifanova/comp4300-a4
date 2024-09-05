#ifndef COMPONENTS_H
#define COMPONENTS_H

#include "Animation.hpp"
#include "Assets.hpp"

class Component
{
public:
    bool has = false;
};

class CDraggable: public Component
{
public:
    CDraggable() = default;

    bool dragging = false;
};

class CTransform: public Component
{
public:
    CTransform() = default;

    explicit CTransform(const Vec2& p) :
        pos(p) {}

    CTransform(const Vec2& p, const Vec2& speed, const Vec2& s, float a) :
        pos(p), prevPos(p), velocity(speed), scale(s), angle(a) {}

    Vec2 pos = {0.0f, 0.0f};
    Vec2 prevPos = {0.0f, 0.0f};
    Vec2 scale = {1.0f, 1.0f};
    Vec2 velocity = {0.0f, 0.0f};
    Vec2 facing = {0.0f, 1.0f};
    float angle = 0;
};

class CLifespan: public Component
{
public:
    CLifespan() = default;

    explicit CLifespan(int duration, int frame) :
        lifespan(duration), frameCreated(frame) {}

    int lifespan = 0;
    int frameCreated = 0;
};

class CDamage: public Component
{
public:
    CDamage() = default;

    explicit CDamage(int d) :
        damage(d) {}

    int damage = 1;
};

class CInvincibility: public Component
{
public:
    CInvincibility() = default;

    explicit CInvincibility(int f) :
        iframes(f) {}

    int iframes = 1;
};

class CHealth: public Component
{
public:
    CHealth() = default;

    CHealth(int m, int c) :
        max(m), current(c) {}

    int max = 1;
    int current = 1;
};

class CInput: public Component
{
public:
    CInput() = default;

    bool up = false;
    bool down = false;
    bool left = false;
    bool right = false;
    bool attack = false;
};

class CBoundingBox: public Component
{
public:
    CBoundingBox() = default;

    explicit CBoundingBox(const Vec2& s):
        size(s), halfSize(s.x / 2, s.y / 2) {}

    CBoundingBox(const Vec2& s, bool m, bool v) :
        size(s), halfSize(s.x / 2.0f, s.y / 2.0f), blockMove(m), blockVision(v) {}

    Vec2 size;
    Vec2 halfSize;
    bool blockMove = false;
    bool blockVision = false;
};

class CAnimation: public Component
{
public:
    CAnimation() = default;

    CAnimation(Animation a, bool r) :
        animation(std::move(a)), repeat(r) {}

    Animation animation;
    bool repeat = false;
    bool paused = false;
};

class CState: public Component
{
public:
    CState() = default;

    explicit CState(std::string s) :
        state(std::move(s)) {}

    std::string state = "LinkStand";
    bool changed = false;
};

class CFollowPlayer: public Component
{
public:
    CFollowPlayer() = default;

    CFollowPlayer(Vec2 p, float s) :
        home(p), speed(s) {}

    Vec2 home = {0.0f, 0.0f};
    float speed = 0.0f;
};

class CPatrol: public Component
{
public:
    CPatrol() = default;

    CPatrol(std::vector<Vec2>& pos, float s) :
        positions(pos), speed(s) {}

    std::vector<Vec2> positions;
    size_t currentPosition = 0;
    float speed = 0;
};

#endif //COMPONENTS_H
