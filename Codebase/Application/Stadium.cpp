#include "Stadium.h"
#include <vector>
#include "Rendering\Renderer.h"

#define SCALE 1.5f
const float WALL_HEIGHT = 150;
const float HALF_GOAL_WIDTH = 40;

Stadium::Stadium(Material* material, Material* wallMaterial, ExtendedMaterial* redPostMaterial, ExtendedMaterial* bluePostMaterial, const std::string& name /*= ""*/) :
GameObject(name),
m_wallMaterial(wallMaterial),
m_redPostMaterial(redPostMaterial),
m_bluePostMaterial(bluePostMaterial)
{
	m_stadiumMesh = ModelLoader::LoadMGL(MODEL_DIR"Stadium/Stadium.mgl", true);

	RigidPhysicsObject* floorPhysics = new RigidPhysicsObject();
	floorPhysics->CreateCollisionShape(0, Vec3Physics(0, 1, 0), true);
	floorPhysics->CreatePhysicsBody(0, Vec3Physics(0, -1, 0), QuatPhysics(0, 0, 0, 1));

	this->SetRenderComponent(new RenderComponent(material, m_stadiumMesh));
	this->SetPhysicsComponent(floorPhysics);
	this->GetPhysicsComponent()->GetPhysicsBody()->setRestitution(0.3);
	this->SetLocalTransform(Mat4Graphics::Scale(Vec3Physics(SCALE, SCALE, SCALE)) * Mat4Graphics::Translation(Vec3Graphics(6.8, -28.5, 2)) * Mat4Graphics::RotationX(-0.7f) * Mat4Graphics::RotationY(30.5f));
	this->GetPhysicsComponent()->GetPhysicsBody()->setFriction(1.3);
	this->GetPhysicsComponent()->GetPhysicsBody()->setHitFraction(0.7);


	m_netTexture = Texture::Get(TEXTURE_DIR"link2.png");
	m_netTexture->SetTextureParams(TextureFlags::REPEATING | TextureFlags::ANISOTROPIC_FILTERING);
	
	m_wallMaterial->Set("diffuseTex", m_netTexture);

	m_postTexture = Texture::Get(TEXTURE_DIR"wood.png");
	m_postTexture->SetTextureParams(TextureFlags::REPEATING | TextureFlags::ANISOTROPIC_FILTERING);

	static_cast<Material*>(m_redPostMaterial)->Set("diffuseTex", m_postTexture);
	m_redPostMaterial->Set("colour", Vec4Graphics(1.0f, 0.2f, 0.2f, 1));

	static_cast<Material*>(m_bluePostMaterial)->Set("diffuseTex", m_postTexture);
	m_bluePostMaterial->Set("colour", Vec4Graphics(0.2f, 0.2f, 1.0f, 1));

	m_postMesh = ModelLoader::LoadMGL(MODEL_DIR"Common/cube.mgl", true);

	CreateCollisionWalls();
}

Stadium::~Stadium()
{
	if (m_stadiumMesh)
	{
		delete m_stadiumMesh;
		m_stadiumMesh = nullptr;
	}
	if (m_postMesh)
	{
		delete m_postMesh;
		m_postMesh = nullptr;
	}

	for (auto& wall : m_walls)
	{
		if (wall)
		{
			delete wall;
			wall = nullptr;
		}
	}
	m_walls.clear();
}

