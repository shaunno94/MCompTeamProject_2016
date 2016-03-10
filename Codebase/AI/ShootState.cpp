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

	btVector3 aiToBall = ballPos - parentPos;

	aiToBall.normalize();

	aiToBall *= 10.0f;
	m_parent->GetControllerComponent()->AddForce(aiToBall.x(), 0.0f, aiToBall.z());
}

