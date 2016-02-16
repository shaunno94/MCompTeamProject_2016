#include "GameObject.h"
#include "PhysicsEngine.h"

GameObject::GameObject(const std::string& name, PhysicsObject* physicsObj) :
	m_Scene(nullptr), m_Parent(nullptr), m_IsBoundingSphere(true), m_Name(name),
	m_Scale(Vec3Physics::ONES), m_Colour(Vec4Graphics::ONES), m_BoundingRadius(1.0f) {

	//Identity is default?
	//m_LocalTransform.ToIdentity();
	//m_WorldTransform.ToIdentity();

	m_PhysicsObject = physicsObj;
	if (m_PhysicsObject)
		m_PhysicsObject->SetParent(*this);

}

GameObject::~GameObject() {
	if (m_PhysicsObject) {
		delete m_PhysicsObject;
		m_PhysicsObject = nullptr;
	}

	if (m_StateMachine) {
		m_StateMachine = nullptr;
	}
}



void GameObject::AddPhysics(PhysicsObject* physicsObj) {
	if (m_PhysicsObject) {
		PhysicsEngine::Instance()->RemovePhysicsObject(m_PhysicsObject);
		delete m_PhysicsObject;
	}
	m_PhysicsObject = physicsObj;
	if (m_PhysicsObject) {
		m_PhysicsObject->SetParent(*this);
		PhysicsEngine::Instance()->AddPhysicsObject(m_PhysicsObject);
	}
}

void GameObject::RemovePhysics() {
	if (m_PhysicsObject) {
		PhysicsEngine::Instance()->RemovePhysicsObject(m_PhysicsObject);
		delete m_PhysicsObject;
		m_PhysicsObject = nullptr;
	}
}


GameObject*			GameObject::FindGameObject(const std::string& name) {
	//Has this object got the same name?
	if (GetName().compare(name) == 0)
		return this;

	//Recursively search ALL child objects and return the first one matching the given name
	for (auto child : m_Children) {
		//Has the object in question got the same name?
		GameObject* cObj = child->FindGameObject(name);
		if (cObj != NULL)
			return cObj;
	}

	//Object not found with the given name
	return NULL;
}

void				GameObject::AddChildObject(GameObject* child) {
	m_Children.push_back(child);
	child->m_Parent = this;
	child->m_Scene = this->m_Scene;
}

void GameObject::Ditach() {
	if (m_PhysicsObject)
		PhysicsEngine::Instance()->RemovePhysicsObject(m_PhysicsObject);
}