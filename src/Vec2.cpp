#include <cmath>
#include "Vec2.hpp"

float Vec2::epsilon = 0.001f;

const Vec2 Vec2::kZero{0.0f, 0.0f};

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

float Vec2::length() const
{
    return std::sqrt(x * x + y * y);
}

float Vec2::length_squared() const
{
    return x * x + y * y;
}

Vec2 Vec2::normalize()
{
    float l = length();
    x /= l;
    y /= l;
    return *this;
}

// Point class
float point2::epsilon = 0.001f;

const point2 point2::kOrigin{0.0f, 0.0f};

point2::point2() = default;

point2::point2(float xin, float yin) :
    x(xin), y(yin) {}

bool point2::operator==(const point2& rhs) const
{
    return x == rhs.x && y == rhs.y;
}

bool point2::operator!=(const point2& rhs) const
{
    return x != rhs.x || y != rhs.y;
}

float point2::distance(point2 p) const
{
    // float dx = rhs.x - x;
    // float dy = rhs.y - y;
    // return std::sqrt(dx * dx + dy * dy);
    return (p - *this).length();
}

float point2::distance_squared(point2 p) const
{
    return (p - *this).length_squared();
}

Vec2 operator+(Vec2 lhs, Vec2 rhs)
{
    return {lhs.x + rhs.x, lhs.y + rhs.y};
}

point2 operator+(point2 lhs, Vec2 rhs)
{
    return {lhs.x + rhs.x, lhs.y + rhs.y};
}

Vec2 operator-(Vec2 lhs, Vec2 rhs)
{
    return {lhs.x - rhs.x, lhs.y - rhs.y};
}

Vec2 operator-(point2 lhs, point2 rhs)
{
    return {lhs.x - rhs.x, lhs.y - rhs.y};
}

Vec2 operator*(float val, Vec2 lhs)
{
    return {lhs.x * val, lhs.y * val};
}

bool equals(Vec2 lhs, Vec2 rhs)
{
    float dx = std::abs(lhs.x - rhs.x);
    float dy = std::abs(lhs.y - rhs.y);
    return dx <= Vec2::epsilon && dy <= Vec2::epsilon;
}

bool equals(point2 lhs, point2 rhs)
{
    float dx = std::abs(lhs.x - rhs.x);
    float dy = std::abs(lhs.y - rhs.y);
    return dx <= point2::epsilon && dy <= point2::epsilon;
}

float length(Vec2 a)
{
    return a.length();
}

float length_squared(Vec2 a)
{
    return a.length_squared();
}

Vec2 normalize(Vec2 a)
{
    a.normalize();
    return a;
}

float dot(Vec2 lhs, Vec2 rhs)
{
    return lhs.x * rhs.x + lhs.y * rhs.y;
}

float distance(point2 lhs, point2 rhs)
{
    return (lhs - rhs).length();
}

float distance_squared(point2 lhs, point2 rhs)
{
    return (lhs - rhs).length_squared();
}

float angle_radians(Vec2 lhs, Vec2 rhs)
{
    if (lhs == rhs)
        return 0.0;

    lhs.normalize();
    rhs.normalize();

    float d = dot(lhs, rhs);
    if (std::abs(d - 1.0f) < Vec2::epsilon)
    {
        d = 1.0f;
    }

    return std::acos(d);
}

float angle_degrees(Vec2 lhs, Vec2 rhs)
{
    float a = angle_radians(lhs, rhs);
    return a * 180.0f / 3.14159265359f;
}

bool clockwise(Vec2 lhs, Vec2 rhs)
{
    float cross = lhs.x * rhs.y - lhs.y * rhs.x;
    return cross < 0;
}

bool counterclockwise(Vec2 lhs, Vec2 rhs)
{
    float cross = lhs.x * rhs.y - lhs.y * rhs.x;
    return cross >= 0;
}
