#pragma once

class GameObject;

class State
{
protected:
	GameObject* m_GameObject;
	bool m_Active;

public:
	State(GameObject& go);
	virtual ~State(void);

	virtual void Start(State* previousState);

	virtual void Update(float deltaTime);

	virtual void End();
};

