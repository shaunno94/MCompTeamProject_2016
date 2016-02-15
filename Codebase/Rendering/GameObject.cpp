#include "GameObject.h"

GameObject::GameObject(const std::string& name)
{
	m_Parent = nullptr;
	m_Name = name;
	m_RigidPhysicsObject = nullptr;
	m_MotionState = nullptr;
	m_ColShape = nullptr;
	m_RenderComponent = nullptr;
	m_BoundingRadius = 1.0f;
	m_CamDist = 0.0f;

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
	if (m_RenderComponent) {
		delete m_RenderComponent;
		m_RenderComponent = nullptr;
	}
}

bool GameObject::InitPhysics(double mass, const Vec3Physics& position, const QuatPhysics& orientation, const Vec3Physics& inertia, BodyType type)
{		
	if (!m_ColShape)
		return false;

	//Motion state (http://www.bulletphysics.org/mediawiki-1.5.8/index.php/MotionStates)
	m_MotionState = new btDefaultMotionState(btTransform(btQuaternion(orientation.x, orientation.y, orientation.z, orientation.w),
		btVector3(position.x, position.y, position.z)));
	m_ColShape->calculateLocalInertia(mass, btVector3(inertia.x, inertia.y, inertia.z));
	
	//Create a physics object.
	switch (type)
	{	
	case RIGID:	
		//Rigid body object (http://www.bulletphysics.org/mediawiki-1.5.8/index.php/Rigid_Bodies)
		m_ConstructionInfo = new btRigidBody::btRigidBodyConstructionInfo(mass, m_MotionState, m_ColShape, btVector3(0, 0, 0));
		m_RigidPhysicsObject = new btRigidBody(*m_ConstructionInfo);
		//Add the body to the physics environment.
		//PhysicsEngineInstance::Instance()->addRigidBody(m_RigidPhysicsObject);
		break;
	case SOFT://Soft bodies will go here...
		break;
	case PARTICLE: //There isnt a particle type so I have set the collsion flag to 'CF_NO_CONTACT_RESPONSE' these objects should be exempt from collision response.
		m_ConstructionInfo = new btRigidBody::btRigidBodyConstructionInfo(mass, m_MotionState, m_ColShape, btVector3(0, 0, 0));
		m_RigidPhysicsObject = new btRigidBody(*m_ConstructionInfo);
		m_RigidPhysicsObject->setCollisionFlags(btCollisionObject::CF_NO_CONTACT_RESPONSE);
		//Add the body to the physics environment - with group ID 0 and mask 0 this should prevent particles colliding with everything.
		//PhysicsEngineInstance::Instance()->addRigidBody(m_RigidPhysicsObject, 0, 0);
		break;
	default:
		return false;
	}
	//Add the body to the physics environment
	PhysicsEngineInstance::Instance()->addRigidBody(m_RigidPhysicsObject);
	return true;
}

bool GameObject::CreateCollisionShape(double radius)
{
	m_ColShape = new btSphereShape(radius);
	return true;
}

bool GameObject::CreateCollisionShape(const Vec3Physics& half_extents, CollisionShape shape)
{
	if (shape == CUBOID)
	{
		m_ColShape = new btBoxShape(btVector3(half_extents.x, half_extents.y, half_extents.z));
		return true;
	}
	else if (shape == CYLINDER)
	{
		m_ColShape = new btCylinderShape(btVector3(half_extents.x, half_extents.y, half_extents.z));
		return true;
	}
	return false;
}

bool GameObject::CreateCollisionShape(double radius, double height, CollisionShape shape)
{
	if (shape == CONE)
	{
		m_ColShape = new btConeShape(radius, height);
		return true;
	}
	else if (shape == CAPSULE)
	{
		m_ColShape = new btCapsuleShape(radius, height);
		return true;
	}
	return false;
}

bool GameObject::CreateCollisionShape(double distance, const Vec3Physics& normal, bool normalised)
{
	btVector3 norm = btVector3(normal.x, normal.y, normal.z);
	if (!normalised)
		norm.normalize();

	m_ColShape = new btStaticPlaneShape(btVector3(normal.x, normal.y, normal.z), distance);
	return true;
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
}


//TODO:: Parent-child relationship needs to be undone for sorted drawing!!
void GameObject::OnRenderObject()				
{
	for (auto child : m_Children)
	{
		child->OnRenderObject();
	}
	if (m_RenderComponent)
		m_RenderComponent->Draw();
}

void GameObject::OnUpdateObject(float dt)
{
	for (auto child : m_Children)
	{
		child->OnUpdateObject(dt);
	}
	UpdateTransform();
}

void GameObject::UpdateTransform()
{
	if (!m_RigidPhysicsObject)
		return;

	//Bullet physics native containers: matrix, vec3, quaternion
	btTransform trans;
	btVector3 pos;
	btQuaternion rot;

	//Our containers
	Vec3Graphics p;
	QuatGraphics r;

	//Get current position and rotation of object.
	m_RigidPhysicsObject->getMotionState()->getWorldTransform(trans);
	pos = trans.getOrigin();
	rot = trans.getRotation();

	//Convert
	p = Vec3Graphics(pos.x(), pos.y(), pos.z());
	r = QuatGraphics(rot.x(), rot.y(), rot.z(), rot.w());
	
	//Update model matrix 
	m_WorldTransform = r.ToMatrix4() * Mat4Graphics::Translation(p) * m_LocalTransform;

	if (m_Parent)
		m_WorldTransform = m_Parent->m_WorldTransform * m_WorldTransform;
}