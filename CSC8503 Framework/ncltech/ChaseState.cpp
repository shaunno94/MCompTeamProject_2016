#include "ChaseState.h"
#include "..\Math\Vector3Simple.h"
#include "GameObject.h"

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
	m_parent->SetColour(Vec4Graphics(0.8f, 0.6f, 0.6f, 1.0f));
}

void ChaseState::Update(float dt)
{
	State::Update(dt);

	Vector3Simple direction = m_chaseObject->Physics()->GetPosition() - m_parent->Physics()->GetPosition();
	direction.Normalize();

	m_parent->Physics()->SetForce((m_parent->Physics()->GetForce() + (direction * 0.5f)) * dt);
}
