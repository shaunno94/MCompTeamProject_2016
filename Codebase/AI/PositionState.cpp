#include "PositionState.h"
#include "..\Math\Vector3Simple.h"
#include "Rendering/GameObject.h"
#include "Rendering\Renderer.h"
#include "Helpers\curves.h"


PositionState::PositionState(StateMachine& stateMachine, GameObject& parent, GameObject& ball) :
State(stateMachine, parent),
m_ball(&ball)
{
	m_goal = Renderer::GetInstance()->GetCurrentScene()->findGameObject("goal1");
}


PositionState::~PositionState()
{
}

void PositionState::Start()
{
}

void PositionState::Update(float dt)
{

	State::Update(dt);

	
	btVector3 controlPoint2 = m_goal->GetPhysicsComponent()->GetPhysicsBody()->getWorldTransform().getOrigin() - m_ball->GetPhysicsComponent()->GetPhysicsBody()->getWorldTransform().getOrigin();
	controlPoint2.normalize();

	btVector3 dest = m_ball->GetPhysicsComponent()->GetPhysicsBody()->getWorldTransform().getOrigin() - controlPoint2 * 10.0f;
	btVector3 start = m_parent->GetPhysicsComponent()->GetPhysicsBody()->getWorldTransform().getOrigin();

	btVector3 controlPoint1 = start + m_parent->GetPhysicsComponent()->GetPhysicsBody()->getInterpolationLinearVelocity();
		controlPoint1.normalize();

	controlPoint1 *= 2.0f;
	controlPoint2 *= 2.0f;

	float x = hermiteCurveDerivativeUsingTangents(start.x(), controlPoint1.x(), controlPoint2.x(), dest.x(), 0.01f);
	float z = hermiteCurveDerivativeUsingTangents(start.z(), controlPoint1.z(), controlPoint2.z(), dest.z(), 0.01f);

	m_parent->GetControllerComponent()->AddForce(x, 0.0f, z);
}
