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
const string SIMPLESHADER_VERT = SHADER_DIR"textureVertex.sb";
const string SIMPLESHADER_FRAG = SHADER_DIR"textureFragment.sb";
const string POINTLIGHTSHADER_VERT = SHADER_DIR"2dShadowLightvertex.sb";
const string POINTLIGHTSHADER_FRAG = SHADER_DIR"2dShadowLightfragment.sb";

#endif

struct GoalBallCollisionFilter : public btOverlapFilterCallback
{

public:

	int m_ballID = 0;
	int m_goal1ID = 0;
	int m_goal2ID = 0;

	virtual bool needBroadphaseCollision(btBroadphaseProxy* proxy0, btBroadphaseProxy* proxy1) const override
	{
		if ((proxy0->getUid() == m_ballID && proxy1->getUid() == m_goal1ID) ||
			(proxy1->getUid() == m_ballID && proxy0->getUid() == m_goal1ID))
		{
			//TODO: Increment goals for team 1
			int ifojwe = 8;

			//TODO: Reset Scene
		}
		else if ((proxy0->getUid() == m_ballID && proxy1->getUid() == m_goal2ID) ||
			(proxy1->getUid() == m_ballID && proxy0->getUid() == m_goal2ID))
		{
			//TODO: Increment goals for team 2
			int ifojwe = 8;

			//TODO: Reset Scene
		}
		return true;
	}
};


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
	
protected:
	ControllerManager* myControllers;
	GoalBallCollisionFilter goalBallFilter;

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

