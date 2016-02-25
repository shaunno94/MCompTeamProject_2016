#pragma once
#include "AIAgent.h"
#include "ChaseState.h"
#include "RunAwayState.h"
#include "DistanceTrigger.h"

class ShooterAgent : public AIAgent
{
public:
	ShooterAgent(Material* material, GameObject* ball, const std::string& name = "");
	~ShooterAgent();
};

