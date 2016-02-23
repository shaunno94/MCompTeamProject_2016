#include "Stadium.h"
#include <vector>

Stadium::Stadium(Material* material, const std::string& name /*= ""*/) :
GameObject(name)
{

	RigidPhysicsObject* floorPhysics = new RigidPhysicsObject();
	floorPhysics->CreateCollisionShape(0, Vec3Physics(0, 1, 0), true);
	floorPhysics->CreatePhysicsBody(0, Vec3Physics(0, -1, 0), QuatPhysics(0, 0, 0, 1));

	this->SetRenderComponent(new RenderComponent(material, ModelLoader::LoadMGL(MODEL_DIR"Stadium/Stadium2.mgl", true)));
	this->SetPhysicsComponent(floorPhysics);
	this->GetPhysicsComponent()->GetPhysicsBody()->setRestitution(0.5);
	this->SetLocalTransform(Mat4Graphics::Translation(Vec3Graphics(6.8, -28.5, 2)) * Mat4Graphics::RotationX(-0.7f) * Mat4Graphics::RotationY(30.5f));
	this->GetPhysicsComponent()->GetPhysicsBody()->setFriction(0.5);
	this->GetPhysicsComponent()->GetPhysicsBody()->setHitFraction(0.5);

	CreateCollisionWalls();
}

Stadium::~Stadium()
{
}

void Stadium::CreateCollisionWalls()
{

	// LEFT HALF
	CreateWall(Vec2Physics(335, 0), Vec2Physics(300, -60));
	CreateWall(Vec2Physics(300, -60), Vec2Physics(250, -90));
	CreateWall(Vec2Physics(250, -90), Vec2Physics(125, -120));
	CreateWall(Vec2Physics(125, -120), Vec2Physics(75, -125));
	CreateWall(Vec2Physics(75, -125), Vec2Physics(0, -128));
	CreateWall(Vec2Physics(-335, 0), Vec2Physics(-300, -60));
	CreateWall(Vec2Physics(-300, -60), Vec2Physics(-250, -90));
	CreateWall(Vec2Physics(-250, -90), Vec2Physics(-125, -120));
	CreateWall(Vec2Physics(-125, -120), Vec2Physics(-75, -125));
	CreateWall(Vec2Physics(-75, -125), Vec2Physics(0, -128));

	// RIGHT HALF
	CreateWall(Vec2Physics(335, 0), Vec2Physics(300, 60));
	CreateWall(Vec2Physics(300, 60), Vec2Physics(250, 90));
	CreateWall(Vec2Physics(250, 90), Vec2Physics(125, 120));
	CreateWall(Vec2Physics(125, 120), Vec2Physics(75, 125));
	CreateWall(Vec2Physics(75, 125), Vec2Physics(0, 128));
	CreateWall(Vec2Physics(-335, 0), Vec2Physics(-300, 60));
	CreateWall(Vec2Physics(-300, 60), Vec2Physics(-250, 90));
	CreateWall(Vec2Physics(-250, 90), Vec2Physics(-125, 120));
	CreateWall(Vec2Physics(-125, 120), Vec2Physics(-75, 125));
	CreateWall(Vec2Physics(-75, 125), Vec2Physics(0, 128));



	RigidPhysicsObject* wallPhysics = new RigidPhysicsObject();
	wallPhysics->CreateCollisionShape(m_collisionWalls);
	wallPhysics->CreatePhysicsBody(0, Vec3Physics(0, 0, 0), QuatPhysics(0, 0, 0, 1));

}

void Stadium::CreateWall(Vec2Physics start, Vec2Physics end)
{
	const float WALL_HEIGHT = 100;

	btConvexHullShape* newShape = new btConvexHullShape();

	newShape->addPoint(btVector3(start.x, 0, start.y));
	newShape->addPoint(btVector3(start.x, WALL_HEIGHT, start.y));
	newShape->addPoint(btVector3(end.x, WALL_HEIGHT, end.y));
	newShape->addPoint(btVector3(end.x, 0, end.y));

	m_collisionWalls.push_back(newShape);
}
