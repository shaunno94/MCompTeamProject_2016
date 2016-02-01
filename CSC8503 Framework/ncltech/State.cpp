#include "State.h"


State::State(GameObject& go) {
	m_GameObject = &go;
	m_Active = false;
}


State::~State(void) {
}



void State::Start(State* previousState) { m_Active = true; }

void State::Update(float deltaTime) {}

void State::End() { m_Active = false; }