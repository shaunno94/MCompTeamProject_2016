
class ParticleManager
{
public:
	ParticleManager();

	void AddSystem();
	void RemoveSystem();
	void Update();
	void Render();
	void Shutdown();

	bool DoesExist();
protected:
private:
};