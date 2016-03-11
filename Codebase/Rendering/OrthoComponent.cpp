#include "OrthoComponent.h"

OrthoComponent::OrthoComponent(const float z)
:depth(z)
{

}

OrthoComponent::~OrthoComponent()
{
	for (unsigned int i = 0; i < m_elements.size(); i++)
	{
		for (int j = 0; j < m_elements[i]->GetChildren().size(); j++)
		{
			delete m_elements[i]->GetChildren()[j];
			m_elements[i]->GetChildren()[j] = nullptr;
			m_elements[i]->GetChildren().clear();
		}
		delete m_elements[i];
		m_elements[i] = nullptr;
		m_elements.clear();
	}
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
	for (unsigned int i = 0; i < m_elements.size(); i++)
	{
		for (int j = 0; j < m_elements[i]->GetChildren().size(); j++)
		{
			dynamic_cast<GUIComponent*>(m_elements[i]->GetChildren()[j])->Render();
		}
		m_elements[i]->Render();
	}
}