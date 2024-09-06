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
    Vec2 operator+(float val) const;
    Vec2 operator-(float val) const;

    Vec2 operator*(const Vec2& rhs) const;
    Vec2 operator+(const Vec2& rhs) const;
    Vec2 operator-(const Vec2& rhs) const;

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

    /**
     * Difference vector D:
     *          D = A - B or D = (a.x - b.x, a.y - b.y)
     * Distance squared:
     *          D.x^2 + D.y^2
     */
    [[nodiscard]] float distSqr(const Vec2& rhs) const;

    /**
     * Distance:  _______________
     *          \/ D.x^2 + D.y^2
     */
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

    /**
     * Vector length:
     *                ___________
     *          R = \/ x^2 + y^2
     */
    [[nodiscard]] float length() const;

    /**
     * Squared vector length:
     *
     *          R^2 = x^2 + y^2
     */
    [[nodiscard]] float lengthSquared() const;

    /**
     * Difference vector:
     *          d = (x1 - x2, y1 - y2)
     * Angle from A to B
     *          θ = atan(d.y, d.x)
     */
    [[nodiscard]] float angle(const Vec2& rhs) const;

    Vec2 normalize();

    /**
     * Towards difference vector (x - y)
     * Calculate:
     * Vector length   __________
     *          L = \/ x^2 + y^2
     * Nomralized vector:
     *          N = (x / L, y / L)
     * Velocity can be calculated as
     *          Speed * N.x, Speed * N.y
     *
     * Note: here param length is speed
     */
    Vec2& setMagnitude(float length);

    /**
     *  Velocity can be calculated as
     *          Speed * cos(θ), Speed * sin(θ)
     *
     *  Note: here length is speed
     */
    static const Vec2& setMagnitudeA(float angle, float length);

    float x = 0;
    float y = 0;
};

#endif // VEC2_H
