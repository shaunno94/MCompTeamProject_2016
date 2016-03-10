#pragma once
#include "GUIComponent.h"

class OrthoComponent
{
public:
	OrthoComponent(const float z);
	~OrthoComponent();

	void AddGUIComponent(GUIComponent* element);
	void RemoveGUIComponent(GUIComponent* element);

	void Update();
	virtual void Render();

	bool operator<(const OrthoComponent &rhs) const { return depth < rhs.depth; };

	std::vector<GUIComponent*>& GetElements()
	{
		return m_elements;
	}

protected:
	const float depth;
	std::vector<GUIComponent*> m_elements;
};