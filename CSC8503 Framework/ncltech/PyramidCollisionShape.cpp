#include "PyramidCollisionShape.h"
#include "PhysicsObject.h"
#include "GameObject.h"
#include "Math/nclglMath.h"
#include <nclgl/OGLRenderer.h>


Hull PyramidCollisionShape::m_PyramidHull;

PyramidCollisionShape::PyramidCollisionShape() : CollisionShape(CollisionShapeType::CollisionCuboid) {
	m_Scale = Vec3Physics::ONES;

	if (m_PyramidHull.GetNumVertices() == 0)
		ConstructPyramidHull();
}

PyramidCollisionShape::PyramidCollisionShape(const Vec3Physics& scale) : CollisionShape(CollisionShapeType::CollisionCuboid) {
	m_Scale = scale;
	Mat4Physics transform = Mat4Physics::Scale(scale);
	Vec3Physics m_Points[5] = {
		transform * Vec3Physics(-1.0f, -1.0f, -1.0f),
		transform * Vec3Physics(-1.0f, -1.0f, 1.0f),
		transform * Vec3Physics(1.0f, -1.0f, 1.0f),
		transform * Vec3Physics(1.0f, -1.0f, -1.0f),
		transform * Vec3Physics(0.0f, 1.0f, 0.0f)
	};

	m_Normals[0] = (m_Points[0] - m_Points[3]).Cross(m_Points[4] - m_Points[3]).Normal();
	m_Normals[1] = (m_Points[1] - m_Points[0]).Cross(m_Points[4] - m_Points[0]).Normal();
	m_Normals[2] = (m_Points[2] - m_Points[1]).Cross(m_Points[4] - m_Points[1]).Normal();
	m_Normals[3] = (m_Points[3] - m_Points[2]).Cross(m_Points[4] - m_Points[2]).Normal();
	m_Normals[4] = Vec3Physics(0.0f, -1.0f, 0.0f);

	if (m_PyramidHull.GetNumVertices() == 0)
		ConstructPyramidHull();
}

PyramidCollisionShape::~PyramidCollisionShape() {

}

Mat3Physics PyramidCollisionShape::BuildInverseInertia(float invMass) const {

	Vec3Physics scaleSq = m_Scale * m_Scale;

	Mat3Physics inertia;

	inertia._11 = invMass / ((0.2f * scaleSq.x) + (0.15f * scaleSq.y));
	inertia._22 = invMass / ((0.2f * scaleSq.z) + (0.15f * scaleSq.y));
	inertia._33 = invMass / ((0.2f * scaleSq.z) + (0.2f * scaleSq.x));

	return inertia;
}

void PyramidCollisionShape::GetCollisionAxes(const PhysicsObject* currentObject, std::vector<Vec3Physics>* out_axes) const {
	if (out_axes) {
		Mat3Physics objOrientation = currentObject->GetOrientation().ToMatrix3();
		out_axes->push_back(objOrientation * m_Normals[0]);
		out_axes->push_back(objOrientation * m_Normals[1]);
		out_axes->push_back(objOrientation * m_Normals[2]);
		out_axes->push_back(objOrientation * m_Normals[3]);
		out_axes->push_back(objOrientation * m_Normals[4]);
	}
}

void PyramidCollisionShape::GetEdges(const PhysicsObject* currentObject, std::vector<CollisionEdge>* out_edges) const {
	if (out_edges) {
		Mat4Physics transform = currentObject->GetWorldSpaceTransform() * Mat4Physics::Scale(m_Scale);
		for (unsigned int i = 0; i < m_PyramidHull.GetNumEdges(); ++i) {
			const HullEdge& edge = m_PyramidHull.GetEdge(i);
			Vec3Physics A = transform * m_PyramidHull.GetVertex(edge.vStart).pos;
			Vec3Physics B = transform * m_PyramidHull.GetVertex(edge.vEnd).pos;

			out_edges->push_back(CollisionEdge(A, B));
		}
	}
}

void PyramidCollisionShape::GetMinMaxVertexOnAxis(const PhysicsObject* currentObject, const Vec3Physics& axis, Vec3Physics* out_min, Vec3Physics* out_max) const {
	Mat4Physics wsTransform = currentObject->GetWorldSpaceTransform() * Mat4Physics::Scale(m_Scale);

	Mat3Physics invNormalMatrix = Mat3Physics::Transpose(Mat3Physics(wsTransform));
	Vec3Physics local_axis = invNormalMatrix * axis;

	int vMin, vMax;
	m_PyramidHull.GetMinMaxVerticesInAxis(local_axis, &vMin, &vMax);

	if (out_min) *out_min = wsTransform * m_PyramidHull.GetVertex(vMin).pos;
	if (out_max) *out_max = wsTransform * m_PyramidHull.GetVertex(vMax).pos;
}

