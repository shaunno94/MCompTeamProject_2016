#pragma once

#include "Rendering/Material.h"
#include "Rendering/Mesh.h"

class GameObject;
class Renderer;

class RenderComponent
{
public:
	RenderComponent(Material* material, Mesh* mesh);
	~RenderComponent();

	Material*	m_Material;
	Mesh*		m_Mesh;

	void Draw();

	void SetParent(GameObject* go);

protected:
	GameObject* m_GameObject;
	int m_ModelMatrixLocattion;
};

