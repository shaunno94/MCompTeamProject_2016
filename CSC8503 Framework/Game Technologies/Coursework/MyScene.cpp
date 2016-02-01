#include "MyScene.h"
#include "Player.h"

#include <nclgl/OBJMesh.h>

#include <ncltech\SimpleMeshObject.h>
#include <ncltech\SphereCollisionShape.h>
#include <ncltech\CuboidCollisionShape.h>
#include <ncltech\PhysicsEngine.h>
#include <ncltech\CommonMeshes.h>
#include <ncltech\DistanceConstraint.h>
#include <ncltech\NCLDebug.h>
#include "ncltech\OctreeSpacePartition.h"
#include "ncltech\PositioningState.h"
#include "GoalkeeperGameObject.h"
#include "ScoreHistory.h"
#include "ncltech/PyramidCollisionShape.h"
#include "TestCases.h"


MyScene::MyScene(Window& window) : Scene(window)
{
	if (init == true)
		init = InitialiseGL();
	PhysicsEngine::Instance()->SetUpdateTimestep(1.0f / 120.0f);
	UpdateWorldMatrices(m_RootGameObject, Mat4Physics::IDENTITY);
	m_RenderMode = NormalRenderMode;
	m_DisplayHighScores = false;

	m_OverlayShader = new Shader(SHADERDIR"OverlayVertex.glsl", SHADERDIR"OverlayFragment.glsl");
	if (!m_OverlayShader->IsOperational())
		return;

	m_EndState = LoseGameState;
	m_ProjectileTex = SOIL_load_OGL_texture(TEXTUREDIR"rocks1.jpg", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT);
}

MyScene::~MyScene()
{
	for (GameObject* go : m_Resources)
		delete go;

	TestCases::ReleaseResources();
	delete m_OverlayShader;

	glDeleteTextures(1, &m_ProjectileTex);
}

bool MyScene::InitialiseGL()
{
	m_Camera->SetPosition(Vec3Physics(-6.25f, 2.0f, 10.0f));

	//Create Ground
	SimpleMeshObject* ground = new SimpleMeshObject("Ground");
	ground->SetMesh(CommonMeshes::Cube(), false);
	ground->SetLocalTransform(Mat4Physics::Translation(Vec3Physics(0.0, 0.0, 0.0f)) * Mat4Physics::Scale(Vec3Physics(60.0f, 0.5f, 60.0f))); //80m width, 1m height, 80m depth
	ground->SetColour(Vec4Graphics(0.2f, 1.0f, 0.5f, 1.0f));
	ground->SetBoundingRadius(80.0f * 80.f);
	ground->IsBoundingSphere(false);
	ground->SetBoundingHalfVolume(Vec3Physics(60.0f, 0.5f, 60.0f));
	ground->Physics()->SetCollisionShape(new CuboidCollisionShape(Vec3Physics(60.0f, 0.5f, 60.0f)));
	this->AddGameObject(ground);
	m_Resources.push_back(ground);

	TestCases::AddCuboidStack(this, Vec3Physics(-40.0f, 0.5f, -10.0f), 10);
	TestCases::AddStackedPyramidTestcase(this, Vec3Physics(-40.0f, 0.0f, -5.0f), true);
	TestCases::AddStackedPyramidTestcase(this, Vec3Physics(-35.0f, 0.0f, -7.0f), true);
	TestCases::AddPlaneTestcase(this, Vec3Physics(-40.0f, 0.0f, -40.0f));

	TestCases::AddWall(this, Vec3Physics(-40.0f, 0.0f, -20.0f), Vec3Physics(20.0f, 5.0f, 0.5f));
	TestCases::AddWall(this, Vec3Physics(-40.0f, 0.0f, 10.0f), Vec3Physics(20.0f, 5.0f, 0.5f));
	TestCases::AddWall(this, Vec3Physics(-40.0f, 0.0f, 40.0f), Vec3Physics(20.0f, 5.0f, 0.5f));
	TestCases::AddSpheresCuboid(this, Vec3Physics(-20.0f, 4.0f, 10.0f), Vec3Physics(5.0f, 4.0f, 5.0f));
	TestCases::AddWall(this, Vec3Physics(-20.0f, 0.0f, 25.0f), Vec3Physics(0.5f, 5.0f, 15.0f));

	TestCases::AddSimpleStackedRestingTestcase(this, Vec3Physics(-55.0f, 0.0f, 55.0f));
	TestCases::AddSimpleStackedRestingTestcase(this, Vec3Physics(-50.0f, 0.0f, 50.0f));
	TestCases::AddSimpleSwingingTestcase(this, Vec3Physics(-45.0f, 0.0f, 45.0f));

	TestCases::AddWall(this, Vec3Physics(-60.0f, 0.0f, 0.0f), Vec3Physics(0.5f, 15.0f, 60.0f));
	TestCases::AddWall(this, Vec3Physics(60.0f, 0.0f, 0.0f), Vec3Physics(0.5f, 15.0f, 60.0f));
	TestCases::AddWall(this, Vec3Physics(0.0f, 0.0f, 60.0f), Vec3Physics(60.0f, 15.0f, 0.5f));
	TestCases::AddWall(this, Vec3Physics(0.0f, 0.0f, -60.0f), Vec3Physics(60.0f, 15.0f, 0.5f));

	m_Overlay = new SimpleMeshObject("Overlay", nullptr);
	m_Overlay->SetMesh(Mesh::GenerateQuadTexCoordCol(Vec2Physics(1.f, 1.f), Vec2Physics(0.0f, 1.0f), Vec4Physics(1.0f, 1.0f, 1.0f, 1.0f)), true);
	m_Resources.push_back(m_Overlay);

	return true;
}

