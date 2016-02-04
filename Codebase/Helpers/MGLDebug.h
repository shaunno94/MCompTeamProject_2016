#pragma once
#include "stdafx.h"

#include "MGLUtil.h"

class MGLLog : public MGLSingleton < MGLLog > {
	friend class MGLSingleton < MGLLog >;
public:
	// Write current log to .txt
	void WriteToFile(const std::string fileName, const GLuint log,
		const GLboolean truncate = GL_FALSE);
	// Add string to log file, follows same rules as printf
	void AddLog(const GLuint log, const GLboolean timeStamp, const std::string line, ...);
	// Delete strings in current log (not the .txt file)
	void Flush(const GLuint log = 1);

	// Disables logging to particular log
	void Disable(const GLuint log) { m_enabled->at(log) = 0; }
	// Enables logging to particular log
	void Enable(const GLuint log) { m_enabled->at(log) = 1; }

	MGLvecs* GetLog(const GLuint index) const { return m_logs->at(index); }
protected:
	MGLLog();
	~MGLLog();

	/****** Data ******/

	std::vector<MGLvecs*>* m_logs;
	MGLvecu* m_enabled;

	//MGLvecs* m_mainLog;
	//MGLvecs* m_errorLog;

	//MGLvecs* m_screenLog; TODO
};

