#include "EndScene.h"


EndScene::EndScene()
{
	myControllers = new UIControllerManager();

	guiSystem = new GUISystem();

	if (!guiSystem->HasInitialised())
	{
		std::cout << "GUI not Initialised!" << std::endl;
	}


	SetupShaders();
	SetupMaterials();
	SetupGameObjects();
	DrawGUI();
	SetupControls();
}


EndScene::~EndScene()
{
	delete btnMaterial;
	delete bgMaterial;
	delete guiMaterial;
	delete selectBtnMaterial;
	if (guiSystem)
		delete guiSystem;
}


void EndScene::UpdateScene(float dt)
{
	myControllers->update(dt);
	m_Selected = menuOrtho->Update();
}

void EndScene::SetupGameObjects()
{

}

void EndScene::SetupShaders()
{
#ifndef ORBIS
	orthoShader = new OGLShader(END_VERT, END_FRAG);
#else

#endif

	if (!orthoShader->IsOperational())
		std::cout << "Shader not opertational!" << std::endl;
}

void EndScene::SetupMaterials()
{
	guiMaterial = new Material(orthoShader);
	bgMaterial = new Material(orthoShader);
	btnMaterial = new Material(orthoShader);
	selectBtnMaterial = new Material(orthoShader);

	//textMaterial = new Material(orthoShader);
}

void EndScene::DrawGUI()
{
	bgOrtho = new OrthoComponent(1.0f);
	background = new MenuBackgroundGUI(bgMaterial);
	bgOrtho->AddGUIComponent(background);

	menuOrtho = new MenuOrthoComponent(0.5);
	toMenuBtn = new ButtonGUIComponent(btnMaterial, selectBtnMaterial, Vec3Graphics(-0.7f, 0.7f, 0), Vec2Graphics(0.2f, 0.1f));
	restartBtn = new ButtonGUIComponent(btnMaterial, selectBtnMaterial, Vec3Graphics(-0.7f, 0.4f, 0), Vec2Graphics(0.2f, 0.1f));

	menuOrtho->AddGUIComponent(toMenuBtn);
	menuOrtho->AddGUIComponent(restartBtn);

	guiSystem->AddOrthoComponent(bgOrtho);
	guiSystem->AddOrthoComponent(menuOrtho);
}

void EndScene::LoadAudio()
{

}

void EndScene::SetupControls()
{
	myControllers->setProducer(menuOrtho, 0);
}




