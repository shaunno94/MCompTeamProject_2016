#include "GameObject.h"

GameObject::GameObject(const std::string& name)
{
	m_Scene = nullptr;
	m_Parent = nullptr;
	m_Name = name;
	m_RigidPhysicsObject = nullptr;
	m_MotionState = nullptr;
	m_ColShape = nullptr;

	m_Colour = Vec4Graphics(1.0f, 1.0f, 1.0f, 1.0f);
	m_BoundingRadius = 1.f;

	m_LocalTransform.ToIdentity();
	m_WorldTransform.ToIdentity();
}

GameObject::~GameObject()
{
	if (m_RigidPhysicsObject)
	{
		delete m_ColShape;
		delete m_MotionState;
		delete m_RigidPhysicsObject;
		delete m_ConstructionInfo;
		m_ConstructionInfo = nullptr;
		m_ColShape = nullptr;
		m_MotionState = nullptr;
		m_RigidPhysicsObject = nullptr;
	}
}

void GameObject::InitPhysics(double mass, Vec3Physics inertia, QuatPhysics orientation,
	Vec3Physics position, PhysicsType type)
{		
	//Motion state (http://www.bulletphysics.org/mediawiki-1.5.8/index.php/MotionStates)
	m_MotionState = new btDefaultMotionState(btTransform(btQuaternion(orientation.x, orientation.y, orientation.z, orientation.w),
		btVector3(position.x, position.y, position.z)));
	
	//Create a rigid body physics object.
	switch (type)
	{	
	case RIGID:	
		//Create a collision shape - a sphere for now...
		m_ColShape = new btSphereShape(1.0);
		m_ColShape->calculateLocalInertia(mass, btVector3(inertia.x, inertia.y, inertia.z));

		//Rigid body object (http://www.bulletphysics.org/mediawiki-1.5.8/index.php/Rigid_Bodies)
		m_ConstructionInfo = new btRigidBody::btRigidBodyConstructionInfo(mass, m_MotionState, m_ColShape, btVector3(0, 0, 0));
		m_RigidPhysicsObject = new btRigidBody(*m_ConstructionInfo);
		//Add the body to the physics environment.
		PhysicsEngineInstance::Instance()->addRigidBody(m_RigidPhysicsObject);
		break;
		//Soft bodies will go here...
	case SOFT:
		break;
	case PARTICLE:
		m_ColShape = new btEmptyShape();

		//Rigid body object (http://www.bulletphysics.org/mediawiki-1.5.8/index.php/Rigid_Bodies)
		m_ConstructionInfo = new btRigidBody::btRigidBodyConstructionInfo(mass, m_MotionState, m_ColShape, btVector3(0, 0, 0));
		m_RigidPhysicsObject = new btRigidBody(*m_ConstructionInfo);
		//Add the body to the physics environment - with group ID 0 and mask 0 this should prevent particles colliding with everything.
		PhysicsEngineInstance::Instance()->addRigidBody(m_RigidPhysicsObject, 0, 0);
		break;
	}
}

GameObject*	GameObject::FindGameObject(const std::string& name)
{
	//Has this object got the same name?
	if (GetName().compare(name) == 0)
	{
		return this;
	}

	//Recursively search ALL child objects and return the first one matching the given name
	for (auto child : m_Children)
	{
		//Has the object in question got the same name?
		GameObject* cObj = child->FindGameObject(name);
		if (cObj)
		{
			return cObj;
		}
	}

	//Object not found with the given name
	return nullptr;
}

void GameObject::AddChildObject(GameObject* child)
{
	m_Children.push_back(child);
	child->m_Parent = this;
	child->m_Scene = this->m_Scene;
}
