#ifndef ORBIS
#include "PCNetController.h"
#include "RigidPhysicsObject.h"
#include "GameObject.h"


PCNetController::PCNetController(ControllerComponent* component, unsigned int id)
{
	m_component = component;
	m_id = id;
}

PCNetController::~PCNetController()
{
}


void PCNetController::Sync(NetSessionReader& reader)
{
	auto* sessionMessages = reader.GetMessages();
	if (!sessionMessages)
	return;

	auto* messageList = sessionMessages->Get(m_id, NetReservedPhysicsMessageType);
	if (!messageList)
		return;

		NetMessageDataPhysicsState* data = reinterpret_cast<NetMessageDataPhysicsState*>(messageList->msg->data);

		Vec3Physics& position = data->position;
		QuatPhysics& orientation = data->orientation;
		Vec3Physics& linearVelocity = data->linearVelocity;
		Vec3Physics& angularVelocity = data->angularVelocity;

		m_component->SetState(
		  position,
		  orientation,
		  linearVelocity,
		  angularVelocity
		);
	}

void PCNetController::Sync(NetSessionWriter& writer)
{
	btRigidBody* body = static_cast<RigidPhysicsObject*>(m_component->GetParent()->GetPhysicsComponent())->GetPhysicsBody();
	const btTransform& transform = body->getWorldTransform();
	NetMessageDataPhysicsState data;
	data.position.x = transform.getOrigin().x();
	data.position.y = transform.getOrigin().y();
	data.position.z = transform.getOrigin().z();

	btQuaternion orientation = transform.getRotation();
	data.orientation.x = orientation.x();
	data.orientation.y = orientation.y();
	data.orientation.z = orientation.z();
	data.orientation.w = orientation.w();

	data.linearVelocity.x = body->getLinearVelocity().x();
	data.linearVelocity.y = body->getLinearVelocity().y();
	data.linearVelocity.z = body->getLinearVelocity().z();

	data.angularVelocity.x = body->getAngularVelocity().x();
	data.angularVelocity.y = body->getAngularVelocity().y();
	data.angularVelocity.z = body->getAngularVelocity().z();

	writer.AddGlobalMessage(NetMessage::Make(data, -1, m_id, NetReservedPhysicsMessageType, NetMessageStrategy::NetLatestMessageStrategy));
}
#endif