#include <vector>

class GUIOrthoElements
{
public:
	GUIOrthoElements(float z = 0.0f);
	virtual ~GUIOrthoElements();

	virtual bool Update(float dt) = 0;
	virtual void Render(float dt) = 0;

	std::vector<GUIOrthoElements*>& GetChildren()
	{
		return m_Children;
	}

protected:
	std::vector<GUIOrthoElements*>		m_Children;
	float depth;
};