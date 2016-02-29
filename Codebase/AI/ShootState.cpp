#include "ShootState.h"
#include "Rendering\Renderer.h"
#include "Helpers\curves.h"


ShootState::ShootState(StateMachine& stateMachine, GameObject& parent, GameObject& ball) :
State(stateMachine, parent),
m_ball(&ball),
m_hit(false)
{
	m_goal = Renderer::GetInstance()->GetCurrentScene()->findGameObject("goal1");
}


ShootState::~ShootState()
{
}


void ShootState::Start()
{
	m_hit = false;
}

void ShootState::Update(float dt)
{
	State::Update(dt);

	btVector3 direction = m_goal->GetPhysicsComponent()->GetPhysicsBody()->getWorldTransform().getOrigin() - m_ball->GetPhysicsComponent()->GetPhysicsBody()->getWorldTransform().getOrigin();

	direction.normalize();
	direction *= 10.0f;

	m_parent->GetControllerComponent()->AddForce(direction.x(), 0.0f, direction.z());
}

