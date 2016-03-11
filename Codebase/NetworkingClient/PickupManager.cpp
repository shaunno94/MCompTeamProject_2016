#include "PickupManager.h"
#include "BoostPickupGameObject.h"
#include "Rendering/ModelLoader.h"


PickupManager::PickupManager(Material* material)
{
	m_boostMesh = ModelLoader::LoadMGL(MODEL_DIR"Boost/boost.mgl", true);

	PickupGameObject* pickup0 = new BoostPickupGameObject(material, m_boostMesh, Vec3Physics(150, 2.0, 50));
	m_pickups.insert({ pickup0->GetBroadphaseProxyId(), pickup0 });

	PickupGameObject* pickup1 = new BoostPickupGameObject(material, m_boostMesh, Vec3Physics(150, 2.0, -50));
	m_pickups.insert({ pickup1->GetBroadphaseProxyId(), pickup1 });

	PickupGameObject* pickup2 = new BoostPickupGameObject(material, m_boostMesh, Vec3Physics(50, 2.0, 0));
	m_pickups.insert({ pickup2->GetBroadphaseProxyId(), pickup2 });

	PickupGameObject* pickup3 = new BoostPickupGameObject(material, m_boostMesh, Vec3Physics(-50, 2.0, 0));
	m_pickups.insert({ pickup3->GetBroadphaseProxyId(), pickup3 });

	PickupGameObject* pickup4 = new BoostPickupGameObject(material, m_boostMesh, Vec3Physics(-150, 2.0, 50));
	m_pickups.insert({ pickup4->GetBroadphaseProxyId(), pickup4 });

	PickupGameObject* pickup5 = new BoostPickupGameObject(material, m_boostMesh, Vec3Physics(-150, 2.0, -50));
	m_pickups.insert({ pickup5->GetBroadphaseProxyId(), pickup5 });



	/*pickup0->GetRenderComponent()->disabled = true;

	btVector3 newOrigin = btVector3(0.0, 0.0, 0.0);
	btTransform newWorldTrans;
	newWorldTrans.setOrigin(newOrigin);
	pickup->GetPhysicsComponent()->GetPhysicsBody()->setWorldTransform(newWorldTrans);*/
}


PickupManager::~PickupManager()
{
	delete m_boostMesh;
}

const std::unordered_map<int, PickupGameObject*>* PickupManager::GetPickups()
{
	return &m_pickups;
}

PickupGameObject* PickupManager::FindPickup(int uid)
{
	return m_pickups[uid];
}