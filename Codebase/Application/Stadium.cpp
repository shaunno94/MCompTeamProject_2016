#include "Stadium.h"
#include <vector>
#include "Rendering\Renderer.h"

#define SCALE 1.5f

Stadium::Stadium(Material* material, Material* wallMaterial, const std::string& name /*= ""*/) :
GameObject(name),
m_wallMaterial(wallMaterial)
{

	RigidPhysicsObject* floorPhysics = new RigidPhysicsObject();
	floorPhysics->CreateCollisionShape(0, Vec3Physics(0, 1, 0), true);
	floorPhysics->CreatePhysicsBody(0, Vec3Physics(0, -1, 0), QuatPhysics(0, 0, 0, 1));

	this->SetRenderComponent(new RenderComponent(material, ModelLoader::LoadMGL(MODEL_DIR"Stadium/Stadium.mgl", true)));
	this->SetPhysicsComponent(floorPhysics);
	this->GetPhysicsComponent()->GetPhysicsBody()->setRestitution(0.3);
	this->SetLocalTransform(Mat4Graphics::Scale(Vec3Physics(SCALE, SCALE, SCALE)) * Mat4Graphics::Translation(Vec3Graphics(6.8, -28.5, 2)) * Mat4Graphics::RotationX(-0.7f) * Mat4Graphics::RotationY(30.5f));
	this->GetPhysicsComponent()->GetPhysicsBody()->setFriction(1.3);
	this->GetPhysicsComponent()->GetPhysicsBody()->setHitFraction(0.7);
	m_netTexture = Texture::Get(TEXTURE_DIR"link2.png");
	m_netTexture->SetTextureParams(TextureFlags::REPEATING | TextureFlags::ANISOTROPIC_FILTERING);
	m_wallMaterial->Set("diffuseTex", m_netTexture);

	CreateCollisionWalls();

}

Stadium::~Stadium()
{
}

void Stadium::CreateCollisionWalls()
{

	const float WALL_HEIGHT = 150;
	const float GOAL_HEIGHT = 35;
	const float HALF_GOAL_WIDTH = 40;



	// FRONT HALF
	CreatePlane(m_collisionWalls, Vec3Physics(250, 0, -HALF_GOAL_WIDTH), Vec3Physics(240, WALL_HEIGHT, -65));
	CreatePlane(m_collisionWalls, Vec3Physics(240, 0, -65), Vec3Physics(205, WALL_HEIGHT, -90));
	CreatePlane(m_collisionWalls, Vec3Physics(205, 0, -90), Vec3Physics(125, WALL_HEIGHT, -120));
	CreatePlane(m_collisionWalls, Vec3Physics(125, 0, -120), Vec3Physics(75, WALL_HEIGHT, -125));
	CreatePlane(m_collisionWalls, Vec3Physics(75, 0, -125), Vec3Physics(0, WALL_HEIGHT, -128));

	CreatePlane(m_collisionWalls, Vec3Physics(250, 0, HALF_GOAL_WIDTH), Vec3Physics(240, WALL_HEIGHT, 65));
	CreatePlane(m_collisionWalls, Vec3Physics(240, 0, 65), Vec3Physics(205, WALL_HEIGHT, 90));
	CreatePlane(m_collisionWalls, Vec3Physics(205, 0, 90), Vec3Physics(125, WALL_HEIGHT, 120));
	CreatePlane(m_collisionWalls, Vec3Physics(125, 0, 120), Vec3Physics(75, WALL_HEIGHT, 125));
	CreatePlane(m_collisionWalls, Vec3Physics(75, 0, 125), Vec3Physics(0, WALL_HEIGHT, 128));

	CreatePlane(m_collisionWalls, Vec3Physics(280, 0, -HALF_GOAL_WIDTH), Vec3Physics(280, GOAL_HEIGHT, HALF_GOAL_WIDTH));
	CreatePlane(m_collisionWalls, Vec3Physics(250, 0, -HALF_GOAL_WIDTH), Vec3Physics(280, GOAL_HEIGHT, -HALF_GOAL_WIDTH));
	CreatePlane(m_collisionWalls, Vec3Physics(250, 0, HALF_GOAL_WIDTH), Vec3Physics(280, GOAL_HEIGHT, HALF_GOAL_WIDTH));
	CreatePlane(m_collisionWalls, Vec3Physics(250, GOAL_HEIGHT, HALF_GOAL_WIDTH), Vec3Physics(280, GOAL_HEIGHT, -HALF_GOAL_WIDTH));
	CreatePlane(m_collisionWalls, Vec3Physics(250, GOAL_HEIGHT, HALF_GOAL_WIDTH), Vec3Physics(250, WALL_HEIGHT, -HALF_GOAL_WIDTH));



	// BACK HALF
	CreatePlane(m_collisionWalls, Vec3Physics(-250, 0, -HALF_GOAL_WIDTH), Vec3Physics(-240, WALL_HEIGHT, -65));
	CreatePlane(m_collisionWalls, Vec3Physics(-240, 0, -65), Vec3Physics(-205, WALL_HEIGHT, -90));
	CreatePlane(m_collisionWalls, Vec3Physics(-205, 0, -90), Vec3Physics(-125, WALL_HEIGHT, -120));
	CreatePlane(m_collisionWalls, Vec3Physics(-125, 0, -120), Vec3Physics(-75, WALL_HEIGHT, -125));
	CreatePlane(m_collisionWalls, Vec3Physics(-75, 0, -125), Vec3Physics(0, WALL_HEIGHT, -128));

	CreatePlane(m_collisionWalls, Vec3Physics(-250, 0, HALF_GOAL_WIDTH), Vec3Physics(-240, WALL_HEIGHT, 65));
	CreatePlane(m_collisionWalls, Vec3Physics(-240, 0, 65), Vec3Physics(-205, WALL_HEIGHT, 90));
	CreatePlane(m_collisionWalls, Vec3Physics(-205, 0, 90), Vec3Physics(-125, WALL_HEIGHT, 120));
	CreatePlane(m_collisionWalls, Vec3Physics(-125, 0, 120), Vec3Physics(-75, WALL_HEIGHT, 125));
	CreatePlane(m_collisionWalls, Vec3Physics(-75, 0, 125), Vec3Physics(0, WALL_HEIGHT, 128));

	CreatePlane(m_collisionWalls, Vec3Physics(-280, 0, -HALF_GOAL_WIDTH), Vec3Physics(-280, GOAL_HEIGHT, HALF_GOAL_WIDTH));
	CreatePlane(m_collisionWalls, Vec3Physics(-250, 0, -HALF_GOAL_WIDTH), Vec3Physics(-280, GOAL_HEIGHT, -HALF_GOAL_WIDTH));
	CreatePlane(m_collisionWalls, Vec3Physics(-250, 0, HALF_GOAL_WIDTH), Vec3Physics(-280, GOAL_HEIGHT, HALF_GOAL_WIDTH));
	CreatePlane(m_collisionWalls, Vec3Physics(-250, GOAL_HEIGHT, HALF_GOAL_WIDTH), Vec3Physics(-280, GOAL_HEIGHT, -HALF_GOAL_WIDTH));
	CreatePlane(m_collisionWalls, Vec3Physics(-250, GOAL_HEIGHT, HALF_GOAL_WIDTH), Vec3Physics(-250, WALL_HEIGHT, -HALF_GOAL_WIDTH));



	RigidPhysicsObject* roofPhysics = new RigidPhysicsObject();
	roofPhysics->CreateCollisionShape(0, Vec3Physics(0, -1, 0), true);
	roofPhysics->CreatePhysicsBody(0, Vec3Physics(0, WALL_HEIGHT * SCALE, 0), QuatPhysics(0, 0, 0, 1));
	roofPhysics->GetPhysicsBody()->setRestitution(0.4f);
	roofPhysics->GetPhysicsBody()->getBroadphaseProxy()->m_collisionFilterMask = COL_WALL;

	RigidPhysicsObject* wallPhysics = new RigidPhysicsObject();
	wallPhysics->CreateCollisionShape(m_collisionWalls);
	wallPhysics->CreatePhysicsBody(0, Vec3Physics(0, 0, 0), QuatPhysics(0, 0, 0, 1));
	wallPhysics->GetPhysicsBody()->setRestitution(0.4f);
	wallPhysics->GetPhysicsBody()->getBroadphaseProxy()->m_collisionFilterMask = COL_WALL;


	

}

