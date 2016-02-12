#include "Frustum.h"

void Frustum::FromMatrix(const Mat4Graphics &mat) 
{
	Vec3Graphics xaxis = Vec3Graphics(mat.values[0], mat.values[4], mat.values[8]);
	Vec3Graphics yaxis = Vec3Graphics(mat.values[1], mat.values[5], mat.values[9]);
	Vec3Graphics zaxis = Vec3Graphics(mat.values[2], mat.values[6], mat.values[10]);
	Vec3Graphics waxis = Vec3Graphics(mat.values[3], mat.values[7], mat.values[11]);

	planes.clear();
	//Create frustum planes - plane constant (D), plane normal
	//RIGHT
	planes.push_back(Plane((mat.values[15] - mat.values[12]), (waxis - xaxis), true));
	
	//LEFT
	planes.push_back(Plane((mat.values[15] + mat.values[12]), (waxis + xaxis), true));

	//BOTTOM
	planes.push_back(Plane((mat.values[15] + mat.values[13]), (waxis + yaxis), true));

	//TOP
	planes.push_back(Plane((mat.values[15] - mat.values[13]), (waxis - yaxis), true));

	//FAR
	planes.push_back(Plane((mat.values[15] - mat.values[14]), (waxis - zaxis), true));

	//NEAR
	planes.push_back(Plane((mat.values[15] + mat.values[14]), (waxis + zaxis), true));
}


bool Frustum::InsideFrustum(GameObject* n)	
{		
	if (!n->Physics())
			return true;

	btVector3 min, max, size, position;
	//Get axis aligned bounding box
	n->Physics()->getAabb(min, max);
	position = n->Physics()->getWorldTransform().getOrigin();
	size = (max - position).absolute();

	//Iterate through frustum planes and perform a point in plane check for each point in the AABB.
	for (auto& p : planes)	
	{
		if (p.PointInPlane(Vec3Physics(position.x() - size.x(), position.y() + size.y(), position.z() + size.z()))) continue;

		if (p.PointInPlane(Vec3Physics(position.x() + size.x(), position.y() + size.y(), position.z() + size.z()))) continue;

		if (p.PointInPlane(Vec3Physics(position.x() - size.x(), position.y() - size.y(), position.z() + size.z()))) continue;

		if (p.PointInPlane(Vec3Physics(position.x() + size.x(), position.y() - size.y(), position.z() + size.z()))) continue;

		if (p.PointInPlane(Vec3Physics(position.x() - size.x(), position.y() + size.y(), position.z() - size.z()))) continue;

		if (p.PointInPlane(Vec3Physics(position.x() + size.x(), position.y() + size.y(), position.z() - size.z()))) continue;

		if (p.PointInPlane(Vec3Physics(position.x() - size.x(), position.y() - size.y(), position.z() - size.z()))) continue;

		if (p.PointInPlane(Vec3Physics(position.x() + size.x(), position.y() - size.y(), position.z() - size.z()))) continue;

		//Object is outside frustum.
		return false;
	}
	//Object is inside frustum.
	return true;
}