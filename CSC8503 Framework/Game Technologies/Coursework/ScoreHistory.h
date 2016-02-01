#pragma once

#include <string>
#include <vector>
#include "ncltech/TSingleton.h"

class ScoreHistory : public TSingleton<ScoreHistory> {
 public:
	struct HighScoreRecord {
		std::string name;
		unsigned int score;
		std::string ToString() const;
	};

	ScoreHistory(void);
	~ScoreHistory(void);

	static const unsigned int MaxNameLength = 10;
	static const unsigned int MaxRecords = 5;
	static const std::string FileName;
	static const HighScoreRecord Empty;

	HighScoreRecord HighScores[MaxRecords];

	bool IsHeighScore(unsigned int score);
	bool AddScore(const std::string& name, unsigned int score);
	bool Save();

	static bool SortFunc(const HighScoreRecord& one, const HighScoreRecord& two) {
		if (one.score == two.score)
			return one.name < two.name;
		return one.score > two.score;
	}
};

