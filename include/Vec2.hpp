#ifndef VEC2_H
#define VEC2_H

#include <iostream>

class Vec2
{
public:
    Vec2();
    Vec2(float xin, float yin);

    Vec2 operator*(float val) const;
    Vec2 operator/(float val) const;

    Vec2 operator+=(Vec2 rhs);
    Vec2 operator-=(Vec2 rhs);

    Vec2 operator*=(float val);
    Vec2 operator/=(float val);

    bool operator==(const Vec2& rhs) const;
    bool operator!=(const Vec2& rhs) const;

    friend std::ostream&
    operator<<(std::ostream& os, const Vec2& vec)
    {
        return os << "(" << vec.x << ", " << vec.y << ")";
    }

    [[nodiscard]] float dist(const Vec2& rhs) const;

    /**
     * Line segment intersection
     * 2D Cross
     *          a x b = (a.x * b.y) - (a.y * b.x)
     *
     * Properties:
     *          a x a = 0
     *          (a + r) x s = (a x s) + (r x s)
     *          (r x s) = - (s x r)
     */
    [[nodiscard]] float cross2d(const Vec2& rhs) const;
    [[nodiscard]] float length() const;
    [[nodiscard]] float length_squared() const;
    [[nodiscard]] float angle(const Vec2& point) const;

    Vec2 normalize();

    float x = 0;
    float y = 0;

    static float epsilon;
    static const Vec2 kZero;
};

class point2
{
public:
    point2();
    point2(float xin, float yin);

    bool operator==(const point2& rhs) const;
    bool operator!=(const point2& rhs) const;

    [[nodiscard]] float distance(point2 p) const;
    [[nodiscard]] float distance_squared(point2 p) const;

    float x = 0;
    float y = 0;

    static float epsilon;
    static const point2 kOrigin;
};

Vec2 operator+(Vec2 lhs, Vec2 rhs);
point2 operator+(point2 lhs, Vec2 rhs);
Vec2 operator-(Vec2 lhs, Vec2 rhs);
Vec2 operator-(point2 lhs, point2 rhs);
Vec2 operator*(float val, Vec2 lhs);

bool equals(Vec2 lhs, Vec2 rhs);
bool equals(point2 lhs, point2 rhs);

float length(Vec2 a);
float length_squared(Vec2 a);

Vec2 normalize(Vec2 a);

float dot(Vec2 lhs, Vec2 rhs);

float distance(point2 lhs, point2 rhs);
float distance_squared(point2 lhs, point2 rhs);

float angle_radians(Vec2 lhs, Vec2 rhs);
float angle_degrees(Vec2 lhs, Vec2 rhs);

bool clockwise(Vec2 lhs, Vec2 rhs);
bool counterclockwise(Vec2 lhs, Vec2 rhs);

#endif // VEC2_H
