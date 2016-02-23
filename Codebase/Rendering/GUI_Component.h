#include <vector>

class GUI_Component
{
public:
	GUI_Component();
	virtual ~GUI_Component();

	virtual bool Update(float delta) = 0;
	virtual void Render() = 0;

	std::vector<GUI_Component*>& GetChildren()
	{
		return m_Children;
	}

protected:
	GUI_Component*					m_Parent;
	std::vector<GUI_Component*>		m_Children;
};