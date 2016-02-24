#ifdef ORBIS
#include "PS4Shader.h"

#include <iostream>
#include <fstream>
#include <gnmx\shader_parser.h>
#include <..\samples\sample_code\graphics\api_gnm\toolkit\shader_loader.h>

using std::ifstream;

PS4Shader::PS4Shader(const string& vertex, const string& pixel, const string& geometry, const string& hull, const string& domain)
{
	fetchShader = nullptr;
	vertexShader = nullptr;
	pixelShader = nullptr;
	geometryShader = nullptr;
	hullShader = nullptr;

	GenerateVertexShader(vertex, true);
	GeneratePixelShader(pixel);

	if (!geometry.empty())
	{
		GenerateGeometryShader(geometry);
	}
	if (!hull.empty() && !domain.empty())
	{
		GenerateHullShader(hull);
		GenerateDomainShader(domain);
	}

	m_ModelMatrixLocation = GetResourceByName("modelMatrix");
	for (size_t i = 0; i < ReservedMeshTextures.size; ++i)
		m_ReservedMeshTextureLocations[i] = GetResourceByName(ReservedMeshTextures.values[i].name);

	for (size_t i = 0; i < ReservedMeshColours.size; ++i)
		m_ReservedMeshColourLocations[i] = GetResourceByName(ReservedMeshColours.values[i].name);
}


PS4Shader::~PS4Shader()
{
}

void PS4Shader::GenerateVertexShader(const string&name, bool makeFetch) {
	char*	binData = nullptr;	//resulting compiled shader bytes
	int		binSize = 0;
	sce::Gnmx::ShaderInfo shaderInfo;

	if (ShaderIsBinary(name)) 
	{
		if (LoadShaderBinary(name, binData, binSize)) 
		{
			vertexBinary.loadFromMemory(binData, binSize);

			//sce::Shader::Binary::Buffer* constantBuffer = vertexBinary.getBufferResourceByName("LOLBUFFER");

			sce::Gnmx::parseShader(&shaderInfo, binData);

			void* shaderBinary = garlicAllocator.allocate(shaderInfo.m_gpuShaderCodeSize, sce::Gnm::kAlignmentOfShaderInBytes);
			void* shaderHeader = onionAllocator.allocate(shaderInfo.m_vsShader->computeSize(), sce::Gnm::kAlignmentOfBufferInBytes);

			memcpy(shaderBinary, shaderInfo.m_gpuShaderCode, shaderInfo.m_gpuShaderCodeSize);
			memcpy(shaderHeader, shaderInfo.m_vsShader, shaderInfo.m_vsShader->computeSize());

			vertexShader = (sce::Gnmx::VsShader*)shaderHeader;
			vertexShader->patchShaderGpuAddress(shaderBinary);
		}
		else 
		{
			std::cout << "Failed to generate vertex shader: " << name << " from binary. " << std::endl;
			operational = false;
			return;
		}
	}
	else 
	{
		string shaderString;
		if (LoadShaderText(name, shaderString)) 
		{
			//shaderString now contains the pssl shader data
			//MAGIC GOES HERE
		}
		else 
		{
			std::cout << "Failed to generate vertex shader: " << name << " from raw text. " << std::endl;
			return;
		}
	}

	sce::Gnm::registerResource(nullptr, ownerHandle, vertexShader->getBaseAddress(),
		shaderInfo.m_gpuShaderCodeSize, name.c_str(), sce::Gnm::kResourceTypeShaderBaseAddress, 0);

	sce::Gnmx::generateInputOffsetsCache(&vertexCache, Gnm::kShaderStageVs, vertexShader);

	if (makeFetch) {
		GenerateFetchShader(binData);
	}
	operational = true;
	//delete binData;//done with bindata now!
}

void PS4Shader::GenerateFetchShader(char* binData) {
	fetchShader = garlicAllocator.allocate(sce::Gnmx::Toolkit::calculateMemoryRequiredForVsFetchShader(binData));

	uint32_t shaderModifier;
	sce::Gnmx::generateVsFetchShader(fetchShader, &shaderModifier, vertexShader, NULL);

	vertexShader->applyFetchShaderModifier(shaderModifier);
}

