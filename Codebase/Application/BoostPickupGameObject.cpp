#include "BoostPickupGameObject.h"
#include "Rendering/ModelLoader.h"
#include "PhysicsEngine/PhysicsEngineInstance.h"
#include "GameScene.h"

BoostPickupCollisionFilterStep::BoostPickupCollisionFilterStep(GameObject* obj) : ParticleFilterStep(COL_GROUP_DEFAULT, COL_PICKUP | COL_CAR)
{
	m_obj = obj;
	m_broadphaseProxyId = obj->GetPhysicsComponent()->GetPhysicsBody()->getBroadphaseHandle()->getUid();
}

struct DisablePickupCallback : public btCollisionWorld::ContactResultCallback
{
	inline DisablePickupCallback(PickupGameObject* pickup, GameObject* obj) : pickup(pickup), obj(obj) {}

	virtual bool needsCollision(btBroadphaseProxy* proxy0) const
	{
		return true;
	}

	virtual btScalar addSingleResult(btManifoldPoint& cp, const btCollisionObjectWrapper* colObj0Wrap, int partId0, int index0, const btCollisionObjectWrapper* colObj1Wrap, int partId1, int index1)
	{
		pickup->Disable(5000);
		obj->GetControllerComponent()->boost = 1.0;

		SoundMOD mod;
		mod.looping = false;
		mod.isGlobal = true;
		mod.pitch = 1.7f;
		mod.volume = 0.7f;
		SoundSystem::Instance()->Play(SoundManager::GetSound(POWERUP), mod);
		return 0;
	}

	PickupGameObject* pickup;
	GameObject* obj;
};

bool BoostPickupCollisionFilterStep::callback(btBroadphaseProxy* proxy0, btBroadphaseProxy* proxy1) const
{
	if (proxy0->m_uniqueId == m_broadphaseProxyId)
	{
		auto scene = dynamic_cast<GameScene*>(Renderer::GetInstance()->GetCurrentScene());
		if (scene)
		{
			auto pickupObj = scene->GetPickupManager()->FindPickup(proxy1->m_uniqueId);
			if (pickupObj)
			{
				DisablePickupCallback callback(pickupObj, m_obj);
				PhysicsEngineInstance::Instance()->contactPairTest(pickupObj->GetPhysicsComponent()->GetPhysicsBody(), m_obj->GetPhysicsComponent()->GetPhysicsBody(), callback);
			}
		}
	}
	else if (proxy1->m_uniqueId == m_broadphaseProxyId)
	{
		auto scene = dynamic_cast<GameScene*>(Renderer::GetInstance()->GetCurrentScene());
		if (scene)
		{
			auto pickupObj = scene->GetPickupManager()->FindPickup(proxy0->m_uniqueId);
			if (pickupObj)
			{
				DisablePickupCallback callback(pickupObj, m_obj);
				PhysicsEngineInstance::Instance()->contactPairTest(pickupObj->GetPhysicsComponent()->GetPhysicsBody(), m_obj->GetPhysicsComponent()->GetPhysicsBody(), callback);
			}
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