void PyramidCollisionShape::GetIncidentReferencePolygon(const PhysicsObject* currentObject, const Vec3Physics& axis, std::list<Vec3Physics>* out_face, Vec3Physics* out_normal, std::vector<Plane>* out_adjacent_planes) const {
	Mat4Physics wsTransform = currentObject->GetWorldSpaceTransform() * Mat4Physics::Scale(m_Scale);

	Mat3Physics invNormalMatrix = Mat3Physics::Inverse(Mat3Physics(wsTransform));
	Mat3Physics normalMatrix = Mat3Physics::Transpose(invNormalMatrix);

	Vec3Physics local_axis = invNormalMatrix * axis;

	int minVertex, maxVertex;
	m_PyramidHull.GetMinMaxVerticesInAxis(local_axis, &minVertex, &maxVertex);

	const HullVertex& vert = m_PyramidHull.GetVertex(maxVertex);

	const HullFace* best_face = 0;
	float best_correlation = -FLT_MAX;
	for (int faceIdx : vert.enclosing_faces) {
		const HullFace* face = &m_PyramidHull.GetFace(faceIdx);
		float temp_correlation = local_axis.Dot(face->normal);
		if (temp_correlation > best_correlation) {
			best_correlation = temp_correlation;
			best_face = face;
		}
	}

	if (out_normal) {
		*out_normal = normalMatrix * best_face->normal;
		(*out_normal).Normalize();
	}

	if (out_face) {
		for (int vertIdx : best_face->vert_ids) {
			const HullVertex& vert = m_PyramidHull.GetVertex(vertIdx);
			out_face->push_back(wsTransform * vert.pos);
		}
	}

	if (out_adjacent_planes) {
		//Add the reference face itself to the list of adjacent planes
		Vec3Physics wsPointOnPlane = wsTransform * m_PyramidHull.GetVertex(m_PyramidHull.GetEdge(best_face->edge_ids[0]).vStart).pos;
		Vec3Physics planeNrml = -(normalMatrix * best_face->normal);
		planeNrml.Normalize();
		float planeDist = -(planeNrml.Dot(wsPointOnPlane));

		out_adjacent_planes->push_back(Plane(planeNrml, planeDist));


		for (int edgeIdx : best_face->edge_ids) {
			const HullEdge& edge = m_PyramidHull.GetEdge(edgeIdx);

			wsPointOnPlane = wsTransform * m_PyramidHull.GetVertex(edge.vStart).pos;

			for (int adjFaceIdx : edge.enclosing_faces) {
				if (adjFaceIdx != best_face->idx) {
					const HullFace& adjFace = m_PyramidHull.GetFace(adjFaceIdx);

					planeNrml = -(normalMatrix * adjFace.normal);
					planeNrml.Normalize();
					planeDist = -(planeNrml.Dot(wsPointOnPlane));

					out_adjacent_planes->push_back(Plane(planeNrml, planeDist));
				}
			}
		}
	}
}


void PyramidCollisionShape::DebugDraw(const PhysicsObject* currentObject) const {
	Mat4Physics transform = currentObject->GetWorldSpaceTransform() * Mat4Physics::Scale(m_Scale);
	m_PyramidHull.DebugDraw(transform);
}

void PyramidCollisionShape::ConstructPyramidHull() {
	Vec3Physics v0 = Vec3Physics(-1.0f, -1.0f, -1.0f);		// 0
	Vec3Physics v1 = Vec3Physics(-1.0f, -1.0f,  1.0f);		// 1
	Vec3Physics v2 = Vec3Physics( 1.0f, -1.0f,  1.0f);		// 2
	Vec3Physics v3 = Vec3Physics( 1.0f, -1.0f, -1.0f);		// 3
	Vec3Physics v4 = Vec3Physics( 0.0f,  1.0f,  0.0f);		// 4
	//Vertices
	m_PyramidHull.AddVertex(v0);		// 0
	m_PyramidHull.AddVertex(v1);		// 1
	m_PyramidHull.AddVertex(v2);		// 2
	m_PyramidHull.AddVertex(v3);		// 3
	m_PyramidHull.AddVertex(v4);		// 4

	int face1[] = { 0, 4, 3 };
	int face2[] = { 1, 4, 0 };
	int face3[] = { 2, 4, 1 };
	int face4[] = { 3, 4, 2 };
	int face5[] = { 0, 3, 2, 1 };

	m_PyramidHull.AddFace((v0 - v3).Cross(v4 - v3).Normal(), 3, face1);
	m_PyramidHull.AddFace((v1 - v0).Cross(v4 - v0).Normal(), 3, face2);
	m_PyramidHull.AddFace((v2 - v1).Cross(v4 - v1).Normal(), 3, face3);
	m_PyramidHull.AddFace((v3 - v2).Cross(v4 - v2).Normal(), 3, face4);
	m_PyramidHull.AddFace(Vec3Physics(0.0f, -1.0f, 0.0f), 4, face5);
}