#include "SoftPhysicsObject.h"

SoftPhysicsObject::SoftPhysicsObject()
{
	m_BodyType = SOFT;
	m_SoftBodyInfo = nullptr;
}

bool SoftPhysicsObject::CreatePhysicsBody(double mass, const Vec3Physics& position, const QuatPhysics& orientation, const Vec3Physics& inertia, bool particle)
{
	if (!m_ColShape)
		return false;

	btVector3 btInertia = btVector3(inertia.x, inertia.y, inertia.z);
	m_ColShape->calculateLocalInertia(mass, btInertia);

	m_SoftBodyInfo = new btSoftBodyWorldInfo();
	m_CollisionObject = new btSoftBody(m_SoftBodyInfo);

	m_CollisionObject->setWorldTransform(btTransform(btQuaternion(orientation.x, orientation.y, orientation.z, orientation.w), 
		btVector3(position.x, position.y, position.z)));
	m_CollisionObject->setCollisionShape(m_ColShape);

	if (particle)
		m_CollisionObject->setCollisionFlags(btCollisionObject::CF_NO_CONTACT_RESPONSE);

	//Add the body to the physics environment
	PhysicsEngineInstance::Instance()->addSoftBody(static_cast<btSoftBody*>(m_CollisionObject));
	return true;
}

btSoftBody* SoftPhysicsObject::GetPhysicsBody()
{
	return static_cast<btSoftBody*>(m_CollisionObject);
}

SoftPhysicsObject::~SoftPhysicsObject()
{
	if (m_SoftBodyInfo)
	{
		delete m_SoftBodyInfo;
		m_SoftBodyInfo = nullptr;
	}
}