#include "CarGameObject.h"


CarGameObject::CarGameObject(Vec3Physics& position, QuatPhysics& rotation, Material* material, const std::string& name /* = "" */) :
	GameObject(name)
{
	RigidPhysicsObject* carPhysics = new RigidPhysicsObject();
	carPhysics->CreateCollisionShape(Vec3Physics(5.2, 2.0, 6.5), CUBOID);
	carPhysics->CreatePhysicsBody(8.0, position, rotation, Vec3Physics(1, 1, 1));
	carPhysics->GetPhysicsBody()->setRestitution(btScalar(0.9));
	carPhysics->GetPhysicsBody()->setFriction(0.5);
	carPhysics->GetPhysicsBody()->setRollingFriction(0.5);
	carPhysics->GetPhysicsBody()->setHitFraction(0.5);

	this->SetRenderComponent(new RenderComponent(material, ModelLoader::LoadMGL(MODEL_DIR"Car/car1.mgl", true)));
	this->SetLocalTransform(Mat4Graphics::Scale(Vector3Simple(10, 10, 10)) * Mat4Graphics::Translation(Vector3Simple(0, -0.25f, 0)));
	this->SetPhysicsComponent(carPhysics);
}


CarGameObject::~CarGameObject()
{
}
