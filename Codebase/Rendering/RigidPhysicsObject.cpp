#include "RigidPhysicsObject.h"

RigidPhysicsObject::RigidPhysicsObject()
{
	m_MotionState = nullptr;
	m_ConstructionInfo = nullptr;
	m_BodyType = RIGID;
}

bool RigidPhysicsObject::CreatePhysicsBody(double mass, const Vec3Physics& position, const QuatPhysics& orientation, const Vec3Physics& inertia, bool particle)
{
	if (!m_ColShape)
		return false;

	//Motion state (http://www.bulletphysics.org/mediawiki-1.5.8/index.php/MotionStates)
	m_MotionState = new btDefaultMotionState(btTransform(btQuaternion(orientation.x, orientation.y, orientation.z, orientation.w),
		btVector3(position.x, position.y, position.z)));
	m_ColShape->calculateLocalInertia(mass, btVector3(inertia.x, inertia.y, inertia.z));

	//Create a physics object.
	//Rigid body object (http://www.bulletphysics.org/mediawiki-1.5.8/index.php/Rigid_Bodies)
	m_ConstructionInfo = new btRigidBody::btRigidBodyConstructionInfo(mass, m_MotionState, m_ColShape, btVector3(0, 0, 0));
	m_CollisionObject = new btRigidBody(*m_ConstructionInfo);

	if (particle)
		m_CollisionObject->setCollisionFlags(btCollisionObject::CF_NO_CONTACT_RESPONSE);

	//Add the body to the physics environment
	PhysicsEngineInstance::Instance()->addRigidBody(static_cast<btRigidBody*>(m_CollisionObject));
	return true;
}

btRigidBody* RigidPhysicsObject::GetPhysicsBody()
{
	return static_cast<btRigidBody*>(m_CollisionObject);
}

RigidPhysicsObject::~RigidPhysicsObject()
{
	if (m_ConstructionInfo)
	{
		delete m_ConstructionInfo;
		m_ConstructionInfo = nullptr;
	}
}