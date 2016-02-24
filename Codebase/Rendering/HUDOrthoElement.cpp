#include "HUDOrthoElement.h"

HUDOrthoElement::HUDOrthoElement(float z) : GUIOrthoElements(z)
{
	depth = z;
}

HUDOrthoElement::~HUDOrthoElement()
{

}

bool HUDOrthoElement::Update(float dt)
{
	for (unsigned int i = 0; i < m_Children.size(); i++)
		m_Children[i]->Update(dt);
}

void HUDOrthoElement::Render(float dt)
{
	for (unsigned int i = 0; i < m_Children.size(); i++)
		m_Children[i]->Render(dt);
}


