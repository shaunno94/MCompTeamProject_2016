#pragma once
#include "Rendering\MenuOrthoComponent.h"
#include "MenuController.h"


class UIControllerManager
{
public:
	UIControllerManager();
	~UIControllerManager();

	virtual void setProducer(MenuOrthoComponent* g, unsigned int type);
	virtual void update(float ms);

	void SetAvailableScene(std::vector<Scene*>& scenes) { m_Scenes = scenes; };

protected:
	std::vector<MenuController*> controllers;
	std::vector<Scene*> m_Scenes;
};