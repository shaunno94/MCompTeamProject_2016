#ifdef ORBIS
#include "PS4Shader.h"

#include <iostream>
#include <fstream>
#include <gnmx\shader_parser.h>
#include <..\samples\sample_code\graphics\api_gnm\toolkit\shader_loader.h>

using std::ifstream;

PS4Shader::PS4Shader(const std::string& vertex, const std::string& pixel, const std::string& geometry, const std::string& hull, const std::string& domain)
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
	m_InverseModelMatrixLocation = GetResourceByName("invModelMat");
	for (size_t i = 0; i < ReservedMeshTextures.size; ++i)
		m_ReservedMeshTextureLocations[i] = GetResourceByName(ReservedMeshTextures.values[i].name);

	for (size_t i = 0; i < ReservedMeshColours.size; ++i)
		m_ReservedMeshColourLocations[i] = GetResourceByName(ReservedMeshColours.values[i].name);
}

PS4Shader::~PS4Shader()
{
}

void PS4Shader::GenerateVertexShader(const std::string& name, bool makeFetch)
{
	char*	binData = nullptr;	//resulting compiled shader bytes
	int		binSize = 0;
	sce::Gnmx::ShaderInfo shaderInfo;

	if (ShaderIsBinary(name)) 
	{
		if (LoadShaderBinary(name, binData, binSize)) 
		{
			vertexBinary.loadFromMemory(binData, binSize);

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
		std::cout << "Vertex Shader is not binary: " << name << std::endl;
		return;
	}

	sce::Gnm::registerResource(nullptr, ownerHandle, vertexShader->getBaseAddress(),
		shaderInfo.m_gpuShaderCodeSize, name.c_str(), sce::Gnm::kResourceTypeShaderBaseAddress, 0);

	sce::Gnmx::generateInputOffsetsCache(&vertexCache, sce::Gnm::kShaderStageVs, vertexShader);

	if (makeFetch) 
	{
		GenerateFetchShader(binData);
	}
	operational = true;
	//delete binData;//done with bindata now!
}

void PS4Shader::GenerateFetchShader(char* binData) 
{
	fetchShader = garlicAllocator.allocate(sce::Gnmx::Toolkit::calculateMemoryRequiredForVsFetchShader(binData));

	uint32_t shaderModifier;
	sce::Gnmx::generateVsFetchShader(fetchShader, &shaderModifier, vertexShader, NULL);

	vertexShader->applyFetchShaderModifier(shaderModifier);
}

void PS4Shader::GeneratePixelShader(const std::string& name)
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

	sce::Gnmx::generateInputOffsetsCache(&pixelCache, sce::Gnm::kShaderStagePs, pixelShader);
	operational = true;
	//delete binData;
}

void PS4Shader::GenerateHullShader(const std::string& name)
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

	sce::Gnmx::generateInputOffsetsCache(&hullCache, sce::Gnm::kShaderStageHs, hullShader);
	operational = true;
}

void PS4Shader::GenerateDomainShader(const std::string& name)
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

	sce::Gnmx::generateInputOffsetsCache(&domainCache, sce::Gnm::kShaderStageEs, domainShader);
	operational = true;
}

void PS4Shader::GenerateGeometryShader(const std::string& name)
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

	sce::Gnmx::generateInputOffsetsCache(&geometryCache, sce::Gnm::kShaderStageGs, geometryShader);
	operational = true;
}

bool PS4Shader::LoadShaderBinary(const std::string& name, char*& into, int& dataSize)
{
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

bool PS4Shader::ShaderIsBinary(const std::string& name)
{
	if (name.length() >= 3 &&
		name[name.length() - 3] == '.' &&
		name[name.length() - 2] == 's' &&
	    name[name.length() - 1] == 'b')
	{
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

shaderResourceLocation	PS4Shader::GetResourceByName(const std::string& name) const
{
	sce::Shader::Binary::Buffer* constantBuffer = vertexBinary.getBufferResourceByName(name.c_str());
	sce::Shader::Binary::Buffer* constantBufferA = pixelBinary.getBufferResourceByName(name.c_str());

	shaderResourceLocation result;
	if (constantBuffer)
	{
		result.id = constantBuffer->m_resourceIndex;
		result.stage = sce::Gnm::ShaderStage::kShaderStageVs;
		return result;
	}
	else if (constantBufferA)
	{
		result.id = constantBufferA->m_resourceIndex;
		result.stage = sce::Gnm::ShaderStage::kShaderStagePs;
		return result;
	}

	result.id =-1;
	result.stage = sce::Gnm::ShaderStage::kShaderStageVs;
	return result;
}
#endif