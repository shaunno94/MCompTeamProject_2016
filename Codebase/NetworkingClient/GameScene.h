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
#include "Rendering\ScoreboardGUIComponent.h"
#include "PickupManager.h"

#ifndef ORBIS
#include "Rendering\KeyboardController.h"
const string SIMPLESHADER_VERT = SHADER_DIR"textureVertex.glsl";
const string SIMPLESHADER_FRAG = SHADER_DIR"textureFragment.glsl";
const string POINTLIGHTSHADER_VERT = SHADER_DIR"2dShadowLightvertex.glsl";
const string POINTLIGHTSHADER_FRAG = SHADER_DIR"2dShadowLightfragment.glsl";
const string GUI_VERT = SHADER_DIR"TexturedVertex.glsl";
const string GUI_FRAG = SHADER_DIR"TexturedFragment.glsl";
#else
#include "Rendering\PS4Controller.h"
const std::string SIMPLESHADER_VERT = SHADER_DIR"textureVertex.sb";
const std::string SIMPLESHADER_FRAG = SHADER_DIR"textureFragment.sb";
const std::string POINTLIGHTSHADER_VERT = SHADER_DIR"2dShadowLightvertex.sb";
const std::string POINTLIGHTSHADER_FRAG = SHADER_DIR"2dShadowLightfragment.sb";
const std::string GUI_VERT = SHADER_DIR"TexturedVertex.sb";
const std::string GUI_FRAG = SHADER_DIR"TexturedFragment.sb";
#endif
#include "BulletCollision\CollisionDispatch\btCollisionWorld.h"
#include "Helpers\DeltaTimer.h"

struct GameCollisionFilter;

class GameScene : public Scene
{
public:
	GameScene();
	~GameScene();

	void LoadAudio();
	void SetupGameObjects();
	void SetupShaders();
	void SetupMaterials();
	void SetupControls();
	void DrawGUI();

	void SetControllerActor();
	void SetupAI();

	void IncrementScore(int team);

	virtual void UpdateScene(float dt) override;

	void TriggerExplosion();

	void SetGoalScored(int goal) { goalScored = goal; }
	int GetGoalScored() { return goalScored; }

	PickupManager* GetPickupManager() {
		return pickupManager;
	}

	GUISystem* getGUISystem(){ return guiSystem; }
	virtual void Setup() override;
	virtual void Cleanup() override;


protected:
	
	GUISystem* guiSystem;

	ControllerManager* myControllers;
	int scores[2];

	void ResetObjects();
	void ResetObject(GameObject& object);

	void applyImpulseFromExplosion(CarGameObject* car);
	GameObject* ball;
	
	GameObject* light1;
	GameObject* light2;
	
	GameObject* player;
	
	GameObject* shooterAI;
	GameObject* goalieAI;
	GameObject* aggroAI;

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
	Material* netMaterial;
	Material* postMaterial;

	Material* playerMaterial;
	Material* aiMaterial;
	Material* ai2Material;
	Material* particleMaterial;
	Material* guiMaterial;
	Material* textMaterial;

	PickupManager* pickupManager;

	ControllerComponent* cc;

	OrthoComponent* hudOrtho;
	TextGUIComponent* FPSDebugTextComponent;
	TextGUIComponent* physicsDebugTextComponent;
	TextGUIComponent* graphicsDebugTextComponent;
	ScoreboardGUIComponent* scoreboardComponent;

	float timerCount = 0.0f;
	float currentTime = 0.0f;
	float lastTime = 0;
	int goalScored = 0;

	bool gameStart;
};

