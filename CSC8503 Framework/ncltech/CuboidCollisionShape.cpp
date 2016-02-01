#include "CuboidCollisionShape.h"
#include "PhysicsObject.h"
#include "GameObject.h"
#include "Math/nclglMath.h"
#include <nclgl/OGLRenderer.h>

Hull CuboidCollisionShape::m_CubeHull = Hull();

CuboidCollisionShape::CuboidCollisionShape() : CollisionShape(CollisionShapeType::CollisionCuboid)
{
	m_CuboidHalfDimensions = Vec3Physics(0.5f, 0.5f, 0.5f);

	if (m_CubeHull.GetNumVertices() == 0)
	{
		ConstructCubeHull();
	}
}

CuboidCollisionShape::CuboidCollisionShape(const Vec3Physics& halfdims) : CollisionShape(CollisionShapeType::CollisionCuboid)
{
	m_CuboidHalfDimensions = halfdims;

	if (m_CubeHull.GetNumVertices() == 0)
	{
		ConstructCubeHull();
	}
}

CuboidCollisionShape::~CuboidCollisionShape()
{

}

Mat3Physics CuboidCollisionShape::BuildInverseInertia(float invMass) const
{
	Vec3Physics dimsSq = m_CuboidHalfDimensions + m_CuboidHalfDimensions;
	dimsSq = dimsSq * dimsSq;

	Mat3Physics inertia;

	inertia._11 = 12.f * invMass / (dimsSq.y + dimsSq.z);
	inertia._22 = 12.f * invMass / (dimsSq.x + dimsSq.z);
	inertia._33 = 12.f * invMass / (dimsSq.x + dimsSq.y);
	
	return inertia;
}

void CuboidCollisionShape::GetCollisionAxes(const PhysicsObject* currentObject, std::vector<Vec3Physics>* out_axes) const
{
	if (out_axes)
	{
		Mat3Physics objOrientation = currentObject->GetOrientation().ToMatrix3();
		out_axes->push_back(objOrientation * Vec3Physics(1.0f, 0.0f, 0.0f)); //X - Axis
		out_axes->push_back(objOrientation * Vec3Physics(0.0f, 1.0f, 0.0f)); //Y - Axis
		out_axes->push_back(objOrientation * Vec3Physics(0.0f, 0.0f, 1.0f)); //Z - Axis
	}
}

void CuboidCollisionShape::GetEdges(const PhysicsObject* currentObject, std::vector<CollisionEdge>* out_edges) const
{
	if (out_edges)
	{
		Mat4Physics transform = currentObject->GetWorldSpaceTransform() * Mat4Physics::Scale(Vec3Physics(m_CuboidHalfDimensions));
		for (unsigned int i = 0; i < m_CubeHull.GetNumEdges(); ++i)
		{
			const HullEdge& edge = m_CubeHull.GetEdge(i);
			Vec3Physics A = transform * m_CubeHull.GetVertex(edge.vStart).pos;
			Vec3Physics B = transform * m_CubeHull.GetVertex(edge.vEnd).pos;

			out_edges->push_back(CollisionEdge(A, B));
		}
	}
}

void CuboidCollisionShape::GetMinMaxVertexOnAxis(const PhysicsObject* currentObject, const Vec3Physics& axis, Vec3Physics* out_min, Vec3Physics* out_max) const
{
	Mat4Physics wsTransform = currentObject->GetWorldSpaceTransform() * Mat4Physics::Scale(m_CuboidHalfDimensions);

	Mat3Physics invNormalMatrix = Mat3Physics::Transpose(Mat3Physics(wsTransform));
	Vec3Physics local_axis = invNormalMatrix * axis;

	int vMin, vMax;
	m_CubeHull.GetMinMaxVerticesInAxis(local_axis, &vMin, &vMax);

	if (out_min) *out_min = wsTransform * m_CubeHull.GetVertex(vMin).pos;
	if (out_max) *out_max = wsTransform * m_CubeHull.GetVertex(vMax).pos;
}

