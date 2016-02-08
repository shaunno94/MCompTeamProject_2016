#pragma once
#include "GameObject.h"

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
	struct ContactFrustumCallback : public btCollisionWorld::ContactResultCallback
	{
		btScalar addSingleResult(btManifoldPoint& cp, const btCollisionObjectWrapper* colObj0Wrap, int partId0, int index0, 
		const btCollisionObjectWrapper* colObj1Wrap, int partId1, int index1)
		{
			cp.getPositionWorldOnA();
			// your callback code here
			return 0;
		}
	};
	struct Plane
	{
		~Plane()
		{ 
			delete m_CollisionShape; 
			delete m_MotionState;
			delete m_RigidBody;
			delete m_ConstructionInfo;
		}
		btDefaultMotionState*		m_MotionState;
		btCollisionShape*			m_CollisionShape;
		btRigidBody::btRigidBodyConstructionInfo* m_ConstructionInfo;
		btRigidBody* m_RigidBody;
	};
	ContactFrustumCallback callback;
	vector<Plane*> planes;
};
