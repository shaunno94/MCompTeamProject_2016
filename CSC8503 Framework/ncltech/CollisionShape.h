/******************************************************************************
Class: CollisionShape
Implements:
Author: Pieran Marris <p.marris@newcastle.ac.uk>
Description: 

A Generic template for all the functionality needed to represent a convex collision shape. 

        (\_/)
        ( '_')
     /""""""""""""\=========     -----D
    /"""""""""""""""""""""""\
....\_@____@____@____@____@_/

*//////////////////////////////////////////////////////////////////////////////

#pragma once

#include "Hull.h"

#include "Math/nclglMath.h"
#include <nclgl/Plane.h>
#include <vector>
#include <list>

class PhysicsObject;

struct CollisionEdge
{
	CollisionEdge(const Vec3Physics& a, const Vec3Physics& b) : posA(a), posB(b) {}

	Vec3Physics posA;
	Vec3Physics posB;
};

enum CollisionShapeType : unsigned int {
	CollisionCuboid = 1,
	CollisionSphere = 2,
	CollisionShapeTypeMax
};

class CollisionShape
{
protected:
	CollisionShapeType m_Type;

public:
	CollisionShape(CollisionShapeType type) : m_Type(type)	{}
	~CollisionShape()	{}

	inline CollisionShapeType GetType() const { return m_Type; }

	/* Constructs an inverse inertia matrix of the given collision volume. This is the equivilant of the inverse mass of an object for rotation,
	   a good source for non-inverse inertia matricies can be found here: https://en.wikipedia.org/wiki/List_of_moments_of_inertia
	*/
	virtual Mat3Physics BuildInverseInertia(float invMass) const = 0;

	/* Returns a list of all possible axes that need to be checked when doing the collision detection. 
	   These are equivilant to a list of all the face normals without duplicates, and parallel vectors.
	*/
	virtual void GetCollisionAxes(const PhysicsObject* currentObject, std::vector<Vec3Physics>* out_axes) const = 0;

	/* Returns a list of all edges AB that form the convex hull of the collision shape. These are
		used to check edge/edge collisions aswell as finding the closest point to a sphere. */
	virtual void GetEdges(const PhysicsObject* currentObject, std::vector<CollisionEdge>* out_edges) const = 0;

	/* Returns the minimum and maximum vertex along the given axis. 
	*/
	virtual void GetMinMaxVertexOnAxis(const PhysicsObject* currentObject, const Vec3Physics& axis, Vec3Physics* out_min, Vec3Physics* out_max) const  = 0;

	/* Computes the face that is closest to parallel to that of the given axis, returning the face (as a list of vertices), face normal and the planes of all adjacent faces for clipping against.
	*/
	virtual void GetIncidentReferencePolygon(const PhysicsObject* currentObject, const Vec3Physics& axis, std::list<Vec3Physics>* out_face, Vec3Physics* out_normal, std::vector<Plane>* out_adjacent_planes) const = 0;

	/* Draws this collision shape to the debug renderer
	*/
	virtual void DebugDraw(const PhysicsObject* currentObject) const = 0;

	MEM_ALIGN_NEW_DELETE
};