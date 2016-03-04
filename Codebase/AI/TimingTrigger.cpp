#include "TimingTrigger.h"
#include <iostream>


TimingTrigger::TimingTrigger()
{
}


TimingTrigger::~TimingTrigger()
{
}

void TimingTrigger::setupTrigger(GameObject& parent, int numFramesBeforeTrigger)
{
	Trigger::setupTrigger(parent);
	m_triggerFrames = numFramesBeforeTrigger;
	m_frameCount = 0;
}

bool TimingTrigger::HasTriggered()
{
	m_frameCount++;

	if (m_frameCount == m_triggerFrames) {
		m_frameCount = 0;
		std::cout << "SHOOT -> POSITION" << std::endl;
		return true;
	}

	return false;
}
