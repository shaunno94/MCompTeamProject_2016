#pragma once

#include <string>

#define SHADER_VERTEX   0
#define SHADER_FRAGMENT 1
#define SHADER_GEOMETRY 2
#define SHADER_TCS 3
#define SHADER_TES 4
#define SHADER_MAX_STAGE_COUNT 5

#define SHADER_DIR	"../Assets/Shaders/"
#define MODEL_DIR	"../Assets/Models/"
#define TEXTURE_DIR  "../Assets/Textures/"
#define AUDIO_DIR	"../Assets/AUDIO/"


struct ShaderUniformInfo
{
	unsigned int index;
	char* name;
};

static size_t reservedMeshTextureCounter = 0;

static const struct ReservedMeshTexturesStruct
{
	const union
	{
		const struct
		{
			ShaderUniformInfo DIFFUSE;
			ShaderUniformInfo SPECULAR_COLOUR;
			ShaderUniformInfo SPECULAR_HIGHLIGHT;
			ShaderUniformInfo ALPHA;
			ShaderUniformInfo BUMP;
		};
		ShaderUniformInfo values[5];
	};
	static const size_t size = 5;

} ReservedMeshTextures =
{
	reservedMeshTextureCounter++, "diffuseTex",
	reservedMeshTextureCounter++, "specularColourTex",
	reservedMeshTextureCounter++, "specularHighlightTex",
	reservedMeshTextureCounter++, "alphaTex",
	reservedMeshTextureCounter++, "bumpMap"
};

//TODO - remove?? no
static const struct ReservedOtherTexturesStruct
{
	const union
	{
		const struct
		{
			ShaderUniformInfo DEPTH;
			ShaderUniformInfo NORMALS;
		};
		ShaderUniformInfo values[2];
	};
	static const size_t size = 2;

} ReservedOtherTextures =
{
	reservedMeshTextureCounter++, "depthTex",
	reservedMeshTextureCounter++, "normalTex"
};

static size_t reservedMeshColourCounter = 0;

static const struct ReservedMeshColoursStruct
{
	const union
	{
		const struct
		{
			ShaderUniformInfo AMBIENT;
			ShaderUniformInfo DIFFUSE;
			ShaderUniformInfo SPECULAR;
		};
		ShaderUniformInfo values[3];
	};
	static const size_t size = 3;

} ReservedMeshColours =
{
	reservedMeshColourCounter++, "ambientColour",
	reservedMeshColourCounter++, "diffuseColour",
	reservedMeshColourCounter++, "specularColour"
};



#define MAX_BOUND_TEXTURE_COUNT 32;