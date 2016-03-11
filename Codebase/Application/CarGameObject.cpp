#include "CarGameObject.h"
#include "BoostPickupGameObject.h"


CarGameObject::CarGameObject(const Vec3Physics& position, const QuatPhysics& rotation, Material* material, unsigned int team, const std::string& name /*= ""*/, unsigned int collisionMask /*= COL_CAR*/) :
	GameObject(name),
	m_team(team)
{

	if (m_team == BLUE_TEAM) {
		material->Set("diffuseTex", Texture::Get(MODEL_DIR"car/body1.bmp", true));
	}
	else {
		material->Set("diffuseTex", Texture::Get(MODEL_DIR"car/body.bmp", true));
	}

	Mesh* carBaseMesh = ModelLoader::LoadMGL(MODEL_DIR"Car/car_base.mgl", true);
	Mesh* wheelMesh = ModelLoader::LoadMGL(MODEL_DIR"Car/wheel.mgl", true);

	RigidPhysicsObject* carPhysics = new RigidPhysicsObject();
	carPhysics->CreateCollisionShape(Vec3Physics(5.2, 2.0, 6.5), CUBOID);
	carPhysics->CreatePhysicsBody(100000.0, position, rotation);
	carPhysics->GetPhysicsBody()->setRestitution(btScalar(0.3));
	carPhysics->GetPhysicsBody()->setFriction(0.5);
	carPhysics->GetPhysicsBody()->setRollingFriction(0.7);
	carPhysics->GetPhysicsBody()->setHitFraction(0.2);
	carPhysics->GetPhysicsBody()->getBroadphaseProxy()->m_collisionFilterMask = collisionMask;
	carPhysics->GetPhysicsBody()->getBroadphaseProxy()->m_collisionFilterGroup = COL_GROUP_DEFAULT;
	carPhysics->GetPhysicsBody()->setDamping(0.03, 0.5);

	m_spawnPoint = position;

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

	PhysicsEngineInstance::GetFilter().steps.push_back(new BoostPickupCollisionFilterStep(this));
}


CarGameObject::~CarGameObject()
{
}
