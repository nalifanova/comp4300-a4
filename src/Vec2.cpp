#include <cmath>
#include "Vec2.hpp"

Vec2::Vec2() = default;

Vec2::Vec2(float xin, float yin) :
    x(xin), y(yin) {}

Vec2 Vec2::operator*(const float val) const
{
    return {x * val, y * val};
}

Vec2 Vec2::operator/(const float val) const
{
    return {x / val, y / val};
}

Vec2 Vec2::operator+(const float val) const
{
    return {x + val, y + val};
}

Vec2 Vec2::operator-(const float val) const
{
    return {x - val, y - val};
}

Vec2 Vec2::operator*(const Vec2& rhs) const
{
    return {x * rhs.x, y * rhs.y};
}

Vec2 Vec2::operator+(const Vec2& rhs) const
{
    return {x + rhs.x, y + rhs.y};
}

Vec2 Vec2::operator-(const Vec2& rhs) const
{
    return {x - rhs.x, y - rhs.y};
}

Vec2 Vec2::operator+=(Vec2 rhs)
{
    x += rhs.x;
    y += rhs.y;
    return *this;
}

Vec2 Vec2::operator-=(Vec2 rhs)
{
    x -= rhs.x;
    y -= rhs.y;
    return *this;
}

Vec2 Vec2::operator*=(const float val)
{
    x *= val;
    y *= val;
    return *this;
}

Vec2 Vec2::operator/=(const float val)
{
    x /= val;
    y /= val;
    return *this;
}

bool Vec2::operator==(const Vec2& rhs) const
{
    return x == rhs.x && y == rhs.y;
}

bool Vec2::operator!=(const Vec2& rhs) const
{
    return x != rhs.x || y != rhs.y;
}

float Vec2::dist(const Vec2& rhs) const
{
    const float d_x = abs(x - rhs.x);
    const float d_y = abs(y - rhs.y);
    const auto distance = sqrtf(d_x * d_x + d_y * d_y);
    return distance;
}

float Vec2::cross2d(const Vec2& rhs) const
{
    return x * rhs.y - y * rhs.x;
}

float Vec2::length() const
{
    return std::sqrt(x * x + y * y);
}

float Vec2::length_squared() const
{
    return x * x + y * y;
}

float Vec2::angle(const Vec2& point) const
{
    return atan2(y - point.y, x - point.x);
}

Vec2 Vec2::normalize()
{
    float l = length();
    x /= l;
    y /= l;
    return *this;
}
