#include "GUIComponent.h"

class GUISystem
{
public:
	static bool Initialise();
	static void Destroy();
	static GUISystem& GetInstance(){ return *guiSystem; };

	void AddComponent(GUIComponent* element);
	void RemoveComponent(GUIComponent* element);
	void Update();
	void Render();
	void Shutdown();

	bool HasInitialised();

protected:
	static GUISystem* guiSystem;
	bool init;

private:
	GUISystem();
	~GUISystem(void);
	std::vector<GUIComponent*> m_elements;

};