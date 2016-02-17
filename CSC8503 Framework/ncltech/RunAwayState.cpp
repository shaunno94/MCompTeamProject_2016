#include "RunAwayState.h"
#include "..\Math\Vector3Simple.h"
#include "GameObject.h"

RunAwayState::RunAwayState(StateMachine& stateMachine, GameObject& parent, GameObject& runFromObj) :
State(stateMachine, parent),
m_runFromObject(&runFromObj)
{
}

RunAwayState::~RunAwayState()
{
	delete m_runFromObject;
}

void RunAwayState::Start()
{
	m_parent->SetColour(Vec4Graphics(0.6f, 0.6f, 0.8f, 1.0f));
}

void RunAwayState::Update(float dt)
{
	State::Update(dt);

	Vector3Simple direction = m_parent->Physics()->GetPosition() - m_runFromObject->Physics()->GetPosition();
	direction.Normalize();

	m_parent->Physics()->SetForce((m_parent->Physics()->GetForce() + (direction * 0.5f)) * dt);
}
