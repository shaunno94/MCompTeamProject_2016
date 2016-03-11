#include "ShootState.h"
#include "Rendering\Renderer.h"
#include "Helpers\curves.h"


ShootState::ShootState(StateMachine& stateMachine, GameObject& parent, GameObject& ball) :
State(stateMachine, parent),
m_ball(&ball)
{
}


ShootState::~ShootState()
{
}


void ShootState::Start()
{
}

void ShootState::Update(float dt)
{
	State::Update(dt);

	btVector3 ballPos = m_ball->GetPhysicsComponent()->GetPhysicsBody()->getWorldTransform().getOrigin();
	btVector3 parentPos = m_parent->GetPhysicsComponent()->GetPhysicsBody()->getWorldTransform().getOrigin();

	btVector3 vel = dynamic_cast<RigidPhysicsObject*>(m_ball->GetPhysicsComponent())->GetPhysicsBody()->getLinearVelocity();

	btVector3 desiredPos = ballPos + vel;
	btVector3 aiToDesired = desiredPos - parentPos;

	aiToDesired.normalize();

	aiToDesired *= 10.0f;
	m_parent->GetControllerComponent()->AddForce(aiToDesired.x(), 0.0f, aiToDesired.z());
}

