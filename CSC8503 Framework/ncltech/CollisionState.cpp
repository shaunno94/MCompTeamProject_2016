#include "CollisionState.h"

#include <algorithm>

#include "PhysicsObject.h"
#include "GameObject.h"

CollisionState::CollisionState(GameObject& go) : DataSourceState(go) {
	m_WritingIndex = 1;
	m_OriginalCallback = go.Physics()->GetOnCollisionCallback();
	m_UpdateRequest = false;
	m_CollisionCallback = [&](PhysicsObject* colliding_obj) {
		if (m_UpdateRequest) {
			m_WritingIndex = (m_WritingIndex + 1) % 2;
			m_Targets[m_WritingIndex].clear();
			m_UpdateRequest = false;
		}
		auto match = std::find(m_Targets[m_WritingIndex].begin(), m_Targets[m_WritingIndex].end(), colliding_obj->GetParent());
		if (match == m_Targets[m_WritingIndex].end()) {
			const std::string& name = colliding_obj->GetParent()->GetName();
			for (std::string& str : m_Matches) {
				if (name.find(str) != std::string::npos)
					m_Targets[m_WritingIndex].push_back(colliding_obj->GetParent());
			}
		}
		return m_OriginalCallback(colliding_obj);
	};
}


CollisionState::~CollisionState(void) {
}

void CollisionState::Start(State* previousState) {
	m_UpdateRequest = true;
	m_GameObject->Physics()->SetOnCollisionCallback(m_CollisionCallback);
}

void CollisionState::Update(float deltaTime) {
	m_UpdateRequest = true;
}

void CollisionState::AddMatch(std::string& str) {
	m_Matches.push_back(str);
}

void CollisionState::End() {
	m_GameObject->Physics()->SetOnCollisionCallback(m_OriginalCallback);
}

void* CollisionState::GetData() {
	m_ReturnValue = m_Targets[(m_WritingIndex + 1) % 2];
	return &m_ReturnValue;
}