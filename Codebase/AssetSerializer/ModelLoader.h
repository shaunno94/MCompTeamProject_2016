#pragma once

#include <string>
#include <vector>
#include <sstream>
#include "Rendering/Mesh.h"
#include "Rendering/constants.h"

struct ObjVertData
{
	size_t vertex = 0;
	size_t texture = 0;
	size_t normals = 0;
	GLuint id = 0;

	bool Compare(const ObjVertData& o2)
	{
		if (this->vertex == o2.vertex &&
		    this->texture == o2.texture &&
		    this->normals == o2.normals)
			return true;

		return false;
	}

	bool operator<(const ObjVertData& o) const
	{
		return this->id < o.id ? true : false;
	}
};

struct ObjMeshData
{
	std::vector<Vec2Graphics> inputTexCoords;
	std::vector<Vec3Graphics> inputVertices;
	std::vector<Vec3Graphics> inputNormals;

	std::vector<ObjVertData> objVertexList;
	std::vector<unsigned int> finalIndices;
	std::string mtlReference;

	unsigned int idCounter = 0;
};

class ModelLoader
{
public:
	// Loads file and returns correct Mesh, buffered or unbuffered
	static Mesh* LoadOBJ(const std::string& filePath, bool bufferData = true);

	// Saves a given mesh to .mgl format
	static void SaveMeshToMGL(Mesh* mesh, std::string fileName);

protected:
	// Singleton protection
	ModelLoader() {}
	~ModelLoader() {}

	/****** Methods ******/

	// Gets a istringstream of a full file
	static std::stringstream* LoadFileToSS(const std::string& fileName);
	// Loads binary file and returns a vector of required type
	template < typename T >
	static std::vector<T>* LoadFileToVec(const std::string& fileName);


	static void HandleObjMtlLib(const std::string& filepath, std::unordered_map<std::string, MeshMtlData*>& mtlMap);
	static bool HandleOBJFace(std::string line, ObjMeshData* obj);
	static size_t AddOBJVertToList(ObjMeshData* obj, ObjVertData& vert);
	static Mesh* CreateMesh(ObjMeshData* obj, std::unordered_map<std::string, MeshMtlData*>& mtls);

	// Handle OBJ face with no normals
	static void AddFaceMissing(ObjMeshData* obj, std::vector<unsigned int>& indices, bool isPolygon, bool skipTex);
	// Handle OBJ face
	static void AddFace(ObjMeshData* obj, std::vector<unsigned int>& indices, bool isPolygon);
};

