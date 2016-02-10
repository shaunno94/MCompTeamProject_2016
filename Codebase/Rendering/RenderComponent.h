#pragma once

#include "Rendering/Material.h"
#include "Rendering/Mesh.h"

class GameObject;

/// @ingroup Rendering
/// <summary>
/// Component of the <see cref="GameObject"/> that defines rendering information.
/// <remarks>
/// RenderComponents instances can manage some values of the associated <see cref="GameObject"/> and can not be reused.
/// Provided <see cref="Material"/> and <see cref="Mesh"/> instances don't need to be unique.
/// </remarks>
/// </summary>
class RenderComponent
{
public:
	RenderComponent(Material* material, Mesh* mesh);
	~RenderComponent();

	Material*	m_Material;
	Mesh*		m_Mesh;

	void Draw();

	/// <summary>
	/// Initialization function that is called during the assignment to a <see cref="GameObject"/>.
	/// </summary>
	/// <param name="go"></param>
	void SetParent(GameObject* go);

	bool disabled = false;

protected:
	GameObject* m_GameObject;
};

