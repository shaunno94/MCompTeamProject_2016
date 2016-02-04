#include "stdafx.h"
#include "MGLDebug.h"

#include "MGLExceptions.h"

/******************************/
/*********** MGLLog ***********/
/******************************/

MGLLog::MGLLog() {
	m_logs = new std::vector<MGLvecs*>(MGL_LOG_AMOUNT);
	m_enabled = new MGLvecu(MGL_LOG_AMOUNT);

	for (GLuint i = 0; i < MGL_LOG_AMOUNT; ++i) {
		m_logs->at(i) = new MGLvecs;
		m_logs->at(i)->reserve(MGL_LOG_MAXLOGSIZE);

		m_enabled->at(i) = 1;
	}
}

MGLLog::~MGLLog() {
	for (MGLvecs* vec : *m_logs) {
		delete vec;
	}

	delete m_enabled;
	delete m_logs;
}

void MGLLog::WriteToFile(const std::string fileName, const GLuint log, const GLboolean truncate) {
	std::ofstream out(fileName, truncate ? std::ios::out | std::ios::trunc : std::ios::out | std::ios::app);

	// did it open successfully
	try {
		MGLException_FileError::Test(out.is_open(), fileName);
	}
	catch (MGLException& e) {
		std::cerr << e.what() << "MGLLOG WRTIE ERROR" << std::endl;
		return;
	}

	// write intro string
	std::string intro = "******************************************\n";
	intro += "*************** New MGLLog ***************\n";
	intro += "******************************************\n";
	out.write((char *)&intro[0], intro.size());

	for (const std::string& str : *m_logs->at(log)) {
		out.write((char *)&str[0], str.size());
		out.write("\n", 1);
	}
}

void MGLLog::AddLog(const GLuint log, const GLboolean timeStamp, const std::string line, ...) {
	if (!m_enabled->at(log))
		return;

	// get arguments
	va_list args;
	va_start(args, line);

	// create char buffer based on inputs
	GLchar buffer[MGL_LOG_MAXLINESIZE];
	GLint needed = vsnprintf_s(buffer, MGL_LOG_MAXLINESIZE - 1, _TRUNCATE, line.c_str(), args);
	va_end(args);

	// determine buffer length and possibly truncate size
	GLint length = (needed < 0) ? MGL_LOG_MAXLINESIZE : needed;

	// create timestamp
	std::string text = "";

	// dirty dirty timestamps
	if (timeStamp) {
		std::time_t t = std::time(nullptr);
		struct tm now;
		localtime_s(&now, &t);
		text = "[" + std::to_string(now.tm_mday);
		text += "-" + std::to_string(now.tm_mon + 1);
		text += "-" + std::to_string(now.tm_year + 1900);
		text += " " + std::to_string(now.tm_hour);
		text += ":" + std::to_string(now.tm_min);
		text += ":" + std::to_string(now.tm_sec);
		text += "] ";
	}

	text += std::string(buffer, (size_t)length);

	m_logs->at(log)->push_back(text);
}

void MGLLog::Flush(const GLuint log) {
	m_logs->at(log)->clear();
}