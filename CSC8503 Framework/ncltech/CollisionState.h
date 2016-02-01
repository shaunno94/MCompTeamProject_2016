#pragma once
#include "DataSourceState.h"
#include <vector>
#include <string>
#include <functional>

class PhysicsObject;

class CollisionState : public DataSourceState
{
private:
	std::vector<std::string> m_Matches;
	std::function<bool(PhysicsObject*)> m_CollisionCallback;
	std::function<bool(PhysicsObject*)> m_OriginalCallback;
	std::vector<GameObject*> m_Targets[2];
	unsigned int m_WritingIndex;
	volatile bool m_UpdateRequest;

	std::vector<GameObject*> m_ReturnValue;

public:
	CollisionState(GameObject& go);
	~CollisionState(void);

	void Start(State* previousState) override;

	void Update(float deltaTime) override;

	void End() override;

	void AddMatch(std::string& str);

	void* GetData() override;
};

