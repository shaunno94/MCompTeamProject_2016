#include "BoostPickupGameObject.h"
#include "Rendering/ModelLoader.h"
#include "PhysicsEngine/PhysicsEngineInstance.h"
#include "GameScene.h"

BoostPickupCollisionFilterStep::BoostPickupCollisionFilterStep(GameObject* obj) : ParticleFilterStep(COL_GROUP_DEFAULT, COL_PICKUP | COL_CAR)
{
	m_obj = obj;
	m_broadphaseProxyId = obj->GetPhysicsComponent()->GetPhysicsBody()->getBroadphaseHandle()->getUid();
}

bool BoostPickupCollisionFilterStep::callback(btBroadphaseProxy* proxy0, btBroadphaseProxy* proxy1) const
{
	if (proxy0->m_uniqueId == m_broadphaseProxyId)
	{
		auto scene = dynamic_cast<GameScene*>(Renderer::GetInstance()->GetCurrentScene());
		if (scene)
		{
			m_obj->GetControllerComponent()->boost = 1.0;
			scene->GetPickupManager()->FindPickup(proxy1->m_uniqueId)->Disable(5000);
		}
	}
	else if (proxy1->m_uniqueId == m_broadphaseProxyId)
	{
		auto scene = dynamic_cast<GameScene*>(Renderer::GetInstance()->GetCurrentScene());
		if (scene)
		{
			m_obj->GetControllerComponent()->boost = 1.0;
			scene->GetPickupManager()->FindPickup(proxy0->m_uniqueId)->Disable(5000);
		}
	}
	return true;
}

BoostPickupGameObject::BoostPickupGameObject(Material* mat, Mesh* mesh, const Vec3Physics& pos) : PickupGameObject("boost", 6, pos)
{
	SetLocalTransform(Mat4Graphics::Scale(Vec3Graphics(3, 3, 3)));
	SetRenderComponent(new RenderComponent(mat, mesh));
}


BoostPickupGameObject::~BoostPickupGameObject()
{
}
