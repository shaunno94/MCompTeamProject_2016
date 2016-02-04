#pragma once
#include "stdafx.h"

// MGLException_EXCEPTYPE

/*
*Most* MGLExceptions have a Test(PARAMS) function that
will throw the error if those params are true to the error name.

e.g.
MGLException_IsEqual::Test(1,0) // WONT throw
MGLException_IsEqual::Test(25,25) // throws
MGLException_IsZero::Test(12) // WONT throw
MGLException_IsZero::Test(0) // throws
*/

// Standard exception
class MGLException : public std::runtime_error {
public:
	MGLException(const std::string str)
		: std::runtime_error("MGLEXCEPTION ERROR - " + str + " "){}

	virtual const char* what() const throw() {
		return std::runtime_error::what();
	}
};

/******	Common exceptions ******/

// Throws if file has error on open
class MGLException_FileError : public MGLException {
public:
	MGLException_FileError(std::string file)
		: MGLException("FILE ERROR : " + file + " ") {}

	virtual const char* what() const throw() {
		return MGLException::what();
	}

	static void Test(const GLint success, std::string file) {
		if (success != 1) {
			throw MGLException_FileError(file);
		}
	}
};

// Throws if file size is smaller than minSize
class MGLException_FileSTooSmall : public MGLException {
public:
	MGLException_FileSTooSmall(const std::string file)
		: MGLException("FILE TOO SMALL : " + file + " ") {}

	virtual const char* what() const throw() {
		return MGLException::what();
	}

	static void Test(const std::string fileName, std::ifstream& file, const GLuint size) {
		std::ios_base::seekdir pos = (std::ios_base::seekdir)file.tellg();

		file.seekg(0, std::ios_base::end);
		std::size_t fileSize = (size_t)file.tellg();
		file.seekg(0, pos);

		if (fileSize < size) {
			throw MGLException_FileSTooSmall(fileName);
		}
	}
};

// Throws if pointer given is nullptr
class MGLException_Null : public MGLException {
public:
	MGLException_Null()
		: MGLException("POINTER NULL : ") {}

	virtual const char* what() const throw() {
		return MGLException::what();
	}

	static void Test(const void* success) {
		if (success == nullptr) {
			throw MGLException_Null();
		}
	}
};

// Throws is pointers should/should not be equal
class MGLException_Mismatch : public MGLException {
public:
	MGLException_Mismatch()
		: MGLException("POINTER MISMATCH : ") {}

	virtual const char* what() const throw() {
		return MGLException::what();
	}

	static void Test(const void* query, const void* match, const GLboolean shouldMatch) {
		GLboolean theyMatch = (query == match);
		if ((!theyMatch && shouldMatch) || (theyMatch && !shouldMatch)) {
			throw MGLException_Mismatch();
		}
	}
};

// Throws if given number is zero
class MGLException_IsZero : public MGLException {
public:
	MGLException_IsZero()
		: MGLException("IS ZERO : ") {}

	virtual const char* what() const throw() {
		return MGLException::what();
	}

	template<typename T>
	static void Test(const T number) {
		if (number == 0) {
			throw MGLException_IsZero();
		}
	}
};

// Throws if given number is NOT zero
class MGLException_IsNotZero : public MGLException {
public:
	MGLException_IsNotZero()
		: MGLException("NOT ZERO : ") {}

	virtual const char* what() const throw() {
		return MGLException::what();
	}

	template<typename T>
	static void Test(const T number) {
		if (number != 0) {
			throw MGLException_IsNotZero();
		}
	}
};

// Throws if number is less than size
class MGLException_IsLessThan : public MGLException {
public:
	template<typename T, typename N>
	MGLException_IsLessThan(const T num, const N size)
		: MGLException("LESS THAN : " + std::to_string(num) + " < " + std::to_string(size)) {}

	virtual const char* what() const throw() {
		return MGLException::what();
	}

	template<typename T, typename N>
	static void Test(const T num, const N size) {
		if (num < size) {
			throw MGLException_IsLessThan(num, size);
		}
	}
};

// Throws if num1 != num2
class MGLException_IsNotEqual : public MGLException {
public:
	template<typename T, typename N>
	MGLException_IsNotEqual(const T num1, const N num2)
		: MGLException("NOT EQUAL : " + std::to_string(num1) + " " + std::to_string(num2)) {}

	virtual const char* what() const throw() {
		return MGLException::what();
	}

	template<typename T, typename N>
	static void Test(const T num1, const N num2) {
		if (num1 != num2) {
			throw MGLException_IsNotEqual(num1, num2);
		}
	}
};

// Throws if num1 == num2
class MGLException_IsEqual : public MGLException {
public:
	template<typename T, typename N>
	MGLException_IsEqual(const T num1, const N num2)
		: MGLException("EQUAL : " + std::to_string(num1) + " " + std::to_string(num2)) {}

	virtual const char* what() const throw() {
		return MGLException::what();
	}

	template<typename T, typename N>
	static void Test(const T num1, const N num2) {
		if (num1 == num2) {
			throw MGLException_IsEqual(num1, num2);
		}
	}
};

// Throws is the file given does not have the correct file extension
class MGLException_File_FileType : public MGLException {
public:
	MGLException_File_FileType(const std::string fileName, const std::string fileType)
		: MGLException("FILE TYPE : EXPECTING TYPE " + fileType + " GIVEN " + fileName + " ") {}

	virtual const char* what() const throw() {
		return MGLException::what();
	}

	static void Test(const std::string fileName, const std::string fileType) {
		if (fileName.length() < fileType.length()) {
			throw MGLException_File_FileType(fileName, fileType);
		}
		std::string ext = fileName.substr(fileName.length() - fileType.length(), fileName.length());
		if (ext != fileType) {
			throw MGLException_File_FileType(fileName, fileType);
		}
	}
};

/******	Init exceptions ******/

// Throws if theres a problem with glfwInit() in MGLContext
class MGLException_Init_GLFW : public MGLException {
public:
	MGLException_Init_GLFW()
		: MGLException("INIT : GLFW : "){}

	static void Test(const GLint success) {
		if (success != GL_TRUE) {
			throw MGLException_Init_GLFW();
		}
	}
};

// Throws if theres a problem with glewInit from MGLContext::InitGL
class MGLException_Init_GLEW : public MGLException {
public:
	MGLException_Init_GLEW()
		: MGLException("INIT : GLEW "){}

	static void Test(const GLint success) {
		if (success != GLEW_OK) {
			throw MGLException_Init_GLEW();
		}
	}
};

/****** MGLShader exceptions ******/

// Throws if there was a linking error with shader
class MGLException_Shader_LINK : public MGLException {
public:
	MGLException_Shader_LINK()
		: MGLException("SHADER : LINK ") {}

	virtual const char* what() const throw() {
		return MGLException::what();
	}

	static void Test(const GLint success) {
		if (success != GL_TRUE) {
			throw MGLException_Shader_LINK();
		}
	}
};

// Throws is there is a shader compile error
class MGLException_Shader_COMPILE : public MGLException {
public:
	MGLException_Shader_COMPILE()
		: MGLException("SHADER : COMPILE ") {}

	virtual const char* what() const throw() {
		return MGLException::what();
	}

	static void Test(const GLint success) {
		if (success != GL_TRUE) {
			throw MGLException_Shader_COMPILE();
		}
	}
};

