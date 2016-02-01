/******************************************************************************
Class: SphereCollisionShape
Implements: CollisionShape
Author: Pieran Marris <p.marris@newcastle.ac.uk>
Description:
Extends CollisionShape to represent a sphere.

A sphere object is one of the easiest 3D shapes to model, as for collision purposes it can only ever have one axis of itersection,
it also can only ever have one contact point. This makes alot of the returns quite trival compared to the cuboid that requires alot of
setting up and adjanency information.



       (\_/)
       ( '_')
     /""""""""""""\=========     -----D
    /"""""""""""""""""""""""\
....\_@____@____@____@____@_/

*//////////////////////////////////////////////////////////////////////////////
#pragma once

#include "CollisionShape.h"

class SphereCollisionShape : public CollisionShape
{
public:
	SphereCollisionShape();
	SphereCollisionShape(float radius);
	~SphereCollisionShape();

	//Collision Shape Functionality
	virtual Mat3Physics BuildInverseInertia(float invMass) const override;

	virtual void GetCollisionAxes(const PhysicsObject* currentObject, std::vector<Vec3Physics, STLManagedAllocator<int, 16>>* out_axes) const override;
	virtual void GetEdges(const PhysicsObject* currentObject, std::vector<CollisionEdge>* out_edges) const override;

	virtual void GetMinMaxVertexOnAxis(const PhysicsObject* currentObject, const Vec3Physics& axis, Vec3Physics* out_min, Vec3Physics* out_max) const override;
	virtual void GetIncidentReferencePolygon(const PhysicsObject* currentObject, const Vec3Physics& axis, std::list<Vec3Physics>* out_face, Vec3Physics* out_normal, std::vector<Plane>* out_adjacent_planes) const override;

	virtual void DebugDraw(const PhysicsObject* currentObject) const override;

	//Get/Set Sphere Radius
	void SetRadius(float radius) 		{ m_Radius = radius; }
	float GetRadius() const 			{ return m_Radius; }

protected:
	float				 m_Radius;
};

