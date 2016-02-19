#ifdef ORBIS
#pragma once
#include <Memory\PS4Memory.h>
#include <gnmx\shaderbinary.h>
#include <shader\binary.h>


#include <vector>
#include "Shader.h"

using std::vector;
using std::string;

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
class PS4Shader : public PS4Memory, public BaseShader
{
	//friend class PS4RendererBase;
public:
	PS4Shader(const string& vertex, const string& pixel, const string& geometry = "", const string& hull = "", const string& domain = "");
	~PS4Shader();

protected:
	
	//PS4Shader(const string& vertex, const string& pixel);
	void GenerateVertexShader(const string&name, bool makeFetch);

	void GeneratePixelShader(const string&name);

	void GenerateHullShader(const string& name);
	 
	void GenerateDomainShader(const string& name);

	void GenerateGeometryShader(const string& name);

	void GenerateFetchShader(char* binData);

	bool LoadShaderText(const string &name, string&into);

	bool LoadShaderBinary(const string &name, char*& into, int& dataSize);

	bool ShaderIsBinary(const string& name);

	void SubmitShaderSwitch(sce::Gnmx::GnmxGfxContext& cmdList);

	int	GetResourceByName(const std::string &name) const override;

	bool IsOperational() const override { return operational; }
	
	int GetReservedMeshTextureUniformLocation(size_t index) const override {}

protected:
	void*							fetchShader;

	sce::Shader::Binary::Program	vertexBinary;
	sce::Gnmx::VsShader*			vertexShader;
	sce::Gnmx::InputOffsetsCache	vertexCache;

	sce::Shader::Binary::Program	pixelBinary;
	sce::Gnmx::PsShader*			pixelShader;
	sce::Gnmx::InputOffsetsCache	pixelCache;

	sce::Shader::Binary::Program	hullBinary;
	sce::Gnmx::HsShader*			hullShader;
	sce::Gnmx::InputOffsetsCache	hullCache;

	sce::Shader::Binary::Program	domainBinary;
	sce::Gnmx::EsShader*			domainShader;
	sce::Gnmx::InputOffsetsCache	domainCache;

	sce::Shader::Binary::Program	geometryBinary;
	sce::Gnmx::GsShader*			geometryShader;
	sce::Gnmx::InputOffsetsCache	geometryCache;

	//PS4PipelineType pipelineType;
	bool operational = false;
};
#endif
