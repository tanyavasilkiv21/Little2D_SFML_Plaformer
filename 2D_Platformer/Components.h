#pragma once

#include "Vec2.h"
#include "Animation.h"
#include "SFML/Graphics.hpp"


class Component
{
public:
	bool has = false;
};
class CTransform : public Component
{
public:
	Vec2 pos = { 0.0, 0.0 };
	Vec2 prevPos = { 0.0, 0.0 };
	Vec2 velocity = { 0.0, 0.0 };
	Vec2 scale = { 0.0, 0.0 };
	double angle = 0.0;

	CTransform() {}
	CTransform(Vec2 dataPos) 
	: pos(dataPos) {}
	CTransform(Vec2 dataPos, Vec2 dataVelocity, Vec2 dataScale, double dataAngle)
		: pos(dataPos), velocity(dataVelocity), scale(dataScale), angle(dataAngle) 
	{
		prevPos = pos;
	}
};

class CBoundingBox : public Component
{
public:
	Vec2 size = { 0.f, 0.f };
	Vec2 halfsize;
	CBoundingBox() {}
	CBoundingBox(const Vec2& dataBox)
		: size(dataBox), halfsize(dataBox.x / 2.f, dataBox.y / 2.f) {}
	
};

class CLifespan : public Component
{
public:
	int lifespan = 0;
	int frameCreated = 0;
	CLifespan() {}
	CLifespan(int dataTotal)
		: frameCreated(dataTotal), lifespan(dataTotal) {}
};

class CInput : public Component
{
public:
	bool up = false; 
	bool down = false;
	bool left = false;
	bool right = false;
	bool shoot = false;
	bool canShoot = true;
	bool canJump = true;
	bool jumping = false;
	CInput() {}
};

class CAnimation : public Component
{
public:
	Animation animation;
	bool repeat = false;
	CAnimation() {}
	CAnimation(const Animation & dataAnimation, bool r)
	: animation(dataAnimation), repeat(r) {}
};
class CGravity : public Component
{
public :
	double gravity = 0;
	CGravity() {}
	CGravity(double dataGravity)
		:gravity(dataGravity) {};
};

enum stateType
{
	STAND,
	AIR
};

class CState : public Component
{
public: 
	
	stateType state = STAND;
	CState() {}
	CState(const stateType dataState)
		:state(dataState) {}
};