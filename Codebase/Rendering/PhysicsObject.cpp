#include "PhysicsObject.h"

PhysicsObject::PhysicsObject()
{
	m_ColShape = nullptr;
	m_CollisionObject = nullptr;
}

bool PhysicsObject::CreateCollisionShape(double radius)
{
	m_ColShape = new btSphereShape(radius);
	return true;
}

bool PhysicsObject::CreateCollisionShape(const Vec3Physics& half_extents, CollisionShape shape)
{
	if (shape == CUBOID)
	{
		m_ColShape = new btBoxShape(btVector3(half_extents.x, half_extents.y, half_extents.z));
		return true;
	}
	else if (shape == CYLINDER)
	{
		m_ColShape = new btCylinderShape(btVector3(half_extents.x, half_extents.y, half_extents.z));
		return true;
	}
	return false;
}

bool PhysicsObject::CreateCollisionShape(double radius, double height, CollisionShape shape)
{
	if (shape == CONE)
	{
		m_ColShape = new btConeShape(radius, height);
		return true;
	}
	else if (shape == CAPSULE)
	{
		m_ColShape = new btCapsuleShape(radius, height);
		return true;
	}
	return false;
}

bool PhysicsObject::CreateCollisionShape(double distance, const Vec3Physics& normal, bool normalised)
{
	btVector3 norm = btVector3(normal.x, normal.y, normal.z);
	if (!normalised)
		norm.normalize();

	m_ColShape = new btStaticPlaneShape(btVector3(normal.x, normal.y, normal.z), distance);
	return true;
}

PhysicsObject::~PhysicsObject()
{
	if (m_CollisionObject)
	{
		delete m_ColShape;
		delete m_CollisionObject;
		m_ColShape = nullptr;
		m_CollisionObject = nullptr;
	}
}