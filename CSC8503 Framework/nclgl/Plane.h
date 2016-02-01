/******************************************************************************
Class:Plane
Implements:
Author:Rich Davison	<richard.davison4@newcastle.ac.uk>
Description:VERY simple Plane class. Students are encouraged to modify 
this as necessary!

-_-_-_-_-_-_-_,------,   
_-_-_-_-_-_-_-|   /\_/\   NYANYANYAN
-_-_-_-_-_-_-~|__( ^ .^) /
_-_-_-_-_-_-_-""  ""   

*//////////////////////////////////////////////////////////////////////////////

#pragma once

#include "Math/nclglMath.h"

class Plane	{
public:
	Plane(void){};
	Plane(const Vec3Graphics &normal, float distance, bool normalise = false);
	~Plane(void){};

	//Sets the planes normal, which should be UNIT LENGTH!!!
	void	SetNormal(const Vec3Graphics &normal) {this->normal = normal;}
	//Gets the planes normal.
	Vec3Graphics GetNormal() const				 {return normal;}
	//Sets the planes distance from the origin
	void	SetDistance(float dist)	{distance = dist;}
	//Gets the planes distance from the origin
	float	GetDistance() const		{return distance;}
	//Performs a simple sphere / plane test
	bool SphereInPlane(const Vec3Graphics &position, float radius) const; 
	//Performs a simple sphere / point test
	bool PointInPlane(const Vec3Graphics &position) const;

protected:
	//Unit-length plane normal
	Vec3Graphics normal;
	//Distance from origin
	float	distance;
};