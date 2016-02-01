#pragma once
#include "ncltech\tsingleton.h"
#include "TardisGameObject.h"

#include <vector>
#include <mutex>

#define COLLISION_POINTS 1000
#define OVERLAP_POINTS_PER_SECOND 1000.0f

class GameObject;

class ScoreManager : public TSingleton<ScoreManager> {
	friend class TSingleton < ScoreManager >;
	static std::mutex g_ScoringMutex;
 protected:
	std::vector<GameObject*> m_ScoringObjects[2];
	std::vector<std::pair<GameObject*, float>> m_PenetratingObjects;
	std::vector<GameObject*> m_CollidingObjects;
	unsigned int m_WritingIndex;
	unsigned int m_Score;
	float m_LastAlpha;
	bool m_GameOver;

 public:
	ScoreManager(void);
	~ScoreManager(void);

	inline bool GetGameOver() const { return m_GameOver; }

	void SetScore(unsigned int score);
	unsigned int GetScore() const { return m_Score; }

	void AddScoringObject(GameObject* object, TardisGameObject* tardis);

	void Update(float deltaTime);

};

