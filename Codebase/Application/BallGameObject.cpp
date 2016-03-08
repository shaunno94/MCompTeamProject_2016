#include "BallGameObject.h"
#include "Rendering/ModelLoader.h"

BallGameObject::BallGameObject(const std::string& name, Material* material) : GameObject(name)
{
	SetRenderComponent(new RenderComponent(material, ModelLoader::LoadMGL(MODEL_DIR"Common/sphere.mgl", true)));
	SetLocalTransform(Mat4Graphics::Scale(Vec3Graphics(6, 6, 6)));

	auto ballPhysics = new RigidPhysicsObject();
	ballPhysics->CreateCollisionShape(6.0);
	ballPhysics->CreatePhysicsBody(1.0, Vec3Physics(0, 3, 0), QuatPhysics(0, 0, 0, 1), Vec3Physics(1, 1, 1));
	auto body = ballPhysics->GetPhysicsBody();
	body->setRestitution(btScalar(1.6));
	body->setFriction(0.5);
	body->setRollingFriction(0.5);
	body->setHitFraction(0.5);
	SetPhysicsComponent(ballPhysics);
}


BallGameObject::~BallGameObject()
{
}
