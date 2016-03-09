#ifdef ORBIS
#pragma once
#include <Memory\PS4Memory.h>
#include <gnmx\shaderbinary.h>
#include <shader\binary.h>
#include <vector>
#include "BaseShader.h"

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
	PS4Shader(const std::string& vertex, const std::string& pixel, const std::string& geometry = "", const std::string& hull = "", const std::string& domain = "");
	~PS4Shader();

	shaderResourceLocation GetReservedMeshTextureUniformLocation(uint32_t index) const override { return m_ReservedMeshTextureLocations[index]; }
	shaderResourceLocation GetReservedMeshColourUniformLocation(uint32_t index) const override { return m_ReservedMeshColourLocations[index]; }
	bool IsOperational() const override { return operational; }
	shaderResourceLocation	GetResourceByName(const std::string &name) const override;
	shaderResourceLocation GetModelMatrixLocation() const override { return m_ModelMatrixLocation; }
	shaderResourceLocation GetInverseModelMatrixLocation() const override { return m_InverseModelMatrixLocation; }

protected:
	void GenerateVertexShader(const std::string&name, bool makeFetch);
	void GeneratePixelShader(const std::string&name);
	void GenerateHullShader(const std::string& name);
	void GenerateDomainShader(const std::string& name);
	void GenerateGeometryShader(const std::string& name);
	void GenerateFetchShader(char* binData);
	
	bool LoadShaderBinary(const std::string &name, char*& into, int& dataSize);
	bool ShaderIsBinary(const std::string& name);
	
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

	shaderResourceLocation m_ModelMatrixLocation;
	shaderResourceLocation m_InverseModelMatrixLocation;
	shaderResourceLocation m_ReservedMeshTextureLocations[ReservedMeshTextures.size];
	shaderResourceLocation m_ReservedMeshColourLocations[ReservedMeshColours.size];
};
#endif
