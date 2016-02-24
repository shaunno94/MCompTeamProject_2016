#include "ShooterAgent.h"

ShooterAgent::ShooterAgent(Material* material, GameObject* ball, const std::string& name /*= ""*/) :
	AIAgent(material, ball, name)
{
	StateMachine* ballStateMachine = new StateMachine();

	ChaseState* chase = new ChaseState(*ballStateMachine, *this, *m_ball);
	RunAwayState* run = new RunAwayState(*ballStateMachine, *this, *m_ball);


	// Chase -> Run trigger 
	//	Triggered when two objects are less than 5.0f apart
	DistanceTrigger* chaseToRun = new DistanceTrigger();
	chaseToRun->setupTrigger(*this, *m_ball, 10.0f, true);
	chase->AddTrigger(chaseToRun, "Run");

	ballStateMachine->AddState("Chase", chase);


	// Run -> Chase trigger 
	//	Triggered when two objects are greater than 25.0f apart
	DistanceTrigger* runToChase = new DistanceTrigger();
	runToChase->setupTrigger(*this, *m_ball, 25.0f, false);
	run->AddTrigger(runToChase, "Chase");

	ballStateMachine->AddState("Run", run);

	ballStateMachine->ChangeState("Chase");

	this->SetStateMachine(ballStateMachine);
}

ShooterAgent::~ShooterAgent()
{
}
