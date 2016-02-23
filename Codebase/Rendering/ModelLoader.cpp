#include "ModelLoader.h"
#include "Helpers/File.h"
#include <sstream>
#include <fstream>


#define MGL_LOAD_FAST_EXPERIMENTAL 1
#define MGL_FILE_CURRENTVERSION 0.01f

template<typename T>
inline void ReadFromMGL(std::ifstream& in, T& val)
{
	in.read((char*)&val, sizeof(T));
}


Mesh* ReadMeshFromMGL(std::ifstream& in)
{
	size_t numVertices;
	ReadFromMGL(in, numVertices);
	Vec3Graphics* vertices = new Vec3Graphics[numVertices];
	// Vertices
#if MGL_LOAD_FAST_EXPERIMENTAL == 0
	for (size_t i = 0; i < numVertices; ++i)
		ReadFromMGL(in, vertices[i]);
#else
	in.read((char*)vertices, sizeof(Vec3Graphics) * numVertices);
#endif

	Vec2Graphics* texCoords = nullptr;
	char hasTexCoords;
	ReadFromMGL(in, hasTexCoords);
	// TexCoords
	if (hasTexCoords)
	{
		texCoords = new Vec2Graphics[numVertices];
#if MGL_LOAD_FAST_EXPERIMENTAL == 0
		for (size_t i = 0; i < numVertices; ++i)
			ReadFromMGL(in, texCoords[i]);
#else
		in.read((char*)texCoords, sizeof(Vec2Graphics) * numVertices);
#endif
	}

	Vec3Graphics* normals = nullptr;
	char hasNormals;
	ReadFromMGL(in, hasNormals);
	// Normals
	if (hasNormals)
	{
		normals = new Vec3Graphics[numVertices];
#if MGL_LOAD_FAST_EXPERIMENTAL == 0
		for (size_t i = 0; i < numVertices; ++i)
			ReadFromMGL(in, normals[i]);
#else
		in.read((char*)normals, sizeof(Vec3Graphics) * numVertices);
#endif
	}

	Vec3Graphics* tangents = nullptr;
	char hastangents;
	ReadFromMGL(in, hastangents);
	// Tangents
	if (hastangents)
	{
		tangents = new Vec3Graphics[numVertices];
#if MGL_LOAD_FAST_EXPERIMENTAL == 0
		for (size_t i = 0; i < numVertices; ++i)
			ReadFromMGL(in, tangents[i]);
#else
		in.read((char*)tangents, sizeof(Vec3Graphics) * numVertices);
#endif
	}

	size_t* indices = nullptr;
	size_t numIndices;
	ReadFromMGL(in, numIndices);
	// Indices
	if (numIndices)
	{
		indices = new size_t[numIndices];
#if MGL_LOAD_FAST_EXPERIMENTAL == 0
		for (size_t i = 0; i < numIndices; ++i)
			ReadFromMGL(in, indices[i]);
#else
		in.read((char*)indices, sizeof(size_t) * numIndices);
#endif
	}

#ifndef ORBIS
	Mesh* mesh = new OGLMesh(numVertices, vertices, texCoords, normals, tangents, numIndices, indices);
#else
	Mesh* mesh = new PS4Mesh(numVertices, vertices, texCoords, normals, tangents, numIndices, indices);
#endif
	
	MeshMtlData mtlData;
	memset(&mtlData, 0, sizeof(MeshMtlData));

	//MTL data
	std::stringstream sstream;
	//read textures
	for (size_t i = 0; i < ReservedMeshTextures.size; ++i)
	{
		char c;
		in.get(c);
		if (c)
		{
			while (c)
			{
				sstream << c;
				in.get(c);
			}
			Texture* newTexture = Texture::Get(MODEL_DIR + sstream.str());
			newTexture->SetTextureParams(TextureFlags::REPEATING | TextureFlags::TRILINEAR_MIN_MAX_FILTERING);

			mtlData.textureMaps[i] = newTexture;
			sstream.str("");
			sstream.clear();
		}
	}

#if MGL_LOAD_FAST_EXPERIMENTAL == 0
	for (size_t i = 0; i < ReservedMeshColours.size; ++i)
		ReadFromMGL(in, mtlData.colours[i]);
#else
	in.read((char*)mtlData.colours, sizeof(Vec3Graphics) * ReservedMeshColours.size);
#endif

	ReadFromMGL(in, mtlData.specExponent);

	mesh->SetMtlData(mtlData);


	size_t numChildren;
	ReadFromMGL(in, numChildren);

	for (size_t i = 0; i < numChildren; ++i)
		mesh->AddChild(ReadMeshFromMGL(in));

	// Clean
	for (size_t i = 0; i < ReservedMeshTextures.size; ++i)
		if (mtlData.textureMaps[i])
			mtlData.textureMaps[i]->Clear();

	return mesh;
}

// Loads file and returns correct Mesh, buffered or unbuffered
Mesh* ModelLoader::LoadMGL(const std::string& filePath, bool bufferData)
{
	std::ifstream file(filePath, std::ios::in | std::ios::binary);
	float version;
	ReadFromMGL(file, version);
	if (version != MGL_FILE_CURRENTVERSION)
		throw "Not matching version";

	Mesh* result = ReadMeshFromMGL(file);
	if (bufferData)
		result->BufferData();

	file.close();
	return result;
}