#ifndef ORBIS
#include "OGLShader.h"

OGLShader::OGLShader(std::string vFile, std::string fFile, std::string gFile, std::string tcsFile, std::string tesFile)
{
	program		= glCreateProgram();
	objects[SHADER_VERTEX]		= GenerateShader(vFile	 , GL_VERTEX_SHADER);
	objects[SHADER_FRAGMENT]	= GenerateShader(fFile, GL_FRAGMENT_SHADER);

	operational = objects[SHADER_VERTEX] > 0 && objects[SHADER_FRAGMENT] > 0;

	objects[SHADER_GEOMETRY]	= 0;
	objects[SHADER_TCS]			= 0;
	objects[SHADER_TES]			= 0;

	if (!gFile.empty())
	{
		objects[SHADER_GEOMETRY]	= GenerateShader(gFile, GL_GEOMETRY_SHADER);
		operational = operational && objects[SHADER_GEOMETRY] > 0;
		glAttachShader(program, objects[SHADER_GEOMETRY]);
	}
	if (!tcsFile.empty())
	{
		objects[SHADER_TCS]		= GenerateShader(tcsFile, GL_TESS_CONTROL_SHADER);
		operational = operational && objects[SHADER_TCS] > 0;
		glAttachShader(program, objects[SHADER_TCS]);
	}
	if (!tesFile.empty())
	{
		objects[SHADER_TES]		= GenerateShader(tesFile, GL_TESS_EVALUATION_SHADER);
		operational = operational && objects[SHADER_TES] > 0;
		glAttachShader(program, objects[SHADER_TES]);
	}

	glAttachShader(program, objects[SHADER_VERTEX]);
	glAttachShader(program, objects[SHADER_FRAGMENT]);

	if (operational)
	{
		SetDefaultAttributes();
		glLinkProgram(program);
		glGetProgramiv(program, GL_LINK_STATUS, &operational);
		if (operational != GL_TRUE)
		{
			cout << "Linking failed!" << endl;
			char error[512];
			glGetInfoLogARB(program, sizeof(error), NULL, error);
			cout << error;
		}
		else
		{
			m_ModelMatrixLocation = glGetUniformLocation(GetProgram(), "modelMatrix");
			for (size_t i = 0; i < ReservedMeshTextures.size; ++i)
				m_ReservedMeshTextureLocations[i] = glGetUniformLocation(GetProgram(), ReservedMeshTextures.values[i].name);

			for (size_t i = 0; i < ReservedMeshColours.size; ++i)
				m_ReservedMeshColourLocations[i] = glGetUniformLocation(GetProgram(), ReservedMeshColours.values[i].name);
		}
	}
	else
		operational = GL_FALSE;
}

OGLShader::~OGLShader(void)
{
	for (int i = 0; i < 3; ++i)
	{
		glDetachShader(program, objects[i]);
		glDeleteShader(objects[i]);
	}
	glDeleteProgram(program);
}

bool OGLShader::LoadShaderFile(string from, string& into)
{
	ifstream	file;
	string		temp;

	cout << "Loading shader text from " << from << endl << endl;

	file.open(from.c_str());
	if (!file.is_open())
	{
		cout << "File does not exist!" << endl;
		return false;
	}

	while (!file.eof())
	{
		getline(file, temp);
		into += temp + "\n";
	}

	//cout << into << endl << endl;

	file.close();
	cout << "Loaded shader text!" << endl << endl;
	return true;
}

GLuint OGLShader::GenerateShader(string from, GLenum type)
{
	cout << "Compiling Shader..." << endl;

	string load;
	if (!LoadShaderFile(from, load))
	{
		cout << "Compiling failed!" << endl;
		return 0;
	}

	GLuint shader = glCreateShader(type);

	const char* chars = load.c_str();
	glShaderSource(shader, 1, &chars, NULL);
	glCompileShader(shader);

	GLint status;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &status);

	if (status == GL_FALSE)
	{
		cout << "Compiling failed!" << endl;
		char error[512];
		glGetInfoLogARB(shader, sizeof(error), NULL, error);
		cout << error;
		return 0;
	}
	cout << "Compiling success!" << endl << endl;
	return shader;
}

void OGLShader::SetDefaultAttributes()
{
	glBindAttribLocation(program, VERTEX_BUFFER,  "position");
	glBindAttribLocation(program, NORMAL_BUFFER,  "normal");
	glBindAttribLocation(program, TANGENT_BUFFER, "tangent");
	glBindAttribLocation(program, TEXTURE_BUFFER, "texCoord");
	//TODO: hm???
	glBindAttribLocation(program, MAX_BUFFER + 1,  "transformIndex");
}
#endif