#pragma once

#include "GL/glew.h"
#include "Dependencies/SOIL2/SOIL2.h"
#pragma comment(lib, "SOIL2/soil2.lib")

#include <string>
#include <unordered_map>
#include <vector>
#include "Helpers/common.h"


/// @ingroup Rendering
/// <summary>
/// Manager class for representing a texture file.
/// </summary>
/// <remarks>
/// This class tracks texture requests and needs to be instantiated and cleaned up through static factory methods.
/// </remarks>
class Texture
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
	inline GLuint GetTextureId()
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

	static inline int GetMemoryUsage()
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

protected:
	~Texture();

	static std::unordered_map<std::string, std::vector<Texture*>> s_textureRecords;

	static int s_memoryUsage;
	void MeasureMemoryUsageAdd(GLuint textureId);
	void MeasureMemoryUsageSubstract(GLuint textureId);


	Texture(const std::string& filepath, size_t textureCopyIndex, bool preload = false);


	GLuint textureId;
	size_t textureCopyIndex;
	std::string filePath;
	unsigned int m_referenceCount;
};

