#include "OrthoComponent.h"

OrthoComponent::OrthoComponent(const float z)
:depth(z)
{

}

OrthoComponent::~OrthoComponent()
{

}

void OrthoComponent::AddGUIComponent(GUIComponent* element)
{
	m_elements.push_back(element);
}

void OrthoComponent::RemoveGUIComponent(GUIComponent* element)
{
	for (unsigned int i = 0; i < m_elements.size(); ++i)
	{
		if (m_elements[i] == element)
			m_elements.erase(m_elements.begin());
	}
}

//void OrthoComponent::Update()
//{
//	for (unsigned int i = 0; i < m_elements.size(); ++i)
//		m_elements[i]->Update();
//}

void OrthoComponent::Render()
{
	std::sort(m_elements.begin(), m_elements.end());

	for (unsigned int i = 0; i < m_elements.size(); ++i)
		m_elements[i]->Render();
}