#include "ScoreManager.h"

#include "ncltech\GameObject.h"
#include "ncltech\PhysicsEngine.h"
#include <algorithm>

ScoreManager::ScoreManager(void) : m_Score(0), m_WritingIndex(0), m_GameOver(false) {
}


ScoreManager::~ScoreManager(void) {
}

void ScoreManager::SetScore(unsigned int score) {
	m_Score = score;
}

std::mutex ScoreManager::g_ScoringMutex;

void ScoreManager::AddScoringObject(GameObject* object, TardisGameObject* tardis) {
	if (m_GameOver)
		return;

	std::lock_guard<std::mutex> guard(g_ScoringMutex);
	unsigned int writingIndex = m_WritingIndex;
	auto match = std::find(
	               m_ScoringObjects[writingIndex].begin(),
	               m_ScoringObjects[writingIndex].end(),
	               object
	             );
	if (match == m_ScoringObjects[writingIndex].end()) {
		m_ScoringObjects[writingIndex].push_back(object);
		m_LastAlpha = tardis->GetColour().w;
	}
}

void ScoreManager::Update(float deltaTime) {
	if (m_GameOver)
		return;

	unsigned int readingIndex = 0;
	{
		std::lock_guard<std::mutex> guard(g_ScoringMutex);
		readingIndex = m_WritingIndex;
		m_WritingIndex = (m_WritingIndex + 1) % 2;
	}

	//erase old records that are no longer penetrating
	m_PenetratingObjects.erase(
	  std::remove_if(
	    m_PenetratingObjects.begin(),
	    m_PenetratingObjects.end(),
	[&](std::pair<GameObject*, float>& pair) {
		return std::find(m_ScoringObjects[readingIndex].begin(), m_ScoringObjects[readingIndex].end(), pair.first) == m_ScoringObjects[readingIndex].end();
	}
	  ),
	m_PenetratingObjects.end()
	);

	//erase old records that are no longer colliding
	m_CollidingObjects.erase(
	  std::remove_if(
	    m_CollidingObjects.begin(),
	    m_CollidingObjects.end(),
	[&](GameObject * obj) {
		return std::find(m_ScoringObjects[readingIndex].begin(), m_ScoringObjects[readingIndex].end(), obj) == m_ScoringObjects[readingIndex].end();
	}
	  ),
	m_CollidingObjects.end()
	);

	//update scores
	for (GameObject* go : m_ScoringObjects[readingIndex]) {

		auto matchingPenetratingRecord = std::find_if(
		                                   m_PenetratingObjects.begin(),
		                                   m_PenetratingObjects.end(),
		[&](std::pair<GameObject*, float>& pair) {
			return pair.first == go;
		}
		                                 );
		auto previouslyColliding = std::find(
		                             m_CollidingObjects.begin(),
		                             m_CollidingObjects.end(),
		                             go
		                           );

		if (m_LastAlpha < 1.0f) {
			if (matchingPenetratingRecord == m_PenetratingObjects.end()) {
				if (previouslyColliding == m_CollidingObjects.end())
					m_Score += COLLISION_POINTS;
				m_PenetratingObjects.push_back(std::pair<GameObject*, float>(go, 0.0f));

				go->Physics()->SetLinearVelocity(go->Physics()->GetLinearVelocity() * (1.0f - m_LastAlpha));
				go->Physics()->SetAngularVelocity(go->Physics()->GetAngularVelocity() * (1.0f - m_LastAlpha));
			} else {
				matchingPenetratingRecord->second += deltaTime;
				if (matchingPenetratingRecord->second > 0.0f) {
					float multiplyer = m_LastAlpha + 0.01f;
					float pointsF = std::floor(matchingPenetratingRecord->second * OVERLAP_POINTS_PER_SECOND * multiplyer);
					m_Score += static_cast<unsigned int>(pointsF);
					matchingPenetratingRecord->second -= (pointsF / OVERLAP_POINTS_PER_SECOND / multiplyer);
				}
			}
		} else {
			if (matchingPenetratingRecord != m_PenetratingObjects.end()) {
				m_GameOver = true;
				m_Score = 0;
			}
			if (previouslyColliding == m_CollidingObjects.end()) {
				m_Score += COLLISION_POINTS;
				m_CollidingObjects.push_back(go);
			}
		}
	}
	m_ScoringObjects[readingIndex].clear();
}