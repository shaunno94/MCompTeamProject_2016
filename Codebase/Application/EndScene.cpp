#include "EndScene.h"
#include "Rendering\Renderer.h"


EndScene::EndScene()
{
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
	delete guiMaterial;
	for (int i = 0; i < 3; i++)
	{
		delete bgMaterials[i];
	}
	if (guiSystem)
		delete guiSystem;
}

void EndScene::Setup(){
	background->GetRenderComponent()->m_Material = bgMaterials[winningTeam];
}

void EndScene::UpdateScene(float dt)
{
	//background->GetRenderComponent()->m_Material = bgMaterials[winningTeam];
#ifndef ORBIS
	if (Window::GetWindow().GetKeyboard()->KeyTriggered(KEYBOARD_SPACE) || Window::GetWindow().GetKeyboard()->KeyTriggered(KEYBOARD_RETURN))
		Renderer::GetInstance()->SetCurrentScene(Renderer::GetInstance()->GetScene(0));
#else
	if (PS4Input::getPS4Input()->GetButtonTriggered(BTN_CROSS))
		Renderer::GetInstance()->SetCurrentScene(Renderer::GetInstance()->GetScene(0));
#endif // !ORBIS

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
	bgMaterials[RED_WINS] = new Material(orthoShader);
	bgMaterials[RED_WINS]->Set(ReservedMeshTextures.DIFFUSE.name, Texture::Get(TEXTURE_DIR"WINRED.png"));
	bgMaterials[BLUE_WINS] = new Material(orthoShader);
	bgMaterials[BLUE_WINS]->Set(ReservedMeshTextures.DIFFUSE.name, Texture::Get(TEXTURE_DIR"WINBLUE.png"));
	bgMaterials[TIE] = new Material(orthoShader);
	bgMaterials[TIE]->Set(ReservedMeshTextures.DIFFUSE.name, Texture::Get(TEXTURE_DIR"TIE.png"));

	//textMaterial = new Material(orthoShader);
}

void EndScene::DrawGUI()
{
	bgOrtho = new OrthoComponent(1.0f);
	background = new MenuBackgroundGUI(bgMaterials[TIE]);
	bgOrtho->AddGUIComponent(background);

	menuOrtho = new MenuOrthoComponent(0.5);
	

	guiSystem->AddOrthoComponent(bgOrtho);
	guiSystem->AddOrthoComponent(menuOrtho);
}

void EndScene::LoadAudio()
{

}

void EndScene::SetupControls()
{

}




