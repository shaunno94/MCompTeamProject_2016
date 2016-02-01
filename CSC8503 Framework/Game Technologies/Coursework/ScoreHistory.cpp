#include "ScoreHistory.h"
#include <fstream>
#include <algorithm>
#include "Helpers/common.h"
#include <sstream>
#include <iomanip>

ScoreHistory::ScoreHistory(void) {
	std::ifstream file(FileName);

	if (file) {
		std::string name;
		unsigned int score;
		unsigned int counter = 0;
		file >> std::ws;
		while (!file.eof() && counter < MaxRecords) {
			file >> name >> std::ws;
			if (file.eof())
				score = 0;
			else
				file >> score >> std::ws;

			HighScores[counter].name = name;
			HighScores[counter++].score = score;
		}

		while (counter < MaxRecords)
			HighScores[counter++] = Empty;

		std::sort(HighScores, HighScores + MaxRecords, &ScoreHistory::SortFunc);
	} else {
		for (int i = 0; i < MaxRecords; ++i)
			HighScores[i] = Empty;
	}
}


ScoreHistory::~ScoreHistory(void) {
}

const std::string ScoreHistory::FileName = "HighScores.txt";
const ScoreHistory::HighScoreRecord ScoreHistory::Empty = {"__________", 0};

std::string ScoreHistory::HighScoreRecord::ToString() const {
	std::stringstream stream;
	stream << std::setw(MaxNameLength) << name.substr(0, MaxNameLength)
	       << ": " << std::setw(6) << score;
	return stream.str();
}


bool ScoreHistory::IsHeighScore(unsigned int score) {
	for (unsigned int i = 0; i < MaxRecords; ++i) {
		if (score > HighScores[i].score)
			return true;
	}
	return false;
}
bool ScoreHistory::AddScore(const std::string& name, unsigned int score) {
	if (HighScores[MaxRecords - 1].score < score) {
		HighScores[MaxRecords - 1].name = name.length() == 0 ? "_" : name;
		HighScores[MaxRecords - 1].score = score;

		std::sort(std::begin(HighScores), std::end(HighScores), &SortFunc);
		return true;
	}
	return false;
}
bool ScoreHistory::Save() {
	std::ofstream file(FileName);

	if (file) {
		for (unsigned int i = 0; i < MaxRecords; ++i) {
			file << HighScores[i].name
			     << " "
			     << HighScores[i].score
			     << LINE_SEPARATOR;
		}
		file.close();
		return true;
	}

	return false;
}