#include <cstdio>
#include "Rendering\Renderer.h"
#include "PhysicsEngine\PhysicsEngineInstance.h"
#include "Rendering\ModelLoader.h"
#include "Rendering\DebugDraw.h"
#include "Rendering\GameTimer.h"
#include "Stadium.h"

// Includes for AI States and Triggers
#include "AI\StateMachine.h"
#include "AI\ChaseState.h"
#include "AI\RunAwayState.h"
#include "AI\DistanceTrigger.h"

const float TIME_STEP = 1.0f / 120.0f;
const unsigned int SUB_STEPS = 4;

int main(void) {
	//-------------------
	//--- MAIN Loop ---
	//-------------------

	//Initialise Renderer - including the window context if compiling for Windows - PC
	Renderer renderer("Team Project - 2016", 1280, 800, false);
	if (!renderer.HasInitialised()) {
		return -1;
	}

	GameTimer timer;

	//Initialise Bullet physics engine.
	PhysicsEngineInstance::Instance()->setGravity(btVector3(0, -9.81, 0));

#if DEBUG_DRAW
	PhysicsEngineInstance::Instance()->setDebugDrawer(DebugDraw::Instance());
	DebugDraw::Context(&renderer);
#endif

	//Test Scenario - Tardis (cuboid collision shape), floor (plane collision shape), ball (sphere collison shape)
	Scene* myScene = new Scene();
	myScene->getCamera()->SetPosition(Vec3Graphics(10, 5, 0));

	//Game objects added to scene are delete by the scene so don't delete twice.
	GameObject* ball = new GameObject("ball");
	GameObject* aiBall = new GameObject("aiBall");
	GameObject* light1 = new GameObject("l");
	GameObject* light2 = new GameObject("l");

	//Physics objects hold collision shape and collision object(body), 
	//call CreateCollisionShape before CreatePhysicsBody or the object will not be created correctly.
	//Physics objects will be deleted by the game object.
	RigidPhysicsObject* ballPhysics = new RigidPhysicsObject();
	ballPhysics->CreateCollisionShape(5.0);
	ballPhysics->CreatePhysicsBody(5.0, Vec3Physics(0, 0, 0), QuatPhysics(0, 0, 0, 1), Vec3Physics(1, 1, 1));

	RigidPhysicsObject* aiBallPhysics = new RigidPhysicsObject();
	aiBallPhysics->CreateCollisionShape(2.0);
	aiBallPhysics->CreatePhysicsBody(2.0, Vec3Physics(0.11, 15, 0.5), QuatPhysics(0, 0, 0, 1), Vec3Physics(1, 1, 1));
	
	RigidPhysicsObject* floorPhysics = new RigidPhysicsObject();
	floorPhysics->CreateCollisionShape(0, Vec3Physics(0, 1, 0), true);
	floorPhysics->CreatePhysicsBody(0, Vec3Physics(0, -1, 0), QuatPhysics(0, 0, 0, 1));

#ifndef ORBIS
	BaseShader* simpleShader = new OGLShader(SHADER_DIR"textureVertex.glsl", SHADER_DIR"textureFragment.glsl");
	BaseShader* pointlightShader = new OGLShader(SHADER_DIR"2dShadowLightvertex.glsl", SHADER_DIR"2dShadowLightfragment.glsl");
	//BaseShader* pointlightShader = new OGLShader(SHADER_DIR"CubeShadowLightvertex.glsl", SHADER_DIR"CubeShadowLightfragment.glsl");
#else
	BaseShader* simpleShader = new PS4Shader(COMPILED_SHADER_DIR_D"textureVertex.glsl", COMPILED_SHADER_DIR_D"textureFragment.glsl");
	BaseShader* pointlightShader = new PS4Shader(COMPILED_SHADER_DIR_D"2dShadowLightvertex.glsl", COMPILED_SHADER_DIR_D"2dShadowLightfragment.glsl");
#endif
	
	if (!pointlightShader->IsOperational() || !simpleShader->IsOperational())
		return -1;


	LightMaterial* lightMaterial = new LightMaterial(pointlightShader);
	light1->SetRenderComponent(new RenderComponent(lightMaterial, ModelLoader::LoadMGL(MODEL_DIR"Common/ico.mgl", true)));
	light1->SetWorldTransform(Mat4Graphics::Translation(Vec3Graphics(0, 2, 2)) *Mat4Graphics::Scale(Vec3Graphics(20, 20, 20)));
	light1->SetBoundingRadius(20);

	lightMaterial->shadowType=_2D;
	light2->SetRenderComponent(new RenderComponent(lightMaterial, ModelLoader::LoadMGL(MODEL_DIR"Common/ico.mgl", true)));
	light2->SetWorldTransform(Mat4Graphics::Translation(Vec3Graphics(600, 600, 600)) *Mat4Graphics::Scale(Vec3Graphics(1600, 1600, 1600)));
	light2->SetBoundingRadius(1600);

	Material* material = new Material(simpleShader);
	Material* ballMaterial = new Material(simpleShader);
	ballMaterial->Set(ReservedMeshTextures.DIFFUSE.name, Texture::Get(TEXTURE_DIR"checkerboard.tga", true));

	// Create Stadium
	GameObject* stadium = new Stadium(material, "stadium"); 

	myScene->addGameObject(stadium);
	//myScene->addLightObject(light1);
	myScene->addLightObject(light2);

	ball->SetRenderComponent(new RenderComponent(ballMaterial, ModelLoader::LoadMGL(MODEL_DIR"Common/sphere.mgl", true)));
	ball->SetLocalTransform(Mat4Graphics::Scale(Vector3Simple(5, 5, 5)));
	ball->SetPhysicsComponent(ballPhysics);
	ball->GetPhysicsComponent()->GetPhysicsBody()->setRestitution(btScalar(0.9));
	ball->GetPhysicsComponent()->GetPhysicsBody()->setFriction(0.5);
	ball->GetPhysicsComponent()->GetPhysicsBody()->setHitFraction(0.5);

	aiBall->SetRenderComponent(new RenderComponent(ballMaterial, ModelLoader::LoadMGL(MODEL_DIR"Common/sphere.mgl", true)));
	aiBall->SetLocalTransform(Mat4Graphics::Scale(Vector3Simple(2, 2, 2)) * Mat4Graphics::Translation(Vector3Simple(0, 0, 0)));
	aiBall->SetPhysicsComponent(aiBallPhysics);
	aiBall->GetPhysicsComponent()->GetPhysicsBody()->setRestitution(btScalar(0.9));
	aiBall->GetPhysicsComponent()->GetPhysicsBody()->setFriction(0.5);
	aiBall->GetPhysicsComponent()->GetPhysicsBody()->setHitFraction(0.5);

	//StateMachine* ballStateMachine = new StateMachine();

	//ChaseState* chase = new ChaseState(*ballStateMachine, *aiBall, *ball);
	//RunAwayState* run = new RunAwayState(*ballStateMachine, *aiBall, *ball);


	//// Chase -> Run trigger 
	////	Triggered when two objects are less than 5.0f apart
	//DistanceTrigger* chaseToRun = new DistanceTrigger();
	//chaseToRun->setupTrigger(*aiBall, *ball, 5.0f, true);
	//chase->AddTrigger(chaseToRun, "Run");

	//ballStateMachine->AddState("Chase", chase);


	//// Run -> Chase trigger 
	////	Triggered when two objects are greater than 25.0f apart
	//DistanceTrigger* runToChase = new DistanceTrigger();
	//runToChase->setupTrigger(*aiBall, *ball, 25.0f, false);
	//run->AddTrigger(runToChase, "Chase");

	//ballStateMachine->AddState("Run", run);

	//ballStateMachine->ChangeState("Chase");

	//aiBall->SetStateMachine(ballStateMachine);

	myScene->addGameObject(ball);
	myScene->addGameObject(aiBall);

	renderer.SetCurrentScene(myScene);

	dynamic_cast<RigidPhysicsObject*>(ball->GetPhysicsComponent())->GetPhysicsBody()->setAngularVelocity(btVector3(1, 0, 0));
	dynamic_cast<RigidPhysicsObject*>(aiBall->GetPhysicsComponent())->GetPhysicsBody()->applyCentralForce(btVector3(10, 0, 0));

#ifndef ORBIS
	while (Window::GetWindow().UpdateWindow() && !Window::GetKeyboard()->KeyDown(KEYBOARD_ESCAPE))
#else
	while (true)
#endif
	{
		float ms = timer.GetTimer()->Get(1000.0f);
		PhysicsEngineInstance::Instance()->stepSimulation(ms, SUB_STEPS, TIME_STEP);
		renderer.RenderScene(ms);
	}

	//Cleanup
	PhysicsEngineInstance::Release();

#if DEBUG_DRAW
	DebugDraw::Release();
#endif

	delete myScene;
	return 0;
}