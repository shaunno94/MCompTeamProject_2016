#include "RunAwayState.h"
#include "..\Math\Vector3Simple.h"
#include "Rendering/GameObject.h"

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
	//m_parent->SetColour(Vec4Graphics(0.6f, 0.6f, 0.8f, 1.0f));
}

void RunAwayState::Update(float dt)
{
	State::Update(dt);

	btVector3 direction = m_parent->Physics()->getCenterOfMassPosition() - m_runFromObject->Physics()->getCenterOfMassPosition();
	direction.normalize();

	m_parent->Physics()->applyCentralForce((direction * 0.5f) * dt);
}