void PS4Shader::GeneratePixelShader(const string&name) 
{
	char*	binData = nullptr;	//resulting compiled shader bytes
	int		binSize = 0;
	sce::Gnmx::ShaderInfo shaderInfo;

	if (ShaderIsBinary(name))
	{
		if (LoadShaderBinary(name, binData, binSize)) 
		{
			pixelBinary.loadFromMemory(binData, binSize);
			sce::Gnmx::parseShader(&shaderInfo, binData);

			void* shaderBinary = garlicAllocator.allocate(shaderInfo.m_gpuShaderCodeSize, sce::Gnm::kAlignmentOfShaderInBytes);
			void* shaderHeader = onionAllocator.allocate(shaderInfo.m_psShader->computeSize(), sce::Gnm::kAlignmentOfBufferInBytes);

			memcpy(shaderBinary, shaderInfo.m_gpuShaderCode, shaderInfo.m_gpuShaderCodeSize);
			memcpy(shaderHeader, shaderInfo.m_psShader, shaderInfo.m_psShader->computeSize());

			pixelShader = (sce::Gnmx::PsShader*)shaderHeader;
			pixelShader->patchShaderGpuAddress(shaderBinary);
		}
		else
		{
			std::cout << "Failed to generate pixel shader: " << name << " from binary. " << std::endl;
			operational = false;
			return;
		}
	}
	else 
	{
		std::cout << "Pixel Shader is not binary: " << name << std::endl;
		return;
	}

	sce::Gnm::registerResource(nullptr, ownerHandle, vertexShader->getBaseAddress(),
		shaderInfo.m_gpuShaderCodeSize, name.c_str(), sce::Gnm::kResourceTypeShaderBaseAddress, 0);

	sce::Gnmx::generateInputOffsetsCache(&pixelCache, Gnm::kShaderStagePs, pixelShader);
	operational = true;
	//delete binData;
}

void PS4Shader::GenerateHullShader(const string& name)
{
	char*	binData = nullptr;	//resulting compiled shader bytes
	int		binSize = 0;
	sce::Gnmx::ShaderInfo shaderInfo;

	if (ShaderIsBinary(name)) 
	{
		if (LoadShaderBinary(name, binData, binSize)) 
		{
			hullBinary.loadFromMemory(binData, binSize);
			sce::Gnmx::parseShader(&shaderInfo, binData);

			void* shaderBinary = garlicAllocator.allocate(shaderInfo.m_gpuShaderCodeSize, sce::Gnm::kAlignmentOfShaderInBytes);
			void* shaderHeader = onionAllocator.allocate(shaderInfo.m_psShader->computeSize(), sce::Gnm::kAlignmentOfBufferInBytes);

			memcpy(shaderBinary, shaderInfo.m_gpuShaderCode, shaderInfo.m_gpuShaderCodeSize);
			memcpy(shaderHeader, shaderInfo.m_psShader, shaderInfo.m_psShader->computeSize());

			hullShader = (sce::Gnmx::HsShader*)shaderHeader;
			hullShader->patchShaderGpuAddress(shaderBinary);
		}
	}
	else 
	{
		std::cout << "Hull Shader is not binary: " << name << std::endl;
		operational = false;
		return;
	}

	sce::Gnm::registerResource(nullptr, ownerHandle, vertexShader->getBaseAddress(),
		shaderInfo.m_gpuShaderCodeSize, name.c_str(), sce::Gnm::kResourceTypeShaderBaseAddress, 0);

	sce::Gnmx::generateInputOffsetsCache(&hullCache, Gnm::kShaderStageHs, hullShader);
	operational = true;
}

