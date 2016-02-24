#pragma once
#include "Rendering/GameObject.h"
#include "Rendering/Material.h"
#include <string>
#include "Rendering/ModelLoader.h"

class AIAgent : public GameObject
{
public:
	AIAgent(Material* material, GameObject* ball, const std::string& name = "");
	virtual ~AIAgent();

	virtual void OnUpdateObject(float dt);	//Override to handle things like AI etc on update loop

	StateMachine* GetStateMachine() const
	{
		return m_StateMachine;
	}
	void SetStateMachine(StateMachine* stateMachine)
	{
		m_StateMachine = stateMachine;
	}

protected:
	StateMachine*				m_StateMachine;
	GameObject*					m_ball;
};

