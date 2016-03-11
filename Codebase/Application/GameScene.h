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
#include "EndScene.h"

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
	void SetupParticles();
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

	GUISystem* getGUISystem() override { return guiSystem; }
	virtual void Setup() override;
	virtual void Cleanup() override;


protected:

	GUISystem* guiSystem;

	
	int scores[2];

	void ResetObjects();
	void ResetObject(GameObject& object);

	void applyImpulseFromExplosion(CarGameObject* car);
	GameObject* ball;
	
	//GameObject* light1;
	GameObject* light2;
	
	GameObject* player;
	
	GameObject* shooterAI;
	GameObject* goalieAI;
	GameObject* aggroAI;

	GameObject* stadium;
	GameObject* redGoal;
	GameObject* blueGoal;

	std::vector<GameObject*> m_cars;

	RigidPhysicsObject* ballPhysics;
	RigidPhysicsObject* redGoalBox;
	RigidPhysicsObject* blueGoalBox;

	BaseShader* simpleShader;
	BaseShader* colourShader;
	BaseShader* pointlightShader;
	BaseShader* orthoShader;
	BaseShader* particleShader;

	LightMaterial* lightMaterial;

	Material* material;
	Material* netMaterial;
	Material* ballMaterial;
	ExtendedMaterial* redPostMaterial;
	ExtendedMaterial* bluePostMaterial;

	Material* playerMaterial;
	Material* aiMaterial;
	Material* ai2Material;
	Material* particleMaterial;
	Material* guiMaterial;
	Material* textMaterial;

	PickupManager* pickupManager;

	ControllerComponent* cc;

	OrthoComponent* hudOrtho;
	TextGUIComponent* speedComponent;
	TextGUIComponent* boostComponent;
	TextGUIComponent* FPSDebugTextComponent;
	TextGUIComponent* physicsDebugTextComponent;
	TextGUIComponent* graphicsDebugTextComponent;
	ScoreboardGUIComponent* scoreboardComponent;

	ParticleSystem* playerParticleSystem;
	ParticleSystem* AI1_ParticleSystem;
	ParticleSystem* AI2_ParticleSystem;
	ParticleSystem* AI3_ParticleSystem;

	float timerCount = 0.0f;
	float currentTime = 0.0f;
	float lastTime = 0;
	int goalScored = 0;

	bool gameStart;
};

