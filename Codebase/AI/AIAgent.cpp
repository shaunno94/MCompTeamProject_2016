#include "AIAgent.h"

AIAgent::AIAgent(Material* material, GameObject* ball, const std::string& name /*= ""*/) :
GameObject(name)
{
	RigidPhysicsObject* aiBallPhysics = new RigidPhysicsObject();
	aiBallPhysics->CreateCollisionShape(2.0);
	aiBallPhysics->CreatePhysicsBody(2.0, Vec3Physics(0.11, 15, 0.5), QuatPhysics(0, 0, 0, 1), Vec3Physics(1, 1, 1));


	this->SetRenderComponent(new RenderComponent(material, ModelLoader::LoadMGL(MODEL_DIR"Common/sphere.mgl", true)));
	this->SetLocalTransform(Mat4Graphics::Scale(Vector3Simple(2, 2, 2)) * Mat4Graphics::Translation(Vector3Simple(0, 0, 0)));
	this->SetPhysicsComponent(aiBallPhysics);
	this->GetPhysicsComponent()->GetPhysicsBody()->setRestitution(btScalar(0.9));
	this->GetPhysicsComponent()->GetPhysicsBody()->setFriction(0.5);
	this->GetPhysicsComponent()->GetPhysicsBody()->setHitFraction(0.5);

	m_ball = ball;
}

AIAgent::~AIAgent()
{
}

void AIAgent::OnUpdateObject(float dt)
{
	GameObject::OnUpdateObject(dt);

	m_StateMachine->Update(dt);
}
