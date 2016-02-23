#include <vector>

class GUIOrthoElements
{
public:
	GUIOrthoElements(float z);
	virtual ~GUIOrthoElements();

	virtual bool Update(float delta) = 0;
	virtual void Render() = 0;

	std::vector<GUIOrthoElements*>& GetChildren()
	{
		return m_Children;
	}

protected:
	std::vector<GUIOrthoElements*>		m_Children;
	float depth;
};