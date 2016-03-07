#include "AggressiveState.h"
#include "LinearMath\btVector3.h"


AggressiveState::AggressiveState(StateMachine& stateMachine, GameObject& parent, GameObject& ball, GameObject& player) :
State(stateMachine, parent),
m_ball(&ball),
m_player(&player)
{
	m_playerPhys = dynamic_cast<RigidPhysicsObject*>(m_player->GetPhysicsComponent());
}


AggressiveState::~AggressiveState()
{
}

void AggressiveState::Start()
{

}

void AggressiveState::Update(float dt)
{
	State::Update(dt);

	btVector3 parentPos = m_parent->GetPhysicsComponent()->GetPhysicsBody()->getWorldTransform().getOrigin();
	btVector3 playerPos = m_player->GetPhysicsComponent()->GetPhysicsBody()->getWorldTransform().getOrigin();

	btVector3 playerVel = m_playerPhys->GetPhysicsBody()->getLinearVelocity();

	btVector3 desiredPos = playerPos + playerVel;

	btVector3 direction = desiredPos - parentPos;

	m_parent->GetControllerComponent()->AddForce(direction.x(), 0.0f, direction.z());

}
