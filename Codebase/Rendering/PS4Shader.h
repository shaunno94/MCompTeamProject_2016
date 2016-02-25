#ifdef ORBIS
#pragma once
#include <Memory\PS4Memory.h>
#include <gnmx\shaderbinary.h>
#include <shader\binary.h>
#include <vector>
#include "BaseShader.h"

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
	friend class PS4Renderer;
public:
	PS4Shader(const string& vertex, const string& pixel, const string& geometry = "", const string& hull = "", const string& domain = "");
	~PS4Shader();

	int GetReservedMeshTextureUniformLocation(uint32_t index) const override { return m_ReservedMeshTextureLocations[index]; }
	int GetReservedMeshColourUniformLocation(uint32_t index) const override { return m_ReservedMeshColourLocations[index]; }
	bool IsOperational() const override { return operational; }
	int	GetResourceByName(const std::string &name) const override;
	int GetModelMatrixLocation() const override { return m_ModelMatrixLocation; }

protected:
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

	int m_ModelMatrixLocation;
	int m_ReservedMeshTextureLocations[ReservedMeshTextures.size];
	int m_ReservedMeshColourLocations[ReservedMeshColours.size];
};
#endif
