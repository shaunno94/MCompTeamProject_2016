#pragma once
#include <string>
#include "constants.h"

#ifdef ORBIS
#include <gnmx\shaderbinary.h>
typedef struct PS4ShaderResourceLocations
{
	sce::Gnm::ShaderStage stage;
	int id;
	/*union
	{
		struct
		{
			int stageCs;	///< Compute shader stage.
			int stagePs;	///< Pixel shader stage.
			int stageVs;	///< Vertex shader stage.
			int stageGs;	///< Geometry shader stage.
			int stageEs;	///< Export shader stage.
			int stageHs;	///< Hull shader stage.
			int stageLs;	///< LDS shader stage.
		};
		int arr[sce::Gnm::ShaderStage::kShaderStageCount];
	};*/

	bool eq(const PS4ShaderResourceLocations& other) const
	{
		/*for (int i = 0; i < sce::Gnm::ShaderStage::kShaderStageCount; ++i)
		{
		if (arr[i] != 0 || other.arr[i] != 0)
		{
		return arr[i] == other.arr[i];
		}
		}*/
		return stage == other.stage && id == other.id;
	}

	bool inUse() const
	{
		/*for (int i = 0; i < sce::Gnm::ShaderStage::kShaderStageCount; ++i)
		{
		if (arr[i] >= 0)
		return true;
		}*/
		return id >= 0;
	}
} shaderResourceLocation;

#else
typedef struct OGLShaderResourceLocation
{
	int id;

	bool eq(const OGLShaderResourceLocation& other) const
	{
		return id == other.id;
	}

	bool inUse() const
	{
		return id >= 0;
	}
} shaderResourceLocation;

#endif


class BaseShader
{
public:
	BaseShader() {}
	virtual ~BaseShader() {}

	virtual bool IsOperational() const = 0;
	virtual shaderResourceLocation GetResourceByName(const std::string& name) const = 0;
	virtual shaderResourceLocation GetReservedMeshTextureUniformLocation(unsigned int index) const = 0;
	virtual shaderResourceLocation GetReservedMeshColourUniformLocation(unsigned int index) const = 0;
	virtual shaderResourceLocation GetModelMatrixLocation() const = 0;
protected:
};