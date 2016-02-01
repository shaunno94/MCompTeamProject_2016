/******************************************************************************
Class: Constraint
Implements:
Author: Pieran Marris <p.marris@newcastle.ac.uk>
Description:
A generic template class to represent a linear constraint.

A rigid body has 6 degrees of freedom: 3 positional and 3 rotational. A
constraint in this sense is anything which acts to constrain the movement of that
rigid body.

		(\_/)
		( '_')
	 /""""""""""""\=========     -----D
	/"""""""""""""""""""""""\
....\_@____@____@____@____@_/

*//////////////////////////////////////////////////////////////////////////////
#pragma once
#include "PhysicsObject.h"
#include "Math/nclglMath.h"

class Constraint {
 public:
	Vec3Physics j1, j2, j3, j4; //Jacobian diagonals



	Constraint();

	Constraint(PhysicsObject* objA, PhysicsObject* objB,
	           const Vec3Physics& j1, const Vec3Physics& j2, const Vec3Physics& j3, const Vec3Physics& j4, float b);


	void ApplyImpulse();	//Apply Delta Update


	//Optional overridable functions, incase the constraint values change each frame (e.g distance constraint changes direction after each position update)
	virtual void PreSolverStep(float dt) {}
	virtual void DebugDraw() const {}


	MEM_ALIGN_NEW_DELETE

 public:
	PhysicsObject* objA, *objB;

	float b;				//Bias term

	float delta;			//Constraint Delta

	float impulseSum;		//Accumulated delta over the whole solver iteration
	float impulseSumMin, impulseSumMax; //Min/max values to clamp impulse sum to (default -MAX_INFINITY, +MAX_INFINITY)

	float softness;			//Value that scales the amount of force the constraint applies (related to mass of both objects combined)
};