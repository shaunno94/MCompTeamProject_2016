#pragma once
#include "GameObject.h"

/// @ingroup Rendering
/// <summary>
/// 
/// </summary>
class Frustum	
{
public:
	Frustum(void){};
	~Frustum(void){};

	//Creates the frustum planes from a 'view projection' matrix
	void FromMatrix(const Mat4Graphics &viewProj);

	//Is a SceneNode inside this frustum?
	bool InsideFrustum(GameObject* n);

private:
	struct Plane
	{
		Plane(float dist, Vec3Graphics n)
		{
			distance = dist;
			normal = n;
		}
		float distance;
		Vec3Graphics normal;

		bool Plane::PointInPlane(const Vec3Physics& position) const 
		{
			return !(position.Dot(normal) + distance < -0.001f);
		}
	};
	vector<Plane> planes;
};
