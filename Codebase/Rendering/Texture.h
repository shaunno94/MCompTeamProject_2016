#pragma once

#ifndef ORBIS
#include "Dependencies/glew-1.13.0/include/GL/glew.h"
#include "Dependencies/SOIL2/SOIL2.h"
#pragma comment(lib, "SOIL2/soil2.lib")
#else
#include <fstream>
#include <gnf.h>
#include <gnm\texture.h>
#include "Memory/PS4Memory.h"
#endif

#include <string>
#include <unordered_map>
#include <vector>
#include "Helpers/common.h"

#include <type_traits>

#ifdef _DEBUG
#define TEXTURE_TRACK_STATS
#endif

enum TextureFlags
{
	REPEATING = 1,
	CLAMPING = 2,

	NEAREST_NEIGHBOUR_MIN_FILTERING = 4,
	NEAREST_NEIGHBOUR_MAX_FILTERING = 8,
	NEAREST_NEIGHBOUR_MIN_MAX_FILTERING = 4 | 8,

	BILINEAR_MIN_FILTERING = 16,
	BILINEAR_MAX_FILTERING = 32,
	BILINEAR_MIN_MAX_FILTERING = 16 | 32,

	TRILINEAR_MIN_FILTERING = 64,
	TRILINEAR_MAX_FILTERING = 128,
	TRILINEAR_MIN_MAX_FILTERING = 64 | 128,

	ANISOTROPIC_FILTERING = 256
};

#ifndef ORBIS
typedef unsigned int textureHandle;
#else
typedef const sce::Gnm::Texture* textureHandle;
#endif

/// @ingroup Rendering
/// <summary>
/// Manager class for representing a texture file.
/// </summary>
/// <remarks>
/// This class tracks texture requests and needs to be instantiated and cleaned up through static factory methods.
/// </remarks>
#ifndef ORBIS
class Texture
#else
class Texture : public PS4Memory
#endif

{
public:

	/// <summary>
	/// Loads this Texture into a given GPU texture unit.
	/// </summary>
	/// <remarks>
	/// If this texture is not loaded onto the GPU, the <see cref="Texture::LoadFromFile"/> will be called automatically.
	/// </remarks>
	/// <param name="textureUnit">GPU texture unit.</param>
	void Load(unsigned int textureUnit);

	/// <summary>
	/// Loads the managed texture from file.
	/// </summary>
	/// <remarks>
	/// If this texture is already loaded onto the GPU, the GPU texture will be cleaned up and the texture file will be loaded up again.
	/// </remarks>
	void LoadFromFile();

	/// <summary>
	/// Get the GPU texture id.
	/// </summary>
	/// <remarks>
	/// If this texture is not loaded onto the GPU, the <see cref="Texture::LoadFromFile"/> will be called automatically.
	/// </remarks>
	/// <returns>GPU texture id.</returns>
	inline textureHandle GetTextureId()
	{
		if (!textureId) LoadFromFile();
			return textureId;
	}
	/// <summary>
	/// File path of the texture.
	/// </summary>
	/// <returns>Filepath.</returns>
	inline const std::string& GetFilePath()
	{
		return filePath;
	}

	static inline unsigned int GetMemoryUsage()
	{
		return s_memoryUsage;
	}

	Texture& operator=(const Texture&) = delete;
	Texture(const Texture&) = delete;

	/// <summary>
	/// Informs the Texture management that one user of this Texture no longer needs it.
	/// </summary>
	void Clear();

	/// <summary>
	/// Informs the Texture management that you created a new copy of this Texture.
	/// </summary>
	inline void ReserveCopy()
	{
		++m_referenceCount;
	}

	/// <summary>
	/// Texture Factory function for construction.
	/// </summary>
	/// <remarks>
	/// If a texture with the given filepath already exists, A new variant of that Texture class will be forcefully made, leaving several active copies of the same texture.
	/// If multiple copies of the same texture is not needed, the <see cref="Texture::Get"/> should be used instead.
	/// </remarks>
	/// <param name="filePath">Texture filepath.</param>
	/// <param name="preload">Load onto the GPU.</param>
	/// <returns>Pointer to a Texture.</returns>
	static Texture* Make(const std::string& filePath, bool preload = false);
	/// <summary>
	/// Texture factory function for retrieval.
	/// </summary>
	/// <remarks>
	///	If a texture with the given filepath already exists, that texture will be returned.
	/// </remarks>
	/// <param name="filePath">Texture filepath.</param>
	/// <param name="preload">Load onto the GPU.</param>
	/// <returns>Pointer to a Texture.</returns>
	static Texture* Get(const std::string& filePath, bool preload = false);

	/// <summary>
	/// Clears all Textures that are still left active.
	/// </summary>
	static void ClearAll();

	void SetTextureParams(unsigned int flags);

protected:
	~Texture();

	static std::unordered_map<std::string, std::vector<Texture*>> s_textureRecords;

	static unsigned int s_memoryUsage;
	void MeasureMemoryUsageAdd(textureHandle textureId);
	void MeasureMemoryUsageSubtract(textureHandle textureId);

	Texture(const std::string& filepath, unsigned int textureCopyIndex, bool preload = false);

	textureHandle textureId;
	unsigned int textureCopyIndex;
	std::string filePath;
	unsigned int m_referenceCount;
};