//Scene* MyScene::GetNextScene(Window& window) {
//	return new EndScene(window);;
//}

void MyScene::Cleanup()
{
	for (GameObject* go : m_Resources)
		go->Ditach();
}

void MyScene::UpdateScene(float sec)
{
	sceneUpdateTimer.GetTimedMS();
	std::lock_guard<std::mutex> guard(PhysicsObject::g_ExternalChanges);

	/*
	GameObject* obj = Scene::FindGameObject("Player1");
	if (obj != NULL)
	{
		Vec3Physics player_pos = obj->Physics()->GetPosition();
		NCLDebug::Log(Vec3Physics(1.0f, 0.0f, 0.0f), "Player Position - %5.2fm, %5.2fm, %5.2fm\n", player_pos.x, player_pos.y, player_pos.z);
	}
	*/
	Keyboard* keyboard = Window::GetKeyboard();

	static unsigned int projectileCounter = 0;

	if (keyboard->KeyTriggered(KEYBOARD_J)) {
		Vec3Physics position = m_Camera->GetPosition();
		Vec3Physics viewDirection = -Vec3Physics(viewMatrix.values[2], viewMatrix.values[6], viewMatrix.values[10]);
		viewDirection.Normalize();
		std::stringstream ss;
		ss << "Projectile" << projectileCounter++;
		SimpleMeshObject* projectile = nullptr;

		if (keyboard->KeyDown(KEYBOARD_1)) {
			SimpleMeshObject* cuboid = new SimpleMeshObject(ss.str());
			cuboid->SetMesh(CommonMeshes::Cube(), false);
			cuboid->SetBoundingRadius(0.867f);
			cuboid->Physics()->SetCollisionShape(new CuboidCollisionShape(Vec3Physics(0.5f, 0.5f, 0.5f)));
			projectile = cuboid;

		} else if (keyboard->KeyDown(KEYBOARD_2)) {
			SimpleMeshObject* pyramid = new SimpleMeshObject(ss.str());
			pyramid->SetMesh(CommonMeshes::Pyramid(), false);
			pyramid->SetBoundingRadius(0.867f);
			pyramid->Physics()->SetCollisionShape(new PyramidCollisionShape(Vec3Physics(0.5f, 0.5f, 0.5f)));
			projectile = pyramid;

		} else {
			SimpleMeshObject* sphere = new SimpleMeshObject(ss.str());
			sphere->SetMesh(CommonMeshes::Sphere(), false);
			sphere->SetBoundingRadius(0.50f);
			sphere->Physics()->SetCollisionShape(new SphereCollisionShape(0.5f));
			projectile = sphere;
		}

		projectile->SetLocalTransform(Mat4Physics::Scale(Vec3Physics(0.5f, 0.5f, 0.5f)));//1m radius
		projectile->Physics()->SetInverseMass(1.0f / 3.0f);
		projectile->Physics()->SetInverseInertia(projectile->Physics()->GetCollisionShape()->BuildInverseInertia(projectile->Physics()->GetInverseMass()));
		projectile->Physics()->SetPosition(position);
		projectile->Physics()->SetLinearVelocity(viewDirection * 40.0f);
		projectile->SetTexture(m_ProjectileTex, false);

		//PhysicsEngine::Instance()->AddConstraint(new DistanceConstraint(cube->Physics(), sphere->Physics(), cube->Physics()->GetPosition(), sphere->Physics()->GetPosition()));
		this->AddGameObject(projectile);
		m_Resources.push_back(projectile);
	}

	if (keyboard->KeyTriggered(KEYBOARD_M))
		m_RenderMode = (m_RenderMode + 1) % RenderModeMax;

	if (m_Goalkeeper && keyboard->KeyTriggered(KEYBOARD_K))
		m_Goalkeeper->Physics()->IsEnabled(!m_Goalkeeper->Physics()->IsEnabled());

	if (keyboard->KeyTriggered(KEYBOARD_H))
		m_DisplayHighScores = !m_DisplayHighScores;

	if (keyboard->KeyTriggered(KEYBOARD_X) || keyboard->KeyTriggered(KEYBOARD_ESCAPE)) {
		m_EndState = QuitGameState;
		keyboard->UpdateHolds();
		PhysicsEngine::Instance()->SetPaused(true);
	}

	float scoreSize = 20.0f;
	float cs_size_x = scoreSize / Window::GetWindow().GetScreenSize().x * 2.0f;
	float cs_size_y = scoreSize / Window::GetWindow().GetScreenSize().y * 2.0f;

	static const size_t buffLength = 100;
	char buff[buffLength];
	auto needed = _snprintf_s(&buff[0], sizeof(buff), buffLength, "SCORE: %u", 100);
	NCLDebug::DrawTextClipSpace(Vec4Graphics(1.0f, 1.0f - cs_size_y, 0.0f, 1.0f), scoreSize, std::string(buff, needed < 0 ? buffLength : needed), TEXTALIGN_RIGHT, Vec4Graphics::ONES);
	NCLDebug::DrawTextClipSpace(Vec4Graphics(1.0f, 1.0f - (cs_size_y * 2.0f), 0.0f, 1.0f), 16.0f, "Press H To Toggle High Score", TEXTALIGN_RIGHT, Vec4Graphics::ONES);

	if (m_DisplayHighScores) {
		unsigned int lineCounter = 2.0f;
		for (ScoreHistory::HighScoreRecord& record : ScoreHistory::Instance()->HighScores) {
			lineCounter += 1.0f;
			NCLDebug::DrawTextClipSpace(Vec4Graphics(1.0f, 1.0f - (cs_size_y * lineCounter), 0.0f, 1.0f), 16.0f, record.ToString(), TEXTALIGN_RIGHT, Vec4Graphics::ONES);
		}
	}


	if ((RenderModeMasks[m_RenderMode] & RenderModeMasks[DebugRenderMode]) != 0) {
		PhysicsEngine::Instance()->DebugRender();

		NCLDebug::AddStatusEntry(Vec4Graphics::ONES, "Physics Engine: %s (Press P to toggle)", PhysicsEngine::Instance()->IsPaused() ? "Paused" : "Enabled");
		NCLDebug::AddStatusEntry(Vec4Graphics::ONES, "--------------------------------");
		NCLDebug::AddStatusEntry(Vec4Graphics::ONES, "Physics Timestep : %5.2fms (%5.2f FPS)", PhysicsEngine::Instance()->GetUpdateTimestep() * 1000.0f, 1.0f / PhysicsEngine::Instance()->GetUpdateTimestep());
		NCLDebug::AddStatusEntry(Vec4Graphics::ONES, "Graphics Timestep: %5.2fms (%5.2f FPS)", sec * 1000.0f, 1.0f / sec);
		NCLDebug::AddStatusEntry(Vec4Graphics::ONES, "--------------------------------");

		NCLDebug::AddStatusEntry(Vec4Graphics::ONES, "Physics Update: %5.2fms", PhysicsEngine::g_LastStepTime);
		NCLDebug::AddStatusEntry(Vec4Graphics::ONES, "Scene Update  : %5.2fms", sceneUpdateTimer.GetTimedMS());
	}

	Scene::UpdateScene(sec);
}

