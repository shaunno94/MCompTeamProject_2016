#include "PositioningState.h"

#include <algorithm>
#include "PhysicsObject.h"
#include "GameObject.h"
#include <stdexcept>
#include "PhysicsEngine.h"

PositioningState::PositioningState(GameObject& go, const Vec3Physics& displacement, const QuatPhysics& rotation) : State(go) {
	m_Displacement = displacement;
	m_Rotation = rotation;
	m_LocalTransformCopy = m_GameObject->GetLocalTransform();
	m_Physics = m_GameObject->Physics();
	if (!m_Physics)
		throw std::invalid_argument("Game Object has to have a Physics Object");
}


PositioningState::~PositioningState(void) {
}


void PositioningState::Update(float deltaTime) {
	if (m_Physics->IsEnabled() && !PhysicsEngine::Instance()->IsPaused() ) {
		QuatPhysics frameRotation = m_Rotation;
		frameRotation.x *= deltaTime;
		frameRotation.y *= deltaTime;
		frameRotation.z *= deltaTime;
		frameRotation.w *= deltaTime;

		m_Physics->ExternalRotate(frameRotation);
		m_Physics->ExternalMove(m_Displacement * deltaTime);
	}
}
