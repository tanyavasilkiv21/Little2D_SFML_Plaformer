#include "Vec2.h"
#include <math.h>

bool Vec2::operator==(const Vec2& someVec) const
{
    return (this->x == someVec.x && this->y == someVec.y);
}

bool Vec2::operator!=(const Vec2& someVec) const
{
    return !(this->x == someVec.x && this->y == someVec.y);
}

Vec2 Vec2::operator+(const Vec2& someVec) const
{
    return Vec2((this->x + someVec.x), (this->y + someVec.y));
}

Vec2 Vec2::operator-(const Vec2& someVec) const
{
    return Vec2((this->x - someVec.x), (this->y - someVec.y));
}

Vec2 Vec2::operator/(const Vec2& someVec) const
{
    return Vec2((this->x / someVec.x), (this->y / someVec.y));
}

Vec2 Vec2::operator*(const Vec2& someVec) const
{
    return Vec2((this->x * someVec.x), (this->y * someVec.y));
}

Vec2 Vec2::operator+=(const Vec2& someVec)
{
    x += someVec.x;
    y += someVec.y;
    return *this;
}

Vec2 Vec2::operator-=(const Vec2& someVec)
{
    x -= someVec.x;
    y -= someVec.y;
    return *this;
}

Vec2 Vec2::operator/=(const Vec2& someVec)
{
    x /= someVec.x;
    y /= someVec.y;
    return *this;
}

Vec2 Vec2::operator*=(const Vec2& someVec)
{
    x *= someVec.x;
    y *= someVec.y;
    return *this;
}

Vec2 Vec2::normalize()
{
    return Vec2(x / length(), y / length());
}

double Vec2::length()
{
    return sqrtf((x*x) + (y*y));
}

Vec2 Vec2::dist(Vec2 someVec)
{
    return Vec2(someVec.x - x, someVec.y - y);
}

double Vec2::distSq()
{
    return ((x*x) + (y*y));
}
