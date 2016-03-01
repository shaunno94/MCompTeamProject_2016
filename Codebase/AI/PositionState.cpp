#include "PositionState.h"
#include "..\Math\Vector3Simple.h"
#include "Rendering/GameObject.h"
#include "Rendering\Renderer.h"
#include "Helpers\curves.h"
#include "Helpers\degrees.h"


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

	btVector3 goalPos = m_goal->GetPhysicsComponent()->GetPhysicsBody()->getWorldTransform().getOrigin();
	btVector3 ballPos = m_ball->GetPhysicsComponent()->GetPhysicsBody()->getWorldTransform().getOrigin();
	btVector3 parentPos = m_parent->GetPhysicsComponent()->GetPhysicsBody()->getWorldTransform().getOrigin();

	btVector3 controlPoint2 = goalPos - ballPos;
	controlPoint2.normalize();

	btVector3 aiToBall = parentPos - ballPos;
	aiToBall.normalize();

	btVector3 start = parentPos;

	btVector3 controlPoint1 = start + m_parent->GetPhysicsComponent()->GetPhysicsBody()->getInterpolationLinearVelocity();
	controlPoint1.normalize();

	// Don't want to charge to the ball if on the wrong side of the ball to the target goal
	//	Use this to move around the ball into position
	btScalar dot = -controlPoint2.dot(aiToBall);

	if (dot < 0)
	{
		// If ball is on the right side of the pitch, you want to go around the left side so as not
		//		to hit the wall
		if (ballPos.z() < 0)
		{
			controlPoint1 = aiToBall.rotate(btVector3(0, 1, 0), -dot * DegToRad(-90.0f));
		}
		// else ball is on the left side of the pitch
		else
		{
			controlPoint1 = aiToBall.rotate(btVector3(0, 1, 0), -dot * DegToRad(90.0f));
		}
	}

	btVector3 dest = ballPos - controlPoint2 * 10.0f;


	controlPoint1 *= 2.0f;
	controlPoint2 *= 2.0f;

	float x = hermiteCurveDerivativeUsingTangents(start.x(), controlPoint1.x(), controlPoint2.x(), dest.x(), 0.01f);
	float z = hermiteCurveDerivativeUsingTangents(start.z(), controlPoint1.z(), controlPoint2.z(), dest.z(), 0.01f);

	m_parent->GetControllerComponent()->AddForce(x, 0.0f, z);
}
