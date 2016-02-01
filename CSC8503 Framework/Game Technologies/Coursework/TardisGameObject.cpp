#include "TardisGameObject.h"

#include "nclgl\OBJMesh.h"
#include "ncltech\CuboidCollisionShape.h"
#include "ncltech\PositioningState.h"
#include "ScoreManager.h"
#include "ncltech\PhysicsEngine.h"
#include "Helpers\RNG.h"

TardisGameObject::TardisGameObject(void) : SimpleMeshObject("Tardis") {
	SetMesh(new OBJMesh(MESHDIR"TARDIS\\TARDIS.obj"), true);
	SetLocalTransform(Mat4Physics::Translation(Vec3Physics(0.0f, -3.0f, 0.0f)));
	IsBoundingSphere(false);
	SetBoundingHalfVolume(Vec3Physics(1.0f, 2.0f, 1.0f));
	m_Alpha = 0.0f;
	m_Accumulator = 0.0f;
	m_MaxAlpha = RNG32::Rand(1.2f, 4.0f);
	m_Period = RNG32::Rand(10.0f, 16.0f);
	Physics()->SetCollisionShape(new CuboidCollisionShape(Vec3Physics(1.5f, 3.0f, 1.5f)));

	StateMachine* tardisFST = new StateMachine();
	tardisFST->AddState(new PositioningState(*this, Vec3Physics(0.0f, 0.0f, 0.0f), QuatPhysics(0.0f, 0.003f, 0.0f, 1.0f)), std::vector<std::pair<StateTransition*, unsigned int>>());
	SetStateMachine(tardisFST);

	TardisGameObject* tardis = this;
	std::function<bool(PhysicsObject*)> m_CollisionCallback = [ = ](PhysicsObject * colliding_obj) {
		if (colliding_obj->GetParent()->GetName().find("Projectile") !=  string::npos)
			ScoreManager::Instance()->AddScoringObject(colliding_obj->GetParent(), tardis);
		return m_Colour.w >= 1.0f;
	};
	Physics()->SetOnCollisionCallback(m_CollisionCallback);
}


TardisGameObject::~TardisGameObject(void) {
}

void TardisGameObject::OnUpdateObject(float dt) {
	float step = 2.0f * m_MaxAlpha / m_Period;
	m_Accumulator += dt;
	if (m_Accumulator > m_Period * 0.5f) {
		m_Alpha -= step * dt;
		if (m_Accumulator >= m_Period) {
			m_Accumulator = 0.0f;
			m_MaxAlpha = RNG32::Rand(1.2f, 4.0f);
			m_Period = RNG32::Rand(10.0f, 16.0f);
			m_Alpha = 0.0f;
		}
	} else
		m_Alpha += step * dt;

	m_Colour.w = Clamp(m_Alpha, 0.0f, 1.0f);

	ScoreManager::Instance()->Update(dt);
	GameObject::OnUpdateObject(dt);
};