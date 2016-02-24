#pragma once
#include <Math\nclglMath.h>
#include "PhysicsEngine\PhysicsEngineInstance.h"
#include <vector>

enum CollisionShape
{
	SPHERE, CUBOID, CYLINDER, CONE, CAPSULE
};

enum TYPE
{
	RIGID, SOFT
};

class PhysicsObject
{
public:
	PhysicsObject();
	virtual ~PhysicsObject();

	//Initialise physics body: mass (a mass of 0 will result in a static object), position, orientation, inertia, type (Rigid, Particle, Soft) 
	virtual bool CreatePhysicsBody(double mass, const Vec3Physics& position, const QuatPhysics& orientation, const Vec3Physics& inertia = Vec3Physics(0, 0, 0), bool particle = false) = 0;
	virtual btCollisionObject* GetPhysicsBody() = 0;

	//Sphere collision shape.
	bool CreateCollisionShape(double radius);
	//Cuboid or Cylinder collsion shapes.
	bool CreateCollisionShape(const Vec3Physics& half_extents, CollisionShape shape);
	//Cone or Capsule collision shapes.
	bool CreateCollisionShape(double radius, double height, CollisionShape shape);
	//Static Plane collision shape.
	bool CreateCollisionShape(double distance, const Vec3Physics& normal, bool normalised);

	//Convex Hull collision shape.
	bool CreateCollisionShape(const btVector3* points, int numPoints);
	//Compound collision shape.
	bool CreateCollisionShape(std::vector<btConvexHullShape*> shapes);

	TYPE GetType() const { return m_BodyType; }
protected:
	btCollisionObject*			m_CollisionObject;
	btCollisionShape*			m_ColShape;
	TYPE						m_BodyType;
};