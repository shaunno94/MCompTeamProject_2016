#pragma once

class GameObject;

class Trigger
{
public:
	Trigger();
	virtual ~Trigger();

	virtual void setupTrigger(GameObject& parentObject);
	virtual bool HasTriggered() = 0;

protected:
	GameObject* m_parent;
};

