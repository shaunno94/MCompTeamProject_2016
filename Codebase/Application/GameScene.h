#pragma once

#include "Rendering\Scene.h"
#include "Rendering\Renderer.h"
#include "Rendering\OGLShader.h"
#include "Rendering\ModelLoader.h"
#include "Rendering\LocalControlManager.h"
#include "Stadium.h"
#include "Rendering\DebugDraw.h"
#include "Rendering\GUISystem.h"
#include "Rendering\WheelObject.h"
#include "CarGameObject.h"
#include "Audio\SoundSystem.h"
#include "Rendering\ParticleManager.h"

#ifndef ORBIS
#include "Rendering\KeyboardController.h"
const string SIMPLESHADER_VERT = SHADER_DIR"textureVertex.glsl";
const string SIMPLESHADER_FRAG = SHADER_DIR"textureFragment.glsl";
const string POINTLIGHTSHADER_VERT = SHADER_DIR"2dShadowLightvertex.glsl";
const string POINTLIGHTSHADER_FRAG = SHADER_DIR"2dShadowLightfragment.glsl";
const string GUI_VERT = SHADER_DIR"combineVert.glsl";

#else
#include "Rendering\PS4Controller.h"
const std::string SIMPLESHADER_VERT = SHADER_DIR"textureVertex.sb";
const std::string SIMPLESHADER_FRAG = SHADER_DIR"textureFragment.sb";
const std::string POINTLIGHTSHADER_VERT = SHADER_DIR"2dShadowLightvertex.sb";
const std::string POINTLIGHTSHADER_FRAG = SHADER_DIR"2dShadowLightfragment.sb";

#endif
#include "BulletCollision\CollisionDispatch\btCollisionWorld.h"

struct GameCollisionFilter;

class GameScene : public Scene
{
public:
	GameScene(ControllerManager* controller);
	~GameScene();

	void LoadAudio();
	void SetupGameObjects();
	void SetupShaders();
	void SetupMaterials();
	void SetupControls();
	void DrawGUI();
	void SetControllerActor();

	void IncrementScore(int team) {
		scores[team % 2]++;
		std::cout << "TEAM " << team + 1 << " SCORED!" << std::endl;
		std::cout << scores[0] << " - " << scores[1] << std::endl;
	}
	void ResetScene();

protected:
	ControllerManager* myControllers;
	GameCollisionFilter* goalBallFilter;
	int scores[2];

	void ResetObjects();

	GameObject* ball;
	GameObject* light1;
	GameObject* light2;
	GameObject* player;
	GameObject* shooterAI;
	GameObject* goalieAI;
	GameObject* stadium;
	GameObject* goal1;
	GameObject* goal2;

	RigidPhysicsObject* ballPhysics;
	RigidPhysicsObject* goalBox;
	RigidPhysicsObject* goalBox2;

	BaseShader* simpleShader;
	BaseShader* pointlightShader;
	BaseShader* orthoShader;

	LightMaterial* lightMaterial;

	Material* material;
	Material* ballMaterial;
	Material* netMaterial;
	Material* playerMaterial;
	Material* aiMaterial;
	Material* ai2Material;
	Material* particleMaterial;

	ControllerComponent* cc;

	int ballID;
	int goal1ID;
	int goal2ID;
};

struct GameCollisionFilter : public btOverlapFilterCallback
{

public:

	int m_ballID = 0;
	int m_goal1ID = 0;
	int m_goal2ID = 0;
	GameScene* m_scene;

	GameCollisionFilter(GameScene* scene) : m_scene(scene) {

	}


	virtual bool needBroadphaseCollision(btBroadphaseProxy* proxy0, btBroadphaseProxy* proxy1) const override
	{
		short int combined = COL_CAR | COL_WALL;
		/*int combinedMask = proxy0->m_collisionFilterMask | proxy1->m_collisionFilterMask;
		int test1 = (combinedMask & COL_CAR) == COL_CAR;
		int test2 = (combinedMask & COL_WALL) == COL_WALL;
		int test3;*/
		if ((proxy0->m_collisionFilterMask | proxy1->m_collisionFilterMask) & combined == combined)
		{
			int test = 0;
		}
		else
		{
			int test2 = 0;
		}
		//if (((combinedMask & COL_CAR) == COL_CAR) && ((combinedMask & COL_WALL) == COL_WALL)) {
		//	std::cout << "Car and wall collision" << std::endl;
		//	// sort out car-wall collision
		//}

		if ((proxy0->getUid() == m_ballID && proxy1->getUid() == m_goal1ID) ||
			(proxy1->getUid() == m_ballID && proxy0->getUid() == m_goal1ID))
		{
			//Increment goals for team 1
			m_scene->IncrementScore(0);
			//TODO: Reset Scene
			m_scene->ResetScene();
		}
		else if ((proxy0->getUid() == m_ballID && proxy1->getUid() == m_goal2ID) ||
			(proxy1->getUid() == m_ballID && proxy0->getUid() == m_goal2ID))
		{
			//Increment goals for team 2
			m_scene->IncrementScore(1);
			//TODO: Reset Scene
			m_scene->ResetScene();
		}
		return true;
	}
};

