#include "Stadium.h"
#include <vector>

Stadium::Stadium(Material* material, Material* wallMaterial, const std::string& name /*= ""*/) :
GameObject(name),
m_wallMaterial(wallMaterial)
{

	RigidPhysicsObject* floorPhysics = new RigidPhysicsObject();
	floorPhysics->CreateCollisionShape(0, Vec3Physics(0, 1, 0), true);
	floorPhysics->CreatePhysicsBody(0, Vec3Physics(0, -1, 0), QuatPhysics(0, 0, 0, 1));

	this->SetRenderComponent(new RenderComponent(material, ModelLoader::LoadMGL(MODEL_DIR"Stadium/Stadium.mgl", true)));
	this->SetPhysicsComponent(floorPhysics);
	this->GetPhysicsComponent()->GetPhysicsBody()->setRestitution(0.5);
	this->SetLocalTransform(Mat4Graphics::Translation(Vec3Graphics(6.8, -28.5, 2)) * Mat4Graphics::RotationX(-0.7f) * Mat4Graphics::RotationY(30.5f));
	this->GetPhysicsComponent()->GetPhysicsBody()->setFriction(0.5);
	this->GetPhysicsComponent()->GetPhysicsBody()->setHitFraction(0.5);

	
#ifdef ORBIS
	m_netTexture = Texture::Get(TEXTURE_DIR"link2.gnf");
#else
	m_netTexture = Texture::Get(TEXTURE_DIR"link2.png");
#endif

	m_netTexture->SetTextureParams(TextureFlags::REPEATING | TextureFlags::ANISOTROPIC_FILTERING);
	m_wallMaterial->Set("diffuseTex", m_netTexture);

	CreateCollisionWalls();

}

Stadium::~Stadium()
{
}

void Stadium::CreateCollisionWalls()
{

	const float WALL_HEIGHT = 100;

	// FRONT HALF
	CreatePlane(m_collisionWalls, Vec3Physics(250, 0, -90), Vec3Physics(125, WALL_HEIGHT, -120));
	CreatePlane(m_collisionWalls, Vec3Physics(125, 0, -120), Vec3Physics(75, WALL_HEIGHT, -125));
	CreatePlane(m_collisionWalls, Vec3Physics(75, 0, -125), Vec3Physics(0, WALL_HEIGHT, -128));
	CreatePlane(m_collisionWalls, Vec3Physics(250, 0, 90), Vec3Physics(250, WALL_HEIGHT, 30));
	CreatePlane(m_collisionWalls, Vec3Physics(250, 0, -30), Vec3Physics(250, WALL_HEIGHT, -90));
	CreatePlane(m_collisionWalls, Vec3Physics(250, 0, 90), Vec3Physics(125, WALL_HEIGHT, 120));
	CreatePlane(m_collisionWalls, Vec3Physics(125, 0, 120), Vec3Physics(75, WALL_HEIGHT, 125));
	CreatePlane(m_collisionWalls, Vec3Physics(75, 0, 125), Vec3Physics(0, WALL_HEIGHT, 128));

	CreatePlane(m_collisionWalls, Vec3Physics(280, 0, -30), Vec3Physics(280, WALL_HEIGHT / 3, 30));
	CreatePlane(m_collisionWalls, Vec3Physics(250, 0, -30), Vec3Physics(280, WALL_HEIGHT / 3, -30));
	CreatePlane(m_collisionWalls, Vec3Physics(250, 0, 30), Vec3Physics(280, WALL_HEIGHT / 3, 30));
	CreatePlane(m_collisionWalls, Vec3Physics(250, WALL_HEIGHT / 3, 30), Vec3Physics(280, WALL_HEIGHT / 3, -30));
	CreatePlane(m_collisionWalls, Vec3Physics(250, WALL_HEIGHT / 3, 30), Vec3Physics(250, WALL_HEIGHT, -30));

	// BACK HALF
	CreatePlane(m_collisionWalls, Vec3Physics(-250, 0, -90), Vec3Physics(-125, WALL_HEIGHT, -120));
	CreatePlane(m_collisionWalls, Vec3Physics(-125, 0, -120), Vec3Physics(-75, WALL_HEIGHT, -125));
	CreatePlane(m_collisionWalls, Vec3Physics(-75, 0, -125), Vec3Physics(0, WALL_HEIGHT, -128));
	CreatePlane(m_collisionWalls, Vec3Physics(-250, 0, 90), Vec3Physics(-250, WALL_HEIGHT, 30));
	CreatePlane(m_collisionWalls, Vec3Physics(-250, 0, -30), Vec3Physics(-250, WALL_HEIGHT, -90));
	CreatePlane(m_collisionWalls, Vec3Physics(-250, 0, 90), Vec3Physics(-125, WALL_HEIGHT, 120));
	CreatePlane(m_collisionWalls, Vec3Physics(-125, 0, 120), Vec3Physics(-75, WALL_HEIGHT, 125));
	CreatePlane(m_collisionWalls, Vec3Physics(-75, 0, 125), Vec3Physics(0, WALL_HEIGHT, 128));

	CreatePlane(m_collisionWalls, Vec3Physics(-280, 0, -30), Vec3Physics(-280, WALL_HEIGHT / 3, 30));
	CreatePlane(m_collisionWalls, Vec3Physics(-250, 0, -30), Vec3Physics(-280, WALL_HEIGHT / 3, -30));
	CreatePlane(m_collisionWalls, Vec3Physics(-250, 0, 30), Vec3Physics(-280, WALL_HEIGHT / 3, 30));
	CreatePlane(m_collisionWalls, Vec3Physics(-250, WALL_HEIGHT / 3, 30), Vec3Physics(-280, WALL_HEIGHT / 3, -30));
	CreatePlane(m_collisionWalls, Vec3Physics(-250, WALL_HEIGHT / 3, 30), Vec3Physics(-250, WALL_HEIGHT, -30));



	RigidPhysicsObject* wallPhysics = new RigidPhysicsObject();
	wallPhysics->CreateCollisionShape(m_collisionWalls);
	wallPhysics->CreatePhysicsBody(0, Vec3Physics(0, 0, 0), QuatPhysics(0, 0, 0, 1));

	
	RigidPhysicsObject* goalPhysics = new RigidPhysicsObject();

}

void Stadium::CreatePlane(std::vector<btConvexHullShape*> &collectionVector, Vec3Physics start, Vec3Physics end)
{

	Vec3Graphics* points = new Vec3Graphics[4];
	points[0] = Vec3Graphics(start.x, start.y, start.z);
	points[1] = Vec3Graphics(start.x, end.y, start.z);
	points[2] = Vec3Graphics(end.x, end.y, end.z);
	points[3] = Vec3Graphics(end.x, start.y, end.z);

	float texLength = (start - end).Length() / 5;

	Mesh* mesh = Mesh::GenerateQuad(points, Vec2Graphics(texLength, texLength));

	GameObject* wall = new GameObject();
	wall->SetRenderComponent(new RenderComponent(m_wallMaterial, mesh));
	//wall->SetWorldTransform(Mat4Graphics::Translation(Vector3Simple(start.x, start.y, start.z)) * Mat4Graphics::Scale(difference));
	this->AddChildObject(wall);



	btConvexHullShape* newShape = new btConvexHullShape();

	newShape->addPoint(btVector3(start.x, start.y, start.z));
	newShape->addPoint(btVector3(start.x, end.y, start.z));
	newShape->addPoint(btVector3(end.x, end.y, end.z));
	newShape->addPoint(btVector3(end.x, start.y, end.z));

	m_collisionWalls.push_back(newShape);
}
