#pragma once

#include "Rendering/Material.h"
#include "Rendering/Mesh.h"

class GameObject;
class Renderer;

class RenderComponent
{
public:
	RenderComponent();
	~RenderComponent();

	Material*	m_material;
	Mesh*		m_mesh;

	void Draw(Renderer* r);

	void SetParent(GameObject* go);

protected:
	GameObject* m_GameObject;
	int m_ModelMatrixLocattion;
};

