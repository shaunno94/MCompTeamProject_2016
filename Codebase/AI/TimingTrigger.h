#pragma once
#include "Trigger.h"

class GameObject;

class TimingTrigger :
	public Trigger
{
public:
	TimingTrigger();
	~TimingTrigger();

	virtual void setupTrigger(GameObject& parent, int numFramesBeforeTrigger);

	virtual bool HasTriggered();

protected:
	GameObject* m_objectB;
	int m_triggerFrames;
	int m_frameCount;
};

