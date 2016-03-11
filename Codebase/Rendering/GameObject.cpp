#include "GameObject.h"

unsigned int GameObject::id = 0;

GameObject::GameObject(const std::string& name)
{
	m_Parent = nullptr;
	m_Name = name;
	m_PhysicsObj = nullptr;
	m_RenderComponent = nullptr;
	m_Audio = nullptr;

	m_BoundingRadius = 1.0f;
	m_CamDist = 0.0f;
	m_Controller = nullptr;
	id++;

	m_spawnPoint = Vec3Graphics(0, 0, 0);

	m_LocalTransform.ToIdentity();
	m_WorldTransform.ToIdentity();
}

GameObject::~GameObject()
{
	if (m_PhysicsObj)
	{
		delete m_PhysicsObj;
		m_PhysicsObj = nullptr;
	}
	if (m_RenderComponent) 
	{
		delete m_RenderComponent;
		m_RenderComponent = nullptr;
	}
	if (m_Controller)
	{
		delete m_Controller;
		m_Controller = nullptr;
	}
	if (m_Audio)
	{
		delete m_Audio;
		m_Audio = nullptr;
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
}

void GameObject::OnRenderObject()				
{
	if (m_RenderComponent)
		m_RenderComponent->Draw();
}

void GameObject::OnUpdateObject(float dt)
{
	if (m_Controller)
		m_Controller->updateObject(dt);

	if (m_Audio)
		m_Audio->Update();

	UpdateTransform();

	for (auto child : m_Children)
	{
		child->OnUpdateObject(dt);
	}
}

void GameObject::UpdateTransform()
{
	if (!m_PhysicsObj)
		m_WorldTransform = m_LocalTransform;
	else
	{
		//Bullet physics native containers: matrix, vec3, quaternion
		btTransform trans;
		btVector3 pos;
		btQuaternion rot;

		//Our containers
		Vec3Graphics p;
		QuatGraphics r;

		//Get current position and rotation of object.
		trans = m_PhysicsObj->GetPhysicsBody()->getWorldTransform();
		pos = trans.getOrigin();
		rot = trans.getRotation();

		//Convert
		p = Vec3Graphics(pos.x(), pos.y(), pos.z());
		r = QuatGraphics(rot.x(), rot.y(), rot.z(), rot.w());
	
		//Update model matrix 
		m_WorldTransform = Mat4Graphics::Translation(p) * r.ToMatrix4() * m_LocalTransform;
	}

	if (m_Parent)
		m_WorldTransform =  m_Parent->m_WorldTransform * m_WorldTransform;
}