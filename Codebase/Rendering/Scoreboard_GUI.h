#include "GUI_Component.h"

class Scoreboard_GUI : public GUI_Component
{
public:
	Scoreboard_GUI(int& score_a, int& score_b, int& timeRem);
	~Scoreboard_GUI();

protected:
	int m_ScoreA;
	int m_ScoreB;
	int m_TimeRem;
};