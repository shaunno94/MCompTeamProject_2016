#pragma once

#include "TSingleton.h"
#include "PhysicsObject.h"
#include "CollisionShape.h"
#include "Manifold.h"

struct CollisionData
{
	float penetration;
	Vec3Physics normal;
	Vec3Physics pointOnPlane;
};

class CollisionDetection : public TSingleton < CollisionDetection >
{
	friend class TSingleton < CollisionDetection > ;
	typedef  bool (CollisionDetection::*CollisionCheckFunc)(const PhysicsObject* obj1, const PhysicsObject* obj2, const CollisionShape* shape1, const CollisionShape* shape2, CollisionData* out_coldata) const;

	CollisionCheckFunc* m_CollisionCheckFunctions;

public:
	CollisionDetection();

	inline bool CheckCollision(const PhysicsObject* obj1, const PhysicsObject* obj2, const CollisionShape* shape1, const CollisionShape* shape2, CollisionData* out_coldata = nullptr) const {
		return CALL_MEMBER_FN(*this, m_CollisionCheckFunctions[shape1->GetType() | shape2->GetType()])(obj1, obj2, shape1, shape2, out_coldata);
	}

	bool BuildCollisionManifold(const PhysicsObject* obj1, const PhysicsObject* obj2, const CollisionShape* shape1, const CollisionShape* shape2, const CollisionData& coldata, Manifold* out_manifold) const;
	

	static inline bool CheckSphereOverlap(const Vec3Physics& pos1, float radius1, const Vec3Physics& pos2, float radius2) {
		return (pos2 - pos1).LengthSq() <= Squared(radius1 + radius2);
	}
	static inline bool CheckAABBOverlap(const Vec3Physics& pos1, const Vec3Physics& halfHidth1, const Vec3Physics& pos2, const Vec3Physics& halfHidth2) {
		if (abs(pos1.x - pos2.x) >= (halfHidth1.x + halfHidth2.x)) return false;
		if (abs(pos1.y - pos2.y) >= (halfHidth1.y + halfHidth2.y)) return false;
		if (abs(pos1.z - pos2.z) >= (halfHidth1.z + halfHidth2.z)) return false;
		return true;
	}
	static inline bool CheckAABBSphereOverlap(const Vec3Physics& center, const Vec3Physics& halfVol, const Vec3Physics& spherePos, float sphereRad) {
		const Vec3Physics minVol = center - halfVol;
		const Vec3Physics maxVol = center + halfVol;
		float distSquared = sphereRad * sphereRad;

		if (spherePos.x <= minVol.x)			distSquared -= Squared(spherePos.x - minVol.x);
		else if (spherePos.x >= maxVol.x)	distSquared -= Squared(spherePos.x - maxVol.x);

		if (spherePos.y <= minVol.y)			distSquared -= Squared(spherePos.y - minVol.y);
		else if (spherePos.y >= maxVol.y)	distSquared -= Squared(spherePos.y - maxVol.y);

		if (spherePos.z <= minVol.z)			distSquared -= Squared(spherePos.z - minVol.z);
		else if (spherePos.z >= maxVol.z)	distSquared -= Squared(spherePos.z - maxVol.z);

		return distSquared > 0;
	}

	static inline bool CheckSphereInsideAABB(const Vec3Physics& spherePos, float sphereRadius, const Vec3Physics& AABBCenter, const Vec3Physics& AABBHalfVol) {
		//min check
		Vec3Physics minPoint = AABBCenter - AABBHalfVol;
		if(minPoint.x > spherePos.x - sphereRadius) return false;
		if(minPoint.y > spherePos.y - sphereRadius) return false;
		if(minPoint.z > spherePos.z - sphereRadius) return false;
		//max check
		Vec3Physics maxPoint = AABBCenter + AABBHalfVol;
		if(maxPoint.x < spherePos.x + sphereRadius) return false;
		if(maxPoint.y < spherePos.y + sphereRadius) return false;
		if(maxPoint.z < spherePos.z + sphereRadius) return false;

		return true;
	}

	static inline bool CheckAABBInsideAABB(const Vec3Physics& AABBInsideCenter, const Vec3Physics& AABBInsideHalfVol, const Vec3Physics& AABBCenter, const Vec3Physics& AABBHalfVol) {
		//min check
		Vec3Physics minPoint = AABBCenter - AABBHalfVol;
		Vec3Physics minInsidePoint = AABBInsideCenter - AABBInsideHalfVol;
		if(minPoint.x > minInsidePoint.x) return false;
		if(minPoint.y > minInsidePoint.y) return false;
		if(minPoint.z > minInsidePoint.z) return false;
		//max check
		Vec3Physics maxPoint = AABBCenter + AABBHalfVol;
		Vec3Physics maxInsidePoint = AABBInsideCenter + AABBInsideHalfVol;
		if(maxPoint.x < maxInsidePoint.x) return false;
		if(maxPoint.y < maxInsidePoint.y) return false;
		if(maxPoint.z < maxInsidePoint.z) return false;

		return true;
	}

protected:
	bool CheckPolyhedronCollision(const PhysicsObject* obj1, const PhysicsObject* obj2, const CollisionShape* shape1, const CollisionShape* shape2, CollisionData* out_coldata = nullptr) const;

	bool CheckPolyhedronSphereCollision(const PhysicsObject* obj1, const PhysicsObject* obj2, const CollisionShape* shape1, const CollisionShape* shape2, CollisionData* out_coldata = nullptr) const;

	bool CheckSphereCollision(const PhysicsObject* obj1, const PhysicsObject* obj2, const CollisionShape* shape1, const CollisionShape* shape2, CollisionData* out_coldata = nullptr) const;

	bool InvalidCheckCollision(const PhysicsObject* obj1, const PhysicsObject* obj2, const CollisionShape* shape1, const CollisionShape* shape2, CollisionData* out_coldata = nullptr) const;


	bool CheckCollisionAxis(const Vec3Physics& axis, const PhysicsObject* obj1, const PhysicsObject* obj2, const CollisionShape* shape1, const CollisionShape* shape2, CollisionData* out_coldata) const;

	Vec3Physics GetClosestPointOnEdges(const Vec3Physics& target, const std::vector<CollisionEdge>& edges) const;
	Vec3Physics PlaneEdgeIntersection(const Plane& plane, const Vec3Physics& start, const Vec3Physics& end) const;
	void	SutherlandHodgesonClipping(const std::list<Vec3Physics>& input_polygon, int num_clip_planes, const Plane* clip_planes, std::list<Vec3Physics>* out_polygon, bool removePoints) const;

};