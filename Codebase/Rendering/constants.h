#pragma once

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

#ifdef _DEBUG
//In debug, the expectation is applications will be run from visual studio (starting directory being the project)
#define SHADER_DIR	"../Assets/Shaders/"
#define MODEL_DIR	"../Assets/Models/"
#define TEXTURE_DIR  "../Assets/Textures/"
#define AUDIO_DIR	"../Assets/AUDIO/"
#else
//In release, the expectation is applications will be run from the .exe files (starting directory being the .exe location)
#define SHADER_DIR	"Assets/Shaders/"
#define MODEL_DIR	"Assets/Models/"
#define TEXTURE_DIR  "Assets/Textures/"
#define AUDIO_DIR	"Assets/AUDIO/"
#endif

/// <summary>
/// Structure for wrapping shader uniform name and index location for any local caching.
/// </summary>
struct ShaderUniformInfo
{
	unsigned int index;
	char* name;
};

/// <summary>
/// Internal counter for easily setting ReservedMeshTexturesStruct indices.
/// </summary>
static size_t reservedMeshTextureCounter = 0;

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
	reservedMeshTextureCounter++, "bumpMap"
};

/// <summary>
/// Internal counter for easily setting ReservedMeshTexturesStruct indices.
/// </summary>
static size_t reservedMeshColourCounter = 0;

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