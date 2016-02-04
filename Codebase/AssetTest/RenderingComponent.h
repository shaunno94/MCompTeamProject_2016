#pragma once

#include "Rendering/Material.h"
#include "Rendering/Mesh.h"

class RenderingComponent
{
public:
	RenderingComponent();
	~RenderingComponent();

	Material*	m_material;
	Mesh*		m_mesh;
};

