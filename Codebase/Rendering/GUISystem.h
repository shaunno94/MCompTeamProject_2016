#include "GUI_Component.h"

class GUISystem
{
public:
	static bool Initialise();
	static void Destroy();
	static GUISystem& GetManager(){ return *guiSystem; };

	void AddComponent(GUI_Component* component);
	void RemoveComponent(GUI_Component* component);
	void Update(float delta);
	void Render(float delta);
	void Shutdown();

	bool HasInitialised();

protected:
	static GUISystem* guiSystem;
	bool init;

private:
	GUISystem();
	~GUISystem(void);
	std::vector<GUI_Component*> m_Components;

};