#include "MeasuringTimer.h"

#include <vector>
#include <stack>


inline MeasuringTimer& MeasuringTimer::operator=(const MeasuringTimer& other)
{
	if(this != &other)
	{
		indentation = other.indentation;
		endLine = other.endLine;
		const_cast<unsigned&>(maxSize) = other.maxSize;
		m_endIndex = other.m_endIndex;

		if(m_logs) delete[] m_logs;
		m_logs = new record[maxSize];
		memcpy(m_logs, other.m_logs, m_endIndex * sizeof(MeasuringTimer::record));
	}
	return *this;
}


void MeasuringTimer::Print(std::ostream& os, unsigned startLevel, unsigned maxDepth, float timeResolution) const
{
	os << "Scale: 1/" << timeResolution << " sec" << LINE_SEPARATOR << LINE_SEPARATOR;
	//collection for calculating average durations per sibling group in a level
	//[level][sibling group][duration]
	std::vector<std::vector<std::vector<float>*>*> siblingDurationsPerLevel;
	std::stack<timestamp> startTimestamps;
	//collection for keeping track of created sibling groups and indicating when to append a new sibling groups to a nesting level
	//[level] (is sibling group open)
	std::vector<bool> siblingGroupOpenOnLevel;
	//have one more level representation than in use, to simplify sibling closing test
	siblingGroupOpenOnLevel.push_back(false);

	//max level to print out based on the depth
	unsigned maxLevel = startLevel + maxDepth;
	if(maxLevel < maxDepth)
		maxLevel = UINT_MAX;

	//variable to store indentation or relative level
	unsigned indentCount;
	//counter to keep track of the current nesting level
	unsigned counter = 0;

	for(unsigned i = 0; i < m_endIndex; ++i)
	{
		if(m_logs[i].isStart)
		{
			if(counter >= startLevel && counter <= maxLevel)   //if in range for printing
			{
				indentCount = counter - startLevel;

				//creating space for new nesting levels
				if(siblingDurationsPerLevel.size() <= indentCount)
				{
					siblingDurationsPerLevel.push_back(new std::vector<std::vector<float>*>());
					siblingDurationsPerLevel.back()->push_back(new std::vector<float>);
					siblingGroupOpenOnLevel.push_back(false);
				}

				for(unsigned j = 0; j < indentCount; ++j)
					os << indentation;

				os << indentCount << '-' << siblingDurationsPerLevel[indentCount]->size() - 1 << " { " << m_logs[i].text << endLine;
				startTimestamps.push(m_logs[i].timestamp);
			}
			++counter; //counting up on start of next periods
		}
		else
		{
			--counter; //counting down on end of periods
			if(counter >= startLevel && counter <= maxLevel)   //if in range for printing
			{
				indentCount = counter - startLevel;
				for(unsigned j = 0; j < indentCount; ++j)
					os << indentation;

				if(siblingGroupOpenOnLevel[indentCount + 1])   //if child sibling group exists
				{
					//appending a new sibling group to use
					//nesting levels will have empty sibling groups at the end from when the last parent group closed and made the append
					siblingDurationsPerLevel[indentCount + 1]->push_back(new std::vector<float>());
					siblingGroupOpenOnLevel[indentCount + 1] = false;
				}

				float duration = (float) Timer::Duration(startTimestamps.top(), m_logs[i].timestamp, timeResolution);
				siblingDurationsPerLevel[indentCount]->back()->push_back(duration);
				siblingGroupOpenOnLevel[indentCount] = true;
				startTimestamps.pop();

				os << "} " << duration << m_logs[i].text << endLine;
			}
			else if((counter + 1) == startLevel && siblingDurationsPerLevel.size())   //if the base nesting level records for printing could have different parents and a sibling group already exists for base level
				siblingDurationsPerLevel[0]->push_back(new std::vector<float>()); //start a separate base sibling group
		}
	}
	//printing averages for each nested level sibling group
	//useful for getting average time periods from multiple executions of a performance test
	if(siblingDurationsPerLevel.size())
	{
		os << LINE_SEPARATOR << "Averages:" << LINE_SEPARATOR;

		unsigned levelCount = siblingDurationsPerLevel.size();
		for(unsigned levelIndex = 0; levelIndex < levelCount; ++levelIndex)
		{
			auto level = siblingDurationsPerLevel[levelIndex];
			unsigned siblingSetCount = level->size();
			for(unsigned siblingSetIndex = 0; siblingSetIndex != siblingSetCount; ++siblingSetIndex)
			{

				auto siblingSet = (*level)[siblingSetIndex];
				unsigned durationCount = siblingSet->size();
				if(durationCount)   //disregarding empty sibling groups at the end of each nesting level
				{
					double sum = 0.0;
					for(unsigned i = 0; i < durationCount; ++i)
						sum += (*siblingSet)[i];

					for(unsigned i = 0; i < levelIndex; ++i)
						os << indentation;

					os << levelIndex << '-' << siblingSetIndex << " : " << sum / durationCount << endLine;
				}
			}
		}
	}
	//cleanup
	siblingDurationsPerLevel;
	for(auto levelIt = siblingDurationsPerLevel.begin(); levelIt != siblingDurationsPerLevel.end() ; ++levelIt)
	{
		auto siblingGroupEnd = (*levelIt)->end();
		for(auto siblingGroupIt = (*levelIt)->begin(); siblingGroupIt != siblingGroupEnd; ++siblingGroupIt)
		{
			delete(*siblingGroupIt);
			(*siblingGroupIt) = nullptr;
		}
		delete(*levelIt);
		(*levelIt) = nullptr;
	}
}


std::ostream& operator<<(std::ostream& os, const MeasuringTimer& obj)
{
	obj.Print(os, 0);
	return os;
}