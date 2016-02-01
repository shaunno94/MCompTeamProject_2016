#include "SphereCollisionShape.h"
#include "PhysicsObject.h"
#include "GameObject.h"
#include "Math/nclglMath.h"
#include <ncltech/NCLDebug.h>

SphereCollisionShape::SphereCollisionShape() : CollisionShape(CollisionShapeType::CollisionSphere)
{
	m_Radius = 1.0f;
}

SphereCollisionShape::SphereCollisionShape(float radius) : CollisionShape(CollisionShapeType::CollisionSphere)
{
	m_Radius = radius;
}

SphereCollisionShape::~SphereCollisionShape()
{

}

Mat3Physics SphereCollisionShape::BuildInverseInertia(float invMass) const
{
	float i = 2.5f * invMass / (m_Radius * m_Radius); //SOLID
	//float i = 1.5f * invMass / (m_Radius * m_Radius); //HOLLOW

	Mat3Physics inertia;
	inertia._11 = i;
	inertia._22 = i;
	inertia._33 = i;

	return inertia;
}

void SphereCollisionShape::GetCollisionAxes(const PhysicsObject* currentObject, std::vector<Vec3Physics>* out_axes) const
{
	/* There is infinite edges so handle seperately */
}

void SphereCollisionShape::GetEdges(const PhysicsObject* currentObject, std::vector<CollisionEdge>* out_edges) const
{
	/* There is infinite edges on a sphere so handle seperately */
}

void SphereCollisionShape::GetMinMaxVertexOnAxis(const PhysicsObject* currentObject, const Vec3Physics& axis, Vec3Physics* out_min, Vec3Physics* out_max) const
{
	if (out_min)
		*out_min = currentObject->GetPosition() - axis * m_Radius;

	if (out_max)
		*out_max = currentObject->GetPosition() + axis * m_Radius;
}

void SphereCollisionShape::GetIncidentReferencePolygon(const PhysicsObject* currentObject, const Vec3Physics& axis, std::list<Vec3Physics>* out_face, Vec3Physics* out_normal, std::vector<Plane>* out_adjacent_planes) const
{
	if (out_face)
	{
		out_face->push_back(currentObject->GetPosition() + axis * m_Radius);
	}

	if (out_normal)
	{
		*out_normal = axis;
	}
}

void SphereCollisionShape::DebugDraw(const PhysicsObject* currentObject) const
{
	Vec3Physics pos = currentObject->GetPosition();

	//Draw Filled Circle
	NCLDebug::DrawPoint(pos, m_Radius, Vec4Graphics(1.0f, 1.0f, 1.0f, 0.2f));

	//Draw Perimeter Axes
	Vec3Physics lastX = pos + Vec3Physics(0.0f, 1.0f, 0.0f) * m_Radius;
	Vec3Physics lastY = pos + Vec3Physics(1.0f, 0.0f, 0.0f) * m_Radius;
	Vec3Physics lastZ = pos + Vec3Physics(1.0f, 0.0f, 0.0f) * m_Radius;
	for (int itr = 1; itr < 21; ++itr)
	{
		float angle = itr / 20.0f * 6.2831853f;
		float alpha = cosf(angle) * m_Radius;
		float beta = sinf(angle) * m_Radius;

		Vec3Physics newX = pos + Vec3Physics(0.0f, alpha, beta);
		Vec3Physics newY = pos + Vec3Physics(alpha, 0.0f, beta);
		Vec3Physics newZ = pos + Vec3Physics(alpha, beta, 0.0f);

		NCLDebug::DrawThickLine(lastX, newX, 0.02f, Vec4Graphics(1.0f, 0.3f, 1.0f, 1.0f));
		NCLDebug::DrawThickLine(lastY, newY, 0.02f, Vec4Graphics(1.0f, 0.3f, 1.0f, 1.0f));
		NCLDebug::DrawThickLine(lastZ, newZ, 0.02f, Vec4Graphics(1.0f, 0.3f, 1.0f, 1.0f));

		lastX = newX;
		lastY = newY;
		lastZ = newZ;
	}
}