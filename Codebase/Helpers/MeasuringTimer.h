#pragma once

#include "Timer.h"

#include "common.h"

#include <cassert>
#include <climits>
#include <iostream>
#include <string>

#ifdef _DEBUG
#define MEASURING_TIMER_LOG_START(str) MeasuringTimer::Instance.LogStart(str)
#define MEASURING_TIMER_LOG_END() MeasuringTimer::Instance.LogEnd()
#define MEASURING_TIMER_PRINT(stream) stream << MeasuringTimer::Instance
#define MEASURING_TIMER_CLEAR() MeasuringTimer::Instance.Clear()
#else
#define MEASURING_TIMER_LOG_START(str)  ((void)0)
#define MEASURING_TIMER_LOG_END() ((void)0)
#define MEASURING_TIMER_PRINT(stream) ((void)0)
#define MEASURING_TIMER_CLEAR() ((void)0)
#endif // _DEBUG


/// @ingroup Helpers
/// <summary>
/// Class for measuring logged time points.
/// </summary>
class MeasuringTimer : public Timer
{
private:
	/// <summary>
	/// Structure representing a point in time for a beginning or end of a period.
	/// </summary>
	struct record
	{
		/// <summary>
		/// Representation of a point in time.
		/// </summary>
		timestamp timestamp;
		/// <summary>
		/// Message associated with the time measurement.
		/// </summary>
		std::string text;
		/// <summary>
		/// Indicates the start or end of a period measurement.
		/// </summary>
		bool isStart;
	};

	/// <summary>
	/// Heaped array of <see cref="MeasuringTimer::record"/>, representing all possible time measurements.
	/// </summary>
	/// <remarks>
	/// The size of this heaped array is twice the size given to the <see cref="MeasuringTimer::Timer(unsigned int)"/> constructor,
	/// for logging start and end timestamps and producing a given number of measurable periods.
	/// </remarks>
	record* m_logs;

	/// <summary>
	/// Index of the end of currently used space in <see cref="MeasuringTimer::m_logs"/>.
	/// </summary>
	unsigned int m_endIndex;

public:
	/// <summary>
	/// A static instance of <see cref="MeasuringTimer"/> class.
	/// </summary>
	static MeasuringTimer Instance;

	/// <summary>
	/// Max number of logs that can be taken.
	/// </summary>
	const unsigned int maxSize;

	/// <summary>
	/// String used for indentation when formatting the output.
	/// </summary>
	std::string indentation;

	/// <summary>
	/// String used for line ends when formatting the output.
	/// </summary>
	std::string endLine;

	/// <summary>
	/// Logs a timestamp for the start of a new period.
	/// </summary>
	/// <param name="msg">Message to associate with the new time period.</param>
	/// <returns>Index representation of where the record is placed internally.</returns>
	inline void LogStart(const char* msg = "")
	{
		assert(("MeasuringTimer::logStart overran the max size", m_endIndex < maxSize));
		m_logs[m_endIndex].isStart = true;
		m_logs[m_endIndex].text.assign(msg);
		m_logs[m_endIndex++].timestamp = Timer::Now();
	}

	/// <summary>
	/// Logs a timestamp for the end of the last period.
	/// </summary>
	/// <param name="msg">Message to associate with the end of a time period.</param>
	inline void LogEnd(const char* msg = "")
	{
		assert(("MeasuringTimer::logEnd overran the max size", m_endIndex < maxSize));
		m_logs[m_endIndex].timestamp = Timer::Now();
		m_logs[m_endIndex].isStart = false;
		m_logs[m_endIndex++].text.assign(msg);
	}

	/// <summary>
	/// Logs a timestamp for the start of a new period.
	/// </summary>
	/// <param name="msg">Message to associate with the new time period.</param>
	/// <returns>Index representation of where the record is placed internally.</returns>
	inline void LogStart(const std::string& msg)
	{
		LogStart(msg.c_str());
	}

	/// <summary>
	/// Logs a timestamp for the end of the last period.
	/// </summary>
	/// <param name="msg">Message to associate with the end of a time period.</param>
	inline void LogEnd(const std::string& msg)
	{
		LogEnd(msg.c_str());
	}

	/// <summary>
	/// Returns the <see cref="MeasuringTimer::record"/>, identified internally by the given index.
	/// </summary>
	/// <param name="index">Index representation of where the record is placed internally.</param>
	/// <returns>reference to a <see cref="MeasuringTimer::record"/>.</returns>
	inline const record& Get(unsigned int index) const
	{
		assert(("MeasuringTimer::get went out of bounds", index < m_endIndex));
		return m_logs[index];
	}

	/// <summary>
	/// Prints time periods starting at given nested level.
	/// </summary>
	/// <param name="os">Output stream.</param>
	/// <param name="startLevel">The lowest index level of period nesting to output measurements for.</param>
	/// <param name="maxDepth">The max depth of child periods to output.</param>
	/// <param name="timeResolution">Time period resolution, 1 = seconds and 1000000 = microseconds.</param>
	/// <remarks>
	///	If parameter maxDepth is 0, only the matched start level will be printed.
	/// </remarks>
	void Print(std::ostream& os, unsigned int startLevel, unsigned int maxDepth = UINT_MAX, float timeResolution = 1000.0f) const;

	/// <summary>
	/// Resets the used logging space to indicate an empty collection of logs.
	/// </summary>
	inline void Clear()
	{
		m_endIndex = 0;
	}

	/// <summary>
	/// Creates a timer for measuring duration of time periods.
	/// </summary>
	/// <param name="maxLogs">Max number of logs that can be made.</param>
	/// <remarks>
	/// Parameter maxLogs has to be at least two times the number of time periods that will be recorded.
	/// </remarks>
	inline MeasuringTimer(unsigned int maxLogs): indentation("-\t"), endLine(LINE_SEPARATOR), m_logs(new record[maxLogs]), m_endIndex(0), maxSize(maxLogs) {}

	/// <summary>
	/// Creates a copy of a timer for measuring duration of time periods.
	/// </summary>
	/// <param name="obj">Original timer.</param>
	inline MeasuringTimer(const MeasuringTimer& obj): indentation(obj.indentation), endLine(obj.endLine), m_logs(new record[obj.maxSize]), m_endIndex(obj.m_endIndex), maxSize(obj.maxSize)
	{
		std::memcpy(m_logs, obj.m_logs, obj.m_endIndex * sizeof(record));
	}

	/// <summary>
	/// Creates a timer for measuring duration of time periods, using the existing resources of given object.
	/// </summary>
	/// <param name="obj">Original timer.</param>
	inline MeasuringTimer(MeasuringTimer&& obj): indentation(obj.indentation), endLine(obj.endLine), m_logs(nullptr), m_endIndex(obj.m_endIndex), maxSize(obj.maxSize)
	{
		std::swap(m_logs, obj.m_logs);
	}

	/// <summary>
	/// Destructor.
	/// </summary>
	inline ~MeasuringTimer()
	{
		if(m_logs)
		{
			delete[] m_logs;
			m_logs = nullptr;
		}
	}


	MeasuringTimer& operator=(const MeasuringTimer& other);

	friend std::ostream& operator<<(std::ostream& os, const MeasuringTimer& obj);
};
