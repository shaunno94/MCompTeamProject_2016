#include "State.h"


State::State(string name, GameObject& go) {
	m_GameObject = &go;
	m_Active = false;
	stateName = name;
}


State::~State(void) {
}



void State::Start() { m_Active = true; }

void State::End() { m_Active = false; }