void CuboidCollisionShape::GetIncidentReferencePolygon(const PhysicsObject* currentObject, const Vec3Physics& axis, std::list<Vec3Physics>* out_face, Vec3Physics* out_normal, std::vector<Plane>* out_adjacent_planes) const
{
	Mat4Physics wsTransform = currentObject->GetWorldSpaceTransform() * Mat4Physics::Scale(m_CuboidHalfDimensions);

	Mat3Physics invNormalMatrix = Mat3Physics::Inverse(Mat3Physics(wsTransform));
	Mat3Physics normalMatrix = Mat3Physics::Transpose(invNormalMatrix);
	
	Vec3Physics local_axis = invNormalMatrix * axis;

	int minVertex, maxVertex;
	m_CubeHull.GetMinMaxVerticesInAxis(local_axis, &minVertex, &maxVertex);

	const HullVertex& vert = m_CubeHull.GetVertex(maxVertex);

	const HullFace* best_face = 0;
	float best_correlation = -FLT_MAX;
	for (int faceIdx : vert.enclosing_faces)
	{
		const HullFace* face = &m_CubeHull.GetFace(faceIdx);
		float temp_correlation = local_axis.Dot(face->normal);
		if (temp_correlation > best_correlation)
		{
			best_correlation = temp_correlation;
			best_face = face;
		}
	}

	if (out_normal)
	{
		*out_normal = normalMatrix * best_face->normal;
		(*out_normal).Normalize();
	}

	if (out_face)
	{
		for (int vertIdx : best_face->vert_ids)
		{
			const HullVertex& vert = m_CubeHull.GetVertex(vertIdx);
			out_face->push_back(wsTransform * vert.pos);
		}
	}

	if (out_adjacent_planes)
	{
		//Add the reference face itself to the list of adjacent planes
		Vec3Physics wsPointOnPlane = wsTransform * m_CubeHull.GetVertex(m_CubeHull.GetEdge(best_face->edge_ids[0]).vStart).pos;
		Vec3Physics planeNrml = -(normalMatrix * best_face->normal);
		planeNrml.Normalize();
		float planeDist = -(planeNrml.Dot(wsPointOnPlane));

		out_adjacent_planes->push_back(Plane(planeNrml, planeDist));
		

		for (int edgeIdx : best_face->edge_ids)
		{
			const HullEdge& edge = m_CubeHull.GetEdge(edgeIdx);

			wsPointOnPlane = wsTransform * m_CubeHull.GetVertex(edge.vStart).pos;

			for (int adjFaceIdx : edge.enclosing_faces)
			{
				if (adjFaceIdx != best_face->idx)
				{
					const HullFace& adjFace = m_CubeHull.GetFace(adjFaceIdx);

					planeNrml = -(normalMatrix * adjFace.normal);
					planeNrml.Normalize();
					planeDist = -(planeNrml.Dot(wsPointOnPlane));

					out_adjacent_planes->push_back(Plane(planeNrml, planeDist));
				}
			}	
		}
	}
}


void CuboidCollisionShape::DebugDraw(const PhysicsObject* currentObject) const
{
	Mat4Physics transform = currentObject->GetWorldSpaceTransform() * Mat4Physics::Scale(m_CuboidHalfDimensions);
	m_CubeHull.DebugDraw(transform);
}

void CuboidCollisionShape::ConstructCubeHull()
{
	//Vertices
	m_CubeHull.AddVertex(Vec3Physics(-1.0f, -1.0f, -1.0f));		// 0
	m_CubeHull.AddVertex(Vec3Physics(-1.0f,  1.0f, -1.0f));		// 1
	m_CubeHull.AddVertex(Vec3Physics( 1.0f,  1.0f, -1.0f));		// 2
	m_CubeHull.AddVertex(Vec3Physics( 1.0f, -1.0f, -1.0f));		// 3

	m_CubeHull.AddVertex(Vec3Physics(-1.0f, -1.0f,  1.0f));		// 4
	m_CubeHull.AddVertex(Vec3Physics(-1.0f,  1.0f,  1.0f));		// 5
	m_CubeHull.AddVertex(Vec3Physics( 1.0f,  1.0f,  1.0f));		// 6
	m_CubeHull.AddVertex(Vec3Physics( 1.0f, -1.0f,  1.0f));		// 7


	int face1[] = { 0, 1, 2, 3 };
	int face2[] = { 7, 6, 5, 4 };
	int face3[] = { 5, 6, 2, 1 };
	int face4[] = { 0, 3, 7, 4 };
	int face5[] = { 6, 7, 3, 2 };
	int face6[] = { 4, 5, 1, 0 };

	//Faces
	m_CubeHull.AddFace(Vec3Physics(0.0f, 0.0f, -1.0f), 4, face1);
	m_CubeHull.AddFace(Vec3Physics(0.0f, 0.0f, 1.0f), 4, face2);
	m_CubeHull.AddFace(Vec3Physics(0.0f, 1.0f, 0.0f), 4, face3);
	m_CubeHull.AddFace(Vec3Physics(0.0f, -1.0f, 0.0f), 4, face4);
	m_CubeHull.AddFace(Vec3Physics(1.0f, 0.0f, 0.0f), 4, face5);
	m_CubeHull.AddFace(Vec3Physics(-1.0f, 0.0f, 0.0f), 4, face6);
}