void MyScene::RenderScene()
{
	if ((RenderModeMasks[m_RenderMode] & RenderModeMasks[NormalRenderMode]) != 0) {
		glClearColor(0.6f, 0.6f, 0.6f, 1.f);
		if ((RenderModeMasks[m_RenderMode] & RenderModeMasks[DebugRenderMode]) == 0)
			//force buffer swap and previous buffer cleanup
			NCLDebug::g_NewData = true;
		Scene::RenderScene();
		switch (m_EndState) {
		case LoseGameState:
			break;
		case QuitGameState:
			m_EndScene = true;
			break;
		case HighScoreGameState:
			m_EndScene = true;
			break;
		}
	} else {
		glClearColor(0.0f, 0.0f, 0.0f, 1.f);
		glBindFramebuffer(GL_FRAMEBUFFER, m_ScreenFBO);
		viewMatrix = m_Camera->BuildViewMatrix();
		glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		RenderDebug();
	}
}

void MyScene::UpdateName()
{
	Keyboard* keyboard = Window::GetKeyboard();

	unsigned int size = KEYBOARD_Z - KEYBOARD_A;
	for (unsigned int i = 0; i < size; ++i) {
		if (keyboard->KeyTriggered((KeyboardKeys)(KEYBOARD_A + i)))
			m_HighScoreNameStream << (char)('A' + i);
	}

	if (keyboard->KeyTriggered(KEYBOARD_BACK)) {
		std::string highScoreString = m_HighScoreNameStream.str();
		if (highScoreString.length() > 0) {
			m_HighScoreNameStream.str(highScoreString.substr(0, highScoreString.length() - 1));
			m_HighScoreNameStream.seekp(0, ios::end);
		}
	}
	if (m_HighScoreNameStream.tellp() > ScoreHistory::MaxNameLength) {
		m_HighScoreNameStream.str(m_HighScoreNameStream.str().substr(0, ScoreHistory::MaxNameLength));
		m_HighScoreNameStream.seekp(0, ios::end);
	}
}


void MyScene::DisplayHighScores(float startX, float startY)
{
	unsigned int lineNo = 0;
	float fontSize = 25.0f;
	float cs_Size_y = fontSize / Window::GetWindow().GetScreenSize().y * 2.0f;
	for (ScoreHistory::HighScoreRecord& record : ScoreHistory::Instance()->HighScores)
		NCLDebug::DrawTextClipSpace(Vec4Graphics(startX, 1.0f - startY - (cs_Size_y * lineNo++), 0.0f, 1.0f), fontSize, record.ToString(), TEXTALIGN_LEFT, Vec4Graphics::ONES);
}