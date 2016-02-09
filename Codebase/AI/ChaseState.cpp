#include "ChaseState.h"
#include "..\Math\Vector3Simple.h"
#include "Rendering/GameObject.h"

ChaseState::ChaseState(StateMachine& stateMachine, GameObject& parent, GameObject& chaseObj) :
State(stateMachine, parent),
m_chaseObject(&chaseObj)
{
	m_parent = &parent;
	m_chaseObject = &chaseObj;
}

ChaseState::~ChaseState()
{
	delete m_parent;
	delete m_chaseObject;
}

void ChaseState::Start()
{
	//m_parent->SetColour(Vec4Graphics(0.8f, 0.6f, 0.6f, 1.0f));
}

void ChaseState::Update(float dt)
{
	State::Update(dt);

	btVector3 direction = m_chaseObject->Physics()->getCenterOfMassPosition() - m_parent->Physics()->getCenterOfMassPosition();
	direction.normalize();

	m_parent->Physics()->applyCentralForce((direction * 0.5f) * dt);
}
