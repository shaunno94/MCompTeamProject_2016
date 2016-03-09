#include "CarGameObject.h"


CarGameObject::CarGameObject(const Vec3Physics& position, const QuatPhysics& rotation, Material* material, const std::string name /* = "" */, unsigned int collisionFilterMask /* = COL_CAR */) :
	GameObject(name)
{
	Mesh* carBaseMesh = ModelLoader::LoadMGL(MODEL_DIR"Car/car_base.mgl", true);
	Mesh* wheelMesh = ModelLoader::LoadMGL(MODEL_DIR"Car/wheel.mgl", true);

	RigidPhysicsObject* carPhysics = new RigidPhysicsObject();
	carPhysics->CreateCollisionShape(Vec3Physics(5.2, 2.0, 6.5), CUBOID);
	carPhysics->CreatePhysicsBody(100000.0, position, rotation);
	carPhysics->GetPhysicsBody()->setRestitution(btScalar(0.3));
	carPhysics->GetPhysicsBody()->setFriction(0.5);
	carPhysics->GetPhysicsBody()->setRollingFriction(0.7);
	carPhysics->GetPhysicsBody()->setHitFraction(0.2);
	carPhysics->GetPhysicsBody()->getBroadphaseProxy()->m_collisionFilterMask = collisionFilterMask;
	carPhysics->GetPhysicsBody()->setDamping(0.3, 0.5);

	this->SetRenderComponent(new RenderComponent(material, carBaseMesh));
	this->SetLocalTransform(Mat4Graphics::Scale(Vector3Simple(10, 10, 10)) * Mat4Graphics::Translation(Vector3Simple(0, -0.25f, 0)));
	this->SetPhysicsComponent(carPhysics);

	WheelObject* wheel = new WheelObject("fl");
	wheel->SetRenderComponent(new RenderComponent(material, wheelMesh));
	wheel->SetLocalTransform(Mat4Graphics::Translation(Vec3Graphics(-0.45, 0.17, -0.45))) /*Mat4Graphics::Scale(Vector3Simple(10, 10, 10)))*/;
	this->AddChildObject(wheel);

	wheel = new WheelObject("bl");
	wheel->SetRenderComponent(new RenderComponent(material, wheelMesh));
	wheel->SetLocalTransform(Mat4Graphics::Translation(Vec3Graphics(-0.45, 0.17, 0.45))) /*Mat4Graphics::Scale(Vector3Simple(10, 10, 10)))*/;
	this->AddChildObject(wheel);

	wheel = new WheelObject("fr");
	wheel->SetRenderComponent(new RenderComponent(material, wheelMesh));
	wheel->SetLocalTransform(Mat4Graphics::Translation(Vec3Graphics(0.45, 0.17, -0.45))* Mat4Graphics::Scale(Vector3Simple(-1, -1, -1))) /*Mat4Graphics::Scale(Vector3Simple(10, 10, 10)))*/;
	this->AddChildObject(wheel);

	wheel = new WheelObject("br");
	wheel->SetRenderComponent(new RenderComponent(material, wheelMesh));
	wheel->SetLocalTransform(Mat4Graphics::Translation(Vec3Graphics(0.45, 0.17, 0.45))* Mat4Graphics::Scale(Vector3Simple(-1, -1, -1))) /*Mat4Graphics::Scale(Vector3Simple(10, 10, 10)))*/;
	this->AddChildObject(wheel);
}


CarGameObject::~CarGameObject()
{
}
