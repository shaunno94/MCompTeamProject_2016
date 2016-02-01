#include "CollisionTransition.h"

#include "GameObject.h"
#include "CollisionState.h"
#include <stdexcept>

CollisionTransition::CollisionTransition(State* oldState, State* newState) : StateTransition(oldState, newState) {
	if(!dynamic_cast<CollisionState*>(oldState)) {
		throw std::invalid_argument("oldState is not an instance of CollisionState");
	}
}


CollisionTransition::~CollisionTransition(void) {
}


bool CollisionTransition::Transition() {
	if (m_Active) {
		std::vector<GameObject*>* collisionObject = reinterpret_cast<std::vector<GameObject*>*>(reinterpret_cast<CollisionState*>(m_OldState)->GetData());
		if (collisionObject->size()) {
			return true;
		}
	}
	return false;
}
