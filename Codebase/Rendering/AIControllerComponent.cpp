#include "AIControllerComponent.h"
#include "AI\ChaseState.h"
#include "Renderer.h"
#include "AI\RunAwayState.h"
#include "AI\DistanceTrigger.h"

enum AITypes {
	SHOOTER = 0,
	GOALKEEPER = 1,
	AGGRESSIVE = 2
};

enum AIStates {
	CHASE,
	RUN_AWAY
};

AIControllerComponent::AIControllerComponent(GameObject* parent, unsigned int type) :
ControllerComponent(parent)
{


	GameObject* ball = Renderer::GetInstance()->GetCurrentScene()->findGameObject("ball");

	switch (type)
	{
	case SHOOTER:
	{
					m_StateMachine = new StateMachine();
					ChaseState* chase = new ChaseState(*m_StateMachine, *m_parent, *ball);
					RunAwayState* run = new RunAwayState(*m_StateMachine, *m_parent, *ball);


					// Chase -> Run trigger 
					//	Triggered when two objects are less than 5.0f apart
					DistanceTrigger* chaseToRun = new DistanceTrigger();
					chaseToRun->setupTrigger(*m_parent, *ball, 25.0f, true);
					chase->AddTrigger(chaseToRun, RUN_AWAY);

					m_StateMachine->AddState(CHASE, chase);


					// Run -> Chase trigger 
					//	Triggered when two objects are greater than 25.0f apart
					DistanceTrigger* runToChase = new DistanceTrigger();
					runToChase->setupTrigger(*m_parent, *ball, 65.0f, false);
					run->AddTrigger(runToChase, CHASE);

					m_StateMachine->AddState(RUN_AWAY, run);

					m_StateMachine->ChangeState(CHASE);
	}
		break;
	case GOALKEEPER:
		break;
	case AGGRESSIVE:
		break;
	default:
		break;
	}

}

AIControllerComponent::~AIControllerComponent()
{
}

void AIControllerComponent::updateObject(float dt)
{
	m_StateMachine->Update(dt);
	ControllerComponent::updateObject(dt);
}

void AIControllerComponent::AddForce(float x, float y, float z){
	float clamp = 0.6;

	if (Window::GetWindow().GetKeyboard()->KeyDown(KEYBOARD_SPACE)){
		int i = 0;
	}

	Vec3Physics in(x, y, z);
	//in = getOrientation() * in;
	in.Normalize();

	Vec3Physics forward = getOrientation() * Vec3Physics(0, 0, 1);
	Vec3Physics left = getOrientation() * Vec3Physics(-1, 0, 0);
	forward.Normalize();
	left.Normalize();

	auto rotForce = getOrientation() * in;
	rotForce.Normalize();
	float newX = std::min(std::max(rotForce.x, -clamp), clamp);
	float turningFactor = Lerp(0, 1, newX / clamp);

	float dot = in.Dot(left);

	AddTorque(0, 5 * (dot), 0);

	dot = in.Dot(forward);
	if (dot >= 0){
		force = forward * 7;
	}
	else
	{
		force = -forward * 6;
	}

	force.y = 0;
}