void PS4Shader::GenerateDomainShader(const string& name)
{
	char*	binData = nullptr;	//resulting compiled shader bytes
	int		binSize = 0;
	sce::Gnmx::ShaderInfo shaderInfo;

	if (ShaderIsBinary(name)) 
	{
		if (LoadShaderBinary(name, binData, binSize)) 
		{
			domainBinary.loadFromMemory(binData, binSize);
			sce::Gnmx::parseShader(&shaderInfo, binData);

			void* shaderBinary = garlicAllocator.allocate(shaderInfo.m_gpuShaderCodeSize, sce::Gnm::kAlignmentOfShaderInBytes);
			void* shaderHeader = onionAllocator.allocate(shaderInfo.m_psShader->computeSize(), sce::Gnm::kAlignmentOfBufferInBytes);

			memcpy(shaderBinary, shaderInfo.m_gpuShaderCode, shaderInfo.m_gpuShaderCodeSize);
			memcpy(shaderHeader, shaderInfo.m_psShader, shaderInfo.m_psShader->computeSize());

			domainShader = (sce::Gnmx::EsShader*)shaderHeader;
			domainShader->patchShaderGpuAddress(shaderBinary);
		}
	}
	else 
	{
		std::cout << "Domain Shader is not binary: " << name << std::endl;
		operational = false;
		return;
	}

	sce::Gnm::registerResource(nullptr, ownerHandle, vertexShader->getBaseAddress(),
		shaderInfo.m_gpuShaderCodeSize, name.c_str(), sce::Gnm::kResourceTypeShaderBaseAddress, 0);

	sce::Gnmx::generateInputOffsetsCache(&domainCache, Gnm::kShaderStageEs, domainShader);
	operational = true;
}

void PS4Shader::GenerateGeometryShader(const string& name)
{
	char*	binData = nullptr;	//resulting compiled shader bytes
	int		binSize = 0;
	sce::Gnmx::ShaderInfo shaderInfo;

	if (ShaderIsBinary(name))
	{
		if (LoadShaderBinary(name, binData, binSize))
		{
			geometryBinary.loadFromMemory(binData, binSize);
			sce::Gnmx::parseShader(&shaderInfo, binData);

			void* shaderBinary = garlicAllocator.allocate(shaderInfo.m_gpuShaderCodeSize, sce::Gnm::kAlignmentOfShaderInBytes);
			void* shaderHeader = onionAllocator.allocate(shaderInfo.m_psShader->computeSize(), sce::Gnm::kAlignmentOfBufferInBytes);

			memcpy(shaderBinary, shaderInfo.m_gpuShaderCode, shaderInfo.m_gpuShaderCodeSize);
			memcpy(shaderHeader, shaderInfo.m_psShader, shaderInfo.m_psShader->computeSize());

			geometryShader = (sce::Gnmx::GsShader*)shaderHeader;
			//geometryShader->patchShaderGpuAddress(shaderBinary); ??? GsShader does not have this function
		}
	}
	else
	{
		std::cout << "Geometry Shader is not binary: " << name << std::endl;
		operational = false;
		return;
	}

	sce::Gnm::registerResource(nullptr, ownerHandle, vertexShader->getBaseAddress(),
		shaderInfo.m_gpuShaderCodeSize, name.c_str(), sce::Gnm::kResourceTypeShaderBaseAddress, 0);

	sce::Gnmx::generateInputOffsetsCache(&geometryCache, Gnm::kShaderStageGs, geometryShader);
	operational = true;
}

bool PS4Shader::LoadShaderText(const string &name, string&into) {

	return false;
}

bool PS4Shader::LoadShaderBinary(const string &name, char*& into, int& dataSize) {
	std::ifstream binFile(name, std::ios::binary);
	if (!binFile)
		return false;

	binFile.seekg(0, ios::end);
	int size = binFile.tellg();
	into = new char[size];

	binFile.seekg(0, ios::beg);
	binFile.read(into, size);

	dataSize = size;
	binFile.close();
	return true;
}

bool PS4Shader::ShaderIsBinary(const string& name) {
	if (name.length() >= 3 &&
		name[name.length() - 3] == '.' &&
		name[name.length() - 2] == 's' &&
		name[name.length() - 1] == 'b') {
		return true;
	}
	return false;
}

void	PS4Shader::SubmitShaderSwitch(sce::Gnmx::GnmxGfxContext& cmdList) 
{
	cmdList.setActiveShaderStages(sce::Gnm::kActiveShaderStagesVsPs);
	cmdList.setVsShader(vertexShader, 0, fetchShader, &vertexCache);
	cmdList.setPsShader(pixelShader, &pixelCache);
}

int	PS4Shader::GetResourceByName(const string &name) const
{
	sce::Shader::Binary::Buffer* constantBuffer = vertexBinary.getBufferResourceByName(name.c_str());
	if (!constantBuffer) 
	{
		return -1;
	}
	return constantBuffer->m_resourceIndex;
}
#endif