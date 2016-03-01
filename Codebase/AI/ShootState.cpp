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

	btVector3 goalPos = m_goal->GetPhysicsComponent()->GetPhysicsBody()->getWorldTransform().getOrigin();
	btVector3 ballPos = m_ball->GetPhysicsComponent()->GetPhysicsBody()->getWorldTransform().getOrigin();
	btVector3 parentPos = m_parent->GetPhysicsComponent()->GetPhysicsBody()->getWorldTransform().getOrigin();

	btVector3 aiToBall = ballPos - parentPos;
	btVector3 ballToGoal = goalPos - ballPos;

	aiToBall.normalize();
	ballToGoal.normalize();

	btScalar dot = aiToBall.dot(ballToGoal);

	aiToBall *= 10.0f;
	m_parent->GetControllerComponent()->AddForce(aiToBall.x(), 0.0f, aiToBall.z());
}

