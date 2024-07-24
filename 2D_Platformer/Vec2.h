#pragma once

class Vec2
{
public:
	double x = 0;
	double y = 0;
	Vec2() {}
	Vec2(double dataX, double dataY) : x(dataX), y(dataY) {}

	bool operator == (const Vec2 & someVec) const;
	bool operator != (const Vec2& someVec) const;

	Vec2 operator + (const Vec2& someVec) const;
	Vec2 operator - (const Vec2& someVec) const;
	Vec2 operator / (const Vec2& someVec) const;
	Vec2 operator * (const Vec2& someVec) const;

	Vec2 operator += (const Vec2& someVec);
	Vec2 operator -= (const Vec2& someVec);
	Vec2 operator /= (const Vec2& someVec);
	Vec2 operator *= (const Vec2& someVec);

	Vec2 normalize();
	double length();
	Vec2 dist(Vec2 someVec);
	double distSq();
};

