#include "Frustum.h"

void Frustum::FromMatrix(const Mat4Graphics &mat) {
	Vec3Graphics xaxis = Vec3Graphics(mat.values[0],mat.values[4],mat.values[8]);
	Vec3Graphics yaxis = Vec3Graphics(mat.values[1],mat.values[5],mat.values[9]);
	Vec3Graphics zaxis = Vec3Graphics(mat.values[2],mat.values[6],mat.values[10]);
	Vec3Graphics waxis = Vec3Graphics(mat.values[3],mat.values[7],mat.values[11]);

	//RIGHT
	planes[0] = Plane(waxis - xaxis, (mat.values[15] - mat.values[12]), true);

	//LEFT
	planes[1] = Plane(waxis + xaxis, (mat.values[15] + mat.values[12]), true);

	//BOTTOM
	planes[2] = Plane(waxis + yaxis, (mat.values[15] + mat.values[13]), true);

	//TOP
	planes[3] = Plane(waxis - yaxis, (mat.values[15] - mat.values[13]), true);

	//FAR
	planes[4] = Plane(waxis - zaxis, (mat.values[15] - mat.values[14]), true);

	//NEAR
	planes[5] = Plane(waxis + zaxis, (mat.values[15] + mat.values[14]), true);
}

bool Frustum::InsideFrustum(const Vec3Graphics& position, float radius)	{
	for (int p = 0; p < 6; p++)	{
		if (!planes[p].SphereInPlane(position, radius)) {
			return false;
		}
	}
	return true;
}

bool Frustum::InsideFrustum(SceneNode&n)	{
	for(int p = 0; p < 6; p++ )	{
		//if(!planes[p].PointInPlane(n.GetWorldTransform().GetPositionVector())) {
		if(!planes[p].SphereInPlane(n.GetWorldTransform().GetTranslation(),n.GetBoundingRadius())) {
			return false;
		}
	}
	return true;
}

bool	Frustum::AABBInsideFrustum(const Vec3Graphics &position, const Vec3Graphics &size)	const {
	for(int p = 0; p < 6; p++ )	{
		if(planes[p].PointInPlane(Vec3Graphics(position.x -size.x , position.y +size.y,position.z +size.z))) {
			continue;
		}
		if(planes[p].PointInPlane(Vec3Graphics(position.x +size.x , position.y +size.y,position.z +size.z))) {
			continue;
		}
		if(planes[p].PointInPlane(Vec3Graphics(position.x -size.x , position.y -size.y,position.z +size.z))) {
			continue;
		}
		if(planes[p].PointInPlane(Vec3Graphics(position.x +size.x , position.y -size.y,position.z +size.z))) {
			continue;
		}

		if(planes[p].PointInPlane(Vec3Graphics(position.x -size.x , position.y +size.y,position.z -size.z))) {
			continue;
		}
		if(planes[p].PointInPlane(Vec3Graphics(position.x +size.x , position.y +size.y,position.z -size.z))) {
			continue;
		}
		if(planes[p].PointInPlane(Vec3Graphics(position.x -size.x , position.y -size.y,position.z -size.z))) {
			continue;
		}
		if(planes[p].PointInPlane(Vec3Graphics(position.x +size.x , position.y -size.y,position.z -size.z))) {
			continue;
		}

		return false;
	}

	return true;
}