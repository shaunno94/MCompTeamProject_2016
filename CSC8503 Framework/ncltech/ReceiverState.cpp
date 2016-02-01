#include "ReceiverState.h"


ReceiverState::ReceiverState(GameObject& go) : DataSourceState(go) {
}

void ReceiverState::Receive(void* data) {
	m_ReceivedData = data;
}
void* ReceiverState::GetData() {
	return m_ReceivedData;
}