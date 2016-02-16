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

	btVector3 direction = m_parent->GetPhysicsComponent()->GetPhysicsBody()->getWorldTransform().getOrigin() - m_runFromObject->GetPhysicsComponent()->GetPhysicsBody()->getWorldTransform().getOrigin();
	direction.normalize();

	dynamic_cast<RigidPhysicsObject*>(m_parent->GetPhysicsComponent())->GetPhysicsBody()->applyCentralForce((direction * 0.5f) * dt);
}