void Stadium::CreateCollisionWalls()
{

	const float GOAL_HEIGHT = 35;

	// FRONT HALF
	CreatePlane(m_collisionWalls, Vec3Physics(250, 0, -HALF_GOAL_WIDTH), Vec3Physics(250, WALL_HEIGHT, -65), true);
	CreatePlane(m_collisionWalls, Vec3Physics(250, 0, -65), Vec3Physics(205, WALL_HEIGHT, -90));
	CreatePlane(m_collisionWalls, Vec3Physics(205, 0, -90), Vec3Physics(125, WALL_HEIGHT, -120));
	CreatePlane(m_collisionWalls, Vec3Physics(125, 0, -120), Vec3Physics(75, WALL_HEIGHT, -125));
	CreatePlane(m_collisionWalls, Vec3Physics(75, 0, -125), Vec3Physics(0, WALL_HEIGHT, -128));

	CreatePlane(m_collisionWalls, Vec3Physics(250, 0, HALF_GOAL_WIDTH), Vec3Physics(250, WALL_HEIGHT, 65), true);
	CreatePlane(m_collisionWalls, Vec3Physics(250, 0, 65), Vec3Physics(205, WALL_HEIGHT, 90));
	CreatePlane(m_collisionWalls, Vec3Physics(205, 0, 90), Vec3Physics(125, WALL_HEIGHT, 120));
	CreatePlane(m_collisionWalls, Vec3Physics(125, 0, 120), Vec3Physics(75, WALL_HEIGHT, 125));
	CreatePlane(m_collisionWalls, Vec3Physics(75, 0, 125), Vec3Physics(0, WALL_HEIGHT, 128));

	CreatePlane(m_collisionWalls, Vec3Physics(280, 0, -HALF_GOAL_WIDTH), Vec3Physics(280, GOAL_HEIGHT, HALF_GOAL_WIDTH));
	CreatePlane(m_collisionWalls, Vec3Physics(250, 0, -HALF_GOAL_WIDTH), Vec3Physics(280, GOAL_HEIGHT, -HALF_GOAL_WIDTH));
	CreatePlane(m_collisionWalls, Vec3Physics(250, 0, HALF_GOAL_WIDTH), Vec3Physics(280, GOAL_HEIGHT, HALF_GOAL_WIDTH));
	CreatePlane(m_collisionWalls, Vec3Physics(250, GOAL_HEIGHT, HALF_GOAL_WIDTH), Vec3Physics(280, GOAL_HEIGHT, -HALF_GOAL_WIDTH), false, true);
	CreatePlane(m_collisionWalls, Vec3Physics(250, GOAL_HEIGHT, HALF_GOAL_WIDTH), Vec3Physics(250, WALL_HEIGHT, -HALF_GOAL_WIDTH));


	// BACK HALF
	CreatePlane(m_collisionWalls, Vec3Physics(-250, 0, -HALF_GOAL_WIDTH), Vec3Physics(-250, WALL_HEIGHT, -65), true);
	CreatePlane(m_collisionWalls, Vec3Physics(-250, 0, -65), Vec3Physics(-205, WALL_HEIGHT, -90));
	CreatePlane(m_collisionWalls, Vec3Physics(-205, 0, -90), Vec3Physics(-125, WALL_HEIGHT, -120));
	CreatePlane(m_collisionWalls, Vec3Physics(-125, 0, -120), Vec3Physics(-75, WALL_HEIGHT, -125));
	CreatePlane(m_collisionWalls, Vec3Physics(-75, 0, -125), Vec3Physics(0, WALL_HEIGHT, -128));

	CreatePlane(m_collisionWalls, Vec3Physics(-250, 0, HALF_GOAL_WIDTH), Vec3Physics(-250, WALL_HEIGHT, 65), true);
	CreatePlane(m_collisionWalls, Vec3Physics(-250, 0, 65), Vec3Physics(-205, WALL_HEIGHT, 90));
	CreatePlane(m_collisionWalls, Vec3Physics(-205, 0, 90), Vec3Physics(-125, WALL_HEIGHT, 120));
	CreatePlane(m_collisionWalls, Vec3Physics(-125, 0, 120), Vec3Physics(-75, WALL_HEIGHT, 125));
	CreatePlane(m_collisionWalls, Vec3Physics(-75, 0, 125), Vec3Physics(0, WALL_HEIGHT, 128));

	CreatePlane(m_collisionWalls, Vec3Physics(-280, 0, -HALF_GOAL_WIDTH), Vec3Physics(-280, GOAL_HEIGHT, HALF_GOAL_WIDTH));
	CreatePlane(m_collisionWalls, Vec3Physics(-250, 0, -HALF_GOAL_WIDTH), Vec3Physics(-280, GOAL_HEIGHT, -HALF_GOAL_WIDTH));
	CreatePlane(m_collisionWalls, Vec3Physics(-250, 0, HALF_GOAL_WIDTH), Vec3Physics(-280, GOAL_HEIGHT, HALF_GOAL_WIDTH));
	CreatePlane(m_collisionWalls, Vec3Physics(-250, GOAL_HEIGHT, HALF_GOAL_WIDTH), Vec3Physics(-280, GOAL_HEIGHT, -HALF_GOAL_WIDTH), false, true);
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

void Stadium::CreatePlane(std::vector<btConvexHullShape*> &collectionVector, Vec3Physics start, Vec3Physics end, bool first /* = false */, bool roof /* = false */)
{

	start *= SCALE;
	end *= SCALE;

	Vec3Graphics* points = new Vec3Graphics[4];

	float lengthOfWallX;
	float lengthOfWallY;

	if (!roof) {
		points[0] = Vec3Graphics(start.x, start.y, start.z);
		points[1] = Vec3Graphics(start.x, end.y, start.z);
		points[2] = Vec3Graphics(end.x, end.y, end.z);
		points[3] = Vec3Graphics(end.x, start.y, end.z);

		lengthOfWallX = ((start * Vec3Graphics(1, 0, 1)) - (end * Vec3Graphics(1, 0, 1))).Length();
		lengthOfWallY = ((start * Vec3Graphics(0, 1, 0)) - (end * Vec3Graphics(0, 1, 0))).Length();
	}
	else {
		points[0] = Vec3Graphics(start.x, start.y, start.z);
		points[1] = Vec3Graphics(start.x, start.y, end.z);
		points[2] = Vec3Graphics(end.x, end.y, end.z);
		points[3] = Vec3Graphics(end.x, end.y, start.z);

		lengthOfWallX = ((start * Vec3Graphics(1, 0, 0)) - (end * Vec3Graphics(1, 0, 0))).Length();
		lengthOfWallY = ((start * Vec3Graphics(0, 0, 1)) - (end * Vec3Graphics(0, 0, 1))).Length();
	}

	float texLengthX = lengthOfWallX / 4;
	float texLengthY = lengthOfWallY / 4;

	Mesh* mesh = Mesh::GenerateQuad(points, Vec2Graphics(texLengthX, texLengthY));
	m_walls.push_back(mesh);

	GameObject* wall = new GameObject();
	wall->SetRenderComponent(new RenderComponent(m_wallMaterial, mesh));
	wall->SetLocalTransform(Mat4Graphics::Inverse(Mat4Graphics::Scale(Vec3Physics(SCALE, SCALE, SCALE)) * Mat4Graphics::Translation(Vec3Graphics(6.8, -28.5, 2)) * Mat4Graphics::RotationX(-0.7f) * Mat4Graphics::RotationY(30.5f)));
	this->AddChildObject(wall);

	btConvexHullShape* newShape = new btConvexHullShape();

	if (!roof) {
		newShape->addPoint(btVector3(start.x, start.y, start.z));
		newShape->addPoint(btVector3(start.x, end.y, start.z));
		newShape->addPoint(btVector3(end.x, end.y, end.z));
		newShape->addPoint(btVector3(end.x, start.y, end.z));
	}
	else {
		newShape->addPoint(btVector3(start.x, start.y, start.z));
		newShape->addPoint(btVector3(start.x, start.y, end.z));
		newShape->addPoint(btVector3(end.x, end.y, end.z));
		newShape->addPoint(btVector3(end.x, end.y, start.z));
	}

	m_collisionWalls.push_back(newShape);

	GameObject* post = new GameObject();
	
	ExtendedMaterial* material = (start.x < 0) ? m_bluePostMaterial : m_redPostMaterial;
	if (start.x > -150 && start.z < 0)
		material = m_redPostMaterial;

	post->SetRenderComponent(new RenderComponent(material, m_postMesh));
	post->SetLocalTransform(Mat4Graphics::Scale(Vec3Graphics(1.0f, abs(end.y - start.y), 1.0f)) * Mat4Graphics::Translation(Vec3Graphics(end.x, 0.0f, end.z)));
	wall->AddChildObject(post);

	if (first) {
		post = new GameObject();
		post->SetRenderComponent(new RenderComponent(material, m_postMesh));
		post->SetLocalTransform(Mat4Graphics::Scale(Vec3Graphics(1.0f, abs(end.y - start.y), 1.0f)) * Mat4Graphics::Translation(Vec3Graphics(start.x, 0.0f, start.z)));
		wall->AddChildObject(post);
	}

	if (roof) {
		post = new GameObject();
		post->SetRenderComponent(new RenderComponent(material, m_postMesh));
		post->SetLocalTransform(Mat4Graphics::Scale(Vec3Graphics(1.0f, 1.0f, HALF_GOAL_WIDTH * SCALE)) * Mat4Graphics::Translation(Vec3Graphics(start.x, start.y, 0.0f)));
		wall->AddChildObject(post);

		post = new GameObject();
		post->SetRenderComponent(new RenderComponent(material, m_postMesh));
		post->SetLocalTransform(Mat4Graphics::Scale(Vec3Graphics(1.0f, 1.0f, HALF_GOAL_WIDTH * SCALE)) * Mat4Graphics::Translation(Vec3Graphics(end.x, end.y, 0.0f)));
		wall->AddChildObject(post);

		post = new GameObject();
		post->SetRenderComponent(new RenderComponent(material, m_postMesh));
		post->SetLocalTransform(Mat4Graphics::Translation(Vec3Graphics((start.x + end.x) / 2.0f, end.y, end.z)) * Mat4Graphics::Scale(Vec3Graphics(abs(end.x - start.x) / 2.0f, 1.0f, 1.0f)));
		wall->AddChildObject(post);

		post = new GameObject();
		post->SetRenderComponent(new RenderComponent(material, m_postMesh));
		post->SetLocalTransform(Mat4Graphics::Translation(Vec3Graphics((start.x + end.x) / 2.0f, start.y, start.z)) * Mat4Graphics::Scale(Vec3Graphics(abs(end.x - start.x) / 2.0f, 1.0f, 1.0f)));
		wall->AddChildObject(post);
	}


}
