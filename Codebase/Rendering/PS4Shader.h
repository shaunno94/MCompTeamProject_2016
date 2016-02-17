#ifdef ORBIS
#pragma once
#include <gnmx\shaderbinary.h>
#include <shader\binary.h>
#include <Memory\PS4Memory.h>

#include <vector>
#include <string>

using std::vector;
using std::string;
using namespace sce;

//enum class PS4ShaderType {
//	VERTEX,
//	VERTEXCOPY,
//	PIXEL,
//	GEOMETRY,
//	HULL,
//	DOMAIN,
//	FETCH,
//};
//
//enum PS4PipelineType {
//
//};
class PS4Shader : public PS4Memory
{
	//friend class PS4RendererBase;
public:
	~PS4Shader();

	static PS4Shader* GenerateShader(const string& vertex, const string& pixel);

protected:
	PS4Shader();
	//PS4Shader(const string& vertex, const string& pixel);
	void GenerateVertexShader(const string&name, bool makeFetch);

	void GeneratePixelShader(const string&name);

	void GenerateFetchShader(char* binData);

	bool LoadShaderText(const string &name, string&into);

	bool LoadShaderBinary(const string &name, char*& into, int& dataSize);

	bool ShaderIsBinary(const string& name);

	void	SubmitShaderSwitch(Gnmx::GnmxGfxContext& cmdList);

	int		GetConstantBuffer(const string &name);

protected:
	void*							fetchShader;

	sce::Shader::Binary::Program	vertexBinary;
	Gnmx::VsShader*					vertexShader;
	Gnmx::InputOffsetsCache			vertexCache;

	sce::Shader::Binary::Program	pixelBinary;
	Gnmx::PsShader*					pixelShader;
	Gnmx::InputOffsetsCache			pixelCache;

	//PS4PipelineType pipelineType;
};
#endif
