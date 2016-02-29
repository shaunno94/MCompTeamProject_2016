#pragma once
#include "GameObject.h"

class GUIComponent : public GameObject
{
public:
	GUIComponent(Material* material, Texture* texture, float z, const std::string& name = "") : GameObject(name){};
	virtual ~GUIComponent()
	{
		delete m_Material;
		delete m_Mesh;
	}

	virtual void Update() = 0;
	virtual void Render() = 0;

	bool operator<(const GUIComponent &rhs) const { return depth < rhs.depth; };


protected:
	float depth;

	Material*	m_Material;
	Mesh *		m_Mesh;
	Texture*	m_Texture;

	std::vector<GUIComponent*>	m_Children;
};