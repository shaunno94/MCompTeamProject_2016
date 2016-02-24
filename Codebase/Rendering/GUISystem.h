#include "GUIOrthoElements.h"

class GUISystem
{
public:
	static bool Initialise();
	static void Destroy();
	static GUISystem& GetSystem(){ return *guiSystem; };

	void AddOrthoElement(GUIOrthoElements* element);
	void RemoveOrthoElement(GUIOrthoElements* element);
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
	std::vector<GUIOrthoElements*> m_elements;

};