void Stadium::CreatePlane(std::vector<btConvexHullShape*> &collectionVector, Vec3Physics start, Vec3Physics end)
{

	start *= SCALE;
	end *= SCALE;

	Vec3Graphics* points = new Vec3Graphics[4];
	points[0] = Vec3Graphics(start.x, start.y, start.z);
	points[1] = Vec3Graphics(start.x, end.y, start.z);
	points[2] = Vec3Graphics(end.x, end.y, end.z);
	points[3] = Vec3Graphics(end.x, start.y, end.z);

	float lengthOfWallX = ((start * Vec3Graphics(1, 0, 1)) - (end * Vec3Graphics(1, 0, 1))).Length();
	float lengthOfWallY = ((start * Vec3Graphics(0, 1, 0)) - (end * Vec3Graphics(0, 1, 0))).Length();

	float texLengthX = lengthOfWallX / 4;
	float texLengthY = lengthOfWallY / 4;

	Mesh* mesh = Mesh::GenerateQuad(points, Vec2Graphics(texLengthX, texLengthY));

	GameObject* wall = new GameObject();
	wall->SetRenderComponent(new RenderComponent(m_wallMaterial, mesh));
	wall->SetLocalTransform(Mat4Graphics::Inverse(Mat4Graphics::Scale(Vec3Physics(SCALE, SCALE, SCALE)) * Mat4Graphics::Translation(Vec3Graphics(6.8, -28.5, 2)) * Mat4Graphics::RotationX(-0.7f) * Mat4Graphics::RotationY(30.5f)));
	this->AddChildObject(wall);

	btConvexHullShape* newShape = new btConvexHullShape();

	newShape->addPoint(btVector3(start.x, start.y, start.z));
	newShape->addPoint(btVector3(start.x, end.y, start.z));
	newShape->addPoint(btVector3(end.x, end.y, end.z));
	newShape->addPoint(btVector3(end.x, start.y, end.z));

	m_collisionWalls.push_back(newShape);
}
