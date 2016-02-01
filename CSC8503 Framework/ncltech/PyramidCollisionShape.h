#pragma once
#include "Collisionshape.h"
#include "Hull.h"

class PyramidCollisionShape : public CollisionShape {
 public:
	PyramidCollisionShape(void);
	PyramidCollisionShape(const Vec3Physics& scale);
	~PyramidCollisionShape(void);

	//Collision Shape Functionality
	virtual Mat3Physics BuildInverseInertia(float invMass) const override;

	virtual void GetCollisionAxes(const PhysicsObject* currentObject, std::vector<Vec3Physics>* out_axes) const override;
	virtual void GetEdges(const PhysicsObject* currentObject, std::vector<CollisionEdge>* out_edges) const override;

	virtual void GetMinMaxVertexOnAxis(const PhysicsObject* currentObject, const Vec3Physics& axis, Vec3Physics* out_min, Vec3Physics* out_max) const override;
	virtual void GetIncidentReferencePolygon(const PhysicsObject* currentObject, const Vec3Physics& axis, std::list<Vec3Physics>* out_face, Vec3Physics* out_normal, std::vector<Plane>* out_adjacent_planes) const override;

	virtual void DebugDraw(const PhysicsObject* currentObject) const override;


 protected:
	//Constructs the static cube hull
	static void ConstructPyramidHull();
	Vec3Physics				m_Scale;
	Vec3Physics				m_Normals[5];

	static Hull			 m_PyramidHull;
};

