#include "SignalState.h"

#include "GameObject.h"
#include "Scene.h"
#include "ReceiverState.h"
#include "DataSourceState.h"

SignalState::SignalState(GameObject& go) : State(go) {
	m_DataSource = nullptr;
}


SignalState::~SignalState(void) {
}

void SignalState::Start(State* previousState) {
	m_DataSource = dynamic_cast<DataSourceState*>(previousState);
}

void SignalState::Update(float deltaTime) {
	if (m_DataSource) {
		for (std::string& str : m_Matches) {
			GameObject* go = m_GameObject->GetScene()->FindGameObject(str);
			if (go) {
				ReceiverState* state = dynamic_cast<ReceiverState*>(go->GetStateMachine()->GetCurrentState());
				if (state)
					state->Receive(m_DataSource->GetData());
			}
		}
	}
}

void SignalState::AddMatch(std::string& str) {
	m_Matches.push_back(str);
}