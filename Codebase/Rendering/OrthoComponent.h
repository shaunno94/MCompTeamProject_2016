#include "GUIComponent.h"

class OrthoComponent
{
public:
	OrthoComponent(const float z);
	~OrthoComponent();

	void AddGUIComponent(GUIComponent* element);
	void RemoveGUIComponent(GUIComponent* element);

	void Update();
	void Render();

	bool operator<(const OrthoComponent &rhs) const { return depth < rhs.depth; };

protected:
	const float depth;
	std::vector<GUIComponent*> m_elements;
};