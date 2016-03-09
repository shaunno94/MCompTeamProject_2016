#pragma once
#include <Math\Matrix4Simple.h>
/** @defgroup Rendering Rendering
*  Rendering related functionality.
*  @{
*/

#define SHADER_VERTEX   0
#define SHADER_FRAGMENT 1
#define SHADER_GEOMETRY 2
#define SHADER_TCS 3
#define SHADER_TES 4
#define SHADER_MAX_STAGE_COUNT 5

//size of shadow textures, TODO - smaller?
#define SHADOWSIZE 4096

/// @ingroup Rendering
/// <summary>
///
/// </summary>
static const float biasValues[16] =
{
	0.5, 0.0, 0.0, 0.0,
	0.0, 0.5, 0.0, 0.0,
	0.0, 0.0, 0.5, 0.0,
	0.5, 0.5, 0.5, 1.0
};
/// @ingroup Rendering
/// <summary>
///
/// </summary>
static const Matrix4Simple biasMatrix(biasValues);

//A handy enumerator, to determine which member of the bufferObject array
//holds which data
/// @ingroup Rendering
/// <summary>
/// Enumerator, to determine which member of the bufferObject array holds which data
/// </summary>
enum MeshBuffer
{
	VERTEX_BUFFER,
	TEXTURE_BUFFER,
	NORMAL_BUFFER,
	TANGENT_BUFFER,
	INDEX_BUFFER,
	MAX_BUFFER
};

# define BIT(x) 1<<x
enum CollisionMasks {
	COL_NOTHING = 0,
	COL_WALL = BIT(0),
	COL_CAR = BIT(1),
	COL_AI_CAR = BIT(2),
	COL_BALL = BIT(3),
	COL_GOAL = BIT(4)
};
enum CollisionGroups {
	GROUP_WALL_BALL = COL_WALL | COL_BALL,
	GROUP_CAR_WALL = COL_CAR | COL_WALL,
	GROUP_CAR_BALL = COL_CAR | COL_BALL
};

enum AITypes
{
	SHOOTER,
	GOALKEEPER,
	AGGRESSIVE
};

enum AIStates
{
	POSITION,
	SHOOT,
	DEFENCE,
	ADVANCE,
	GUARD_GOAL,
	CLEAR_GOAL,
	AGGRO
};

#ifdef _DEBUG
//In debug, the expectation is applications will be run from visual studio (starting directory being the project)
#ifndef ORBIS
#define SHADER_DIR	"../Assets/Shaders/"
#define MODEL_DIR	"../Assets/Models/"
#define TEXTURE_DIR  "../Assets/Textures/"
#define AUDIO_DIR	"../Assets/AUDIO/"
#else
#define SHADER_DIR	"/app0/Assets/PS4Shaders/"
#define MODEL_DIR	"/app0/Assets/Models/"
#define TEXTURE_DIR  "/app0/Assets/Textures/"
#define AUDIO_DIR	"/app0/Assets/AUDIO/"
#endif
#else
#ifndef ORBIS
//In release, the expectation is applications will be run from the .exe files (starting directory being the .exe location)
#define SHADER_DIR	"Assets/Shaders/"
#define MODEL_DIR	"Assets/Models/"
#define TEXTURE_DIR  "Assets/Textures/"
#define AUDIO_DIR	"Assets/AUDIO/"
#else
#define SHADER_DIR "/app0/Assets/PS4Shaders/"
#define MODEL_DIR	"/app0/Assets/Models/"
#define TEXTURE_DIR  "/app0/Assets/Textures/"
#define AUDIO_DIR	"/app0/Assets/AUDIO/"
#endif
#endif

/// <summary>
/// Structure for wrapping shader uniform name and index location for any local caching.
/// </summary>
struct ShaderUniformInfo
{
	unsigned char index;
	char* name;
};

/// <summary>
/// Internal counter for easily setting ReservedMeshTexturesStruct indices.
/// </summary>
static unsigned char reservedMeshTextureCounter = 0;

/// <summary>
/// A static constant structure for representing a collection of reserved Mesh Texture uniforms.
/// </summary>
static const struct ReservedMeshTexturesStruct
{
	const union
	{
		const struct
		{
			/// <summary>
			/// Reserved Mesh Texture uniform information on the diffuse Texture.
			/// </summary>
			ShaderUniformInfo DIFFUSE;
			/// <summary>
			/// Reserved Mesh Texture uniform information on the specular colour Texture.
			/// </summary>
			ShaderUniformInfo SPECULAR_COLOUR;
			/// <summary>
			/// Reserved Mesh Texture uniform information on the specular highlight Texture.
			/// </summary>
			ShaderUniformInfo SPECULAR_HIGHLIGHT;
			/// <summary>
			/// Reserved Mesh Texture uniform information on the alpha Texture.
			/// </summary>
			ShaderUniformInfo ALPHA;
			/// <summary>
			/// Reserved Mesh Texture uniform information on the bump map Texture.
			/// </summary>
			ShaderUniformInfo BUMP;
		};
		/// <summary>
		/// Array access point for reserved Mesh Texture uniforms' information.
		/// </summary>
		ShaderUniformInfo values[5];
	};
	/// <summary>
	/// Number of reserved Mesh Texture uniforms.
	/// </summary>
	static const size_t size = 5;

}
/// <summary>
/// Reserved Mesh Texture uniforms.
/// </summary>
ReservedMeshTextures =
{
	reservedMeshTextureCounter++, "diffuseTex",
	reservedMeshTextureCounter++, "specularColourTex",
	reservedMeshTextureCounter++, "specularHighlightTex",
	reservedMeshTextureCounter++, "alphaTex",
	reservedMeshTextureCounter++, "bumpTex"
};

static const struct ReservedOtherTexturesStruct
{
	const union
	{
		const struct
		{
			ShaderUniformInfo DEPTH;
			ShaderUniformInfo NORMALS;
			ShaderUniformInfo EMISSIVE;
			ShaderUniformInfo SPECULAR;
			ShaderUniformInfo SHADOW_2D;
			ShaderUniformInfo SHADOW_CUBE;
			ShaderUniformInfo CUBE;
		};
		ShaderUniformInfo values[7];
};
	static const size_t size = 7;

} ReservedOtherTextures =
{
	reservedMeshTextureCounter++, "depthTex",
	reservedMeshTextureCounter++, "normTex",
	reservedMeshTextureCounter++, "emissiveTex",
	reservedMeshTextureCounter++, "specularTex",
	reservedMeshTextureCounter++, "shadowTex",
	reservedMeshTextureCounter++, "shadowTexCube",
	reservedMeshTextureCounter++, "cubeTex"
};

/// <summary>
/// Internal counter for easily setting ReservedMeshTexturesStruct indices.
/// </summary>
static unsigned char reservedMeshColourCounter = 0;

/// <summary>
/// A static constant structure for representing a collection of reserved Mesh colour uniforms.
/// </summary>
static const struct ReservedMeshColoursStruct
{
	const union
	{
		const struct
		{
			/// <summary>
			/// Reserved Mesh colour uniform information on the ambient colour.
			/// </summary>
			ShaderUniformInfo AMBIENT;
			/// <summary>
			/// Reserved Mesh colour uniform information on the diffuse colour.
			/// </summary>
			ShaderUniformInfo DIFFUSE;
			/// <summary>
			/// Reserved Mesh colour uniform information on the specular colour.
			/// </summary>
			ShaderUniformInfo SPECULAR;
		};
		/// <summary>
		/// Array access point for reserved Mesh colour uniforms' information.
		/// </summary>
		ShaderUniformInfo values[3];
	};
	/// <summary>
	/// Number of reserved Mesh colour uniforms.
	/// </summary>
	static const size_t size = 3;

}
/// <summary>
/// Reserved Mesh colour uniforms.
/// </summary>
ReservedMeshColours =
{
	reservedMeshColourCounter++, "ambientColour",
	reservedMeshColourCounter++, "diffuseColour",
	reservedMeshColourCounter++, "specularColour"
};

/** @} */