#include "Frustum.h"

void Frustum::FromMatrix(const Mat4Graphics &mat) 
{
	Vec3Graphics xaxis = Vec3Graphics(mat.values[0], mat.values[4], mat.values[8]);
	Vec3Graphics yaxis = Vec3Graphics(mat.values[1], mat.values[5], mat.values[9]);
	Vec3Graphics zaxis = Vec3Graphics(mat.values[2], mat.values[6], mat.values[10]);
	Vec3Graphics waxis = Vec3Graphics(mat.values[3], mat.values[7], mat.values[11]);

	for (auto p : planes)
	{
		if (p)
		{
			delete p;
			p = nullptr;
		}	
	}
	planes.clear();
	Plane* p;

	//RIGHT
	p = new Plane;
	p->m_CollisionShape = new btStaticPlaneShape((btVector3(waxis.x, waxis.y, waxis.z) - btVector3(xaxis.x, xaxis.y, xaxis.z)).normalize(), (mat.values[15] - mat.values[12]));
	p->m_MotionState = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(0, 0, 0)));
	p->m_ConstructionInfo = new btRigidBody::btRigidBodyConstructionInfo(0, p->m_MotionState, p->m_CollisionShape, btVector3(0, 0, 0));
	p->m_RigidBody = new btRigidBody(*p->m_ConstructionInfo);
	planes.push_back(p);

	//LEFT
	p = new Plane;
	p->m_CollisionShape = new btStaticPlaneShape((btVector3(waxis.x, waxis.y, waxis.z) + btVector3(xaxis.x, xaxis.y, xaxis.z)).normalize(), (mat.values[15] + mat.values[12]));
	p->m_MotionState = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(0, 0, 0)));
	p->m_ConstructionInfo = new btRigidBody::btRigidBodyConstructionInfo(0, p->m_MotionState, p->m_CollisionShape, btVector3(0, 0, 0));
	p->m_RigidBody = new btRigidBody(*p->m_ConstructionInfo);
	planes.push_back(p);

	//BOTTOM
	p = new Plane;
	p->m_CollisionShape = new btStaticPlaneShape((btVector3(waxis.x, waxis.y, waxis.z) + btVector3(yaxis.x, yaxis.y, yaxis.z)).normalize(), (mat.values[15] + mat.values[13]));
	p->m_MotionState = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(0, 0, 0)));
	p->m_ConstructionInfo = new btRigidBody::btRigidBodyConstructionInfo(0, p->m_MotionState, p->m_CollisionShape, btVector3(0, 0, 0));
	p->m_RigidBody = new btRigidBody(*p->m_ConstructionInfo);
	planes.push_back(p);

	//TOP
	p = new Plane;
	p->m_CollisionShape = new btStaticPlaneShape((btVector3(waxis.x, waxis.y, waxis.z) - btVector3(yaxis.x, yaxis.y, yaxis.z)).normalize(), (mat.values[15] - mat.values[13]));
	p->m_MotionState = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(0, 0, 0)));
	p->m_ConstructionInfo = new btRigidBody::btRigidBodyConstructionInfo(0, p->m_MotionState, p->m_CollisionShape, btVector3(0, 0, 0));
	p->m_RigidBody = new btRigidBody(*p->m_ConstructionInfo);
	planes.push_back(p);

	//FAR
	p = new Plane;
	p->m_CollisionShape = new btStaticPlaneShape((btVector3(waxis.x, waxis.y, waxis.z) - btVector3(zaxis.x, zaxis.y, zaxis.z)).normalize(), (mat.values[15] - mat.values[14]));
	p->m_MotionState = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(0, 0, 0)));
	p->m_ConstructionInfo = new btRigidBody::btRigidBodyConstructionInfo(0, p->m_MotionState, p->m_CollisionShape, btVector3(0, 0, 0));
	p->m_RigidBody = new btRigidBody(*p->m_ConstructionInfo);
	planes.push_back(p);

	//NEAR
	p = new Plane;
	p->m_CollisionShape = new btStaticPlaneShape((btVector3(waxis.x, waxis.y, waxis.z) + btVector3(zaxis.x, zaxis.y, zaxis.z)).normalize(), (mat.values[15] + mat.values[14]));
	p->m_MotionState = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(0, 0, 0)));
	p->m_ConstructionInfo = new btRigidBody::btRigidBodyConstructionInfo(0, p->m_MotionState, p->m_CollisionShape, btVector3(0, 0, 0));
	p->m_RigidBody = new btRigidBody(*p->m_ConstructionInfo);
	planes.push_back(p);
}


bool Frustum::InsideFrustum(GameObject* n)	
{		
	if (!n->Physics())
			return true;
	for (auto& p : planes)
	{
		PhysicsEngineInstance::Instance()->contactPairTest(n->Physics(), p->m_RigidBody, callback);
	}
	return true;
}