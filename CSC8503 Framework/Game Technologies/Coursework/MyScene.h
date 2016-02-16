#pragma once

#include <nclgl\Mesh.h>
#include <ncltech\Scene.h>
#include <ncltech\SimpleMeshObject.h>
#include <nclgl\GameTimer.h>
#include "GoalkeeperGameObject.h"
#include <sstream>

enum RenderMode : unsigned int {
	NormalRenderMode,
	DebugRenderMode,
	NormalAndDebugRenderMode,
	RenderModeMax
};
static const unsigned int RenderModeMasks[RenderModeMax] = {
	1,
	2,
	3
};

enum EndGameStates {
	LoseGameState,
	HighScoreGameState,
	QuitGameState
};

class MyScene : public Scene {
 public:
	MyScene(Window& window);
	~MyScene();

	bool InitialiseGL()	override;
	
	void initStateMachine();

	void RenderScene() override;
	void UpdateScene(float dt)  override;
	void	Cleanup() override;
	//Scene* GetNextScene(Window& window) override;

 protected:
	GLuint			m_ProjectileTex;
	unsigned int	m_RenderMode;
	GameTimer		sceneUpdateTimer;
	GoalkeeperGameObject* m_Goalkeeper;
	std::vector<GameObject*> m_Resources;
	bool			m_DisplayHighScores;

	SimpleMeshObject* m_Overlay;
	Shader* m_OverlayShader;
	EndGameStates	m_EndState;
	std::stringstream m_HighScoreNameStream;
	void UpdateName();
	void DisplayHighScores(float startX, float startY);

	StateMachine* m_sphereStateMachine;

};