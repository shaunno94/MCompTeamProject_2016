#include "BallGameObject.h"
#include "Rendering/ModelLoader.h"

BallGameObject::BallGameObject(const std::string& name, Material* material) : GameObject(name)
{
	material->Set(ReservedMeshTextures.DIFFUSE.name, Texture::Get(TEXTURE_DIR"football.png", true));

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

	ballPhysics->GetPhysicsBody()->getBroadphaseProxy()->m_collisionFilterGroup = COL_GROUP_DEFAULT;
	ballPhysics->GetPhysicsBody()->getBroadphaseProxy()->m_collisionFilterMask = COL_BALL;
	SetPhysicsComponent(ballPhysics);

	m_spawnPoint = Vec3Graphics(0,0,0);
}


BallGameObject::~BallGameObject()
{
}
