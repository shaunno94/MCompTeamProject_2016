#include "GoalCollisionFilterStep.h"
#include "Rendering/Renderer.h"
#include "GameScene.h"

GoalCollisionFilterStep::GoalCollisionFilterStep() : ParticleFilterStep(COL_GROUP_DEFAULT, COL_BALL | COL_GOAL1 | COL_GOAL2)
{
}

bool GoalCollisionFilterStep::callback(btBroadphaseProxy* proxy0, btBroadphaseProxy* proxy1) const
{
	auto scene = dynamic_cast<GameScene*>(Renderer::GetInstance()->GetCurrentScene());
	if (scene)
	{
		if ((proxy0->m_collisionFilterMask | proxy1->m_collisionFilterMask) & COL_GOAL1)
			scene->SetGoalScored(1);
		else
			scene->SetGoalScored(2);
	}
	return true;
}