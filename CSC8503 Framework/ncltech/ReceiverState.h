#pragma once
#include "DataSourceState.h"


class ReceiverState : public DataSourceState
{
protected:
	void* m_ReceivedData;
public:

	ReceiverState(GameObject& go);

	virtual void Receive(void* data);

	virtual void* GetData() override;
};

