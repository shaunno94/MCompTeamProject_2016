#include "ModelLoader.h"
#include "Helpers/File.h"
#include <unordered_map>

Mesh* ModelLoader::LoadOBJ(const std::string& filePath, bool bufferData)
{
	std::string workingDirectory = File::GetContainingFolder(filePath);

	assert(("ModelLoader::LoadOBJ could not determine working directory from given filePath", workingDirectory != ""));

	// load file into stringstream
	std::ifstream stream = std::ifstream(filePath);
	if (!stream)   // no file
		throw std::invalid_argument("ModelLoader::HandleObjMtlLib could not find file.");

	std::unordered_map<std::string, MeshMtlData*> mtlMap;

	ObjMeshData* obj = new ObjMeshData();
	// init data with *blanks* for defaults
	obj->inputVertices.push_back(Vec3Graphics(0, 0, 0));
	obj->inputNormals.push_back(Vec3Graphics(0, 0, 0));
	obj->inputTexCoords.push_back(Vec2Graphics(0, 0));

	std::string token;
	Mesh* finalMesh = nullptr;

	// parse file from line start and build vertex data
	while (!stream.eof())
	{
		token = "";
		stream >> token;

		switch (token[0])
		{
		case '#': // comment
		{
			std::string line;  // remove line
			std::getline(stream, line);
			continue;
		}
		case 'v':
		{
			switch (token[1])
			{
			case 't': // texture
			{
				Vec2Graphics vec;
				stream >> vec.x >> vec.y;
				obj->inputTexCoords.push_back(vec);
				break;
			}
			case 'n': // normal
			{
				Vec3Graphics vec;
				stream >> vec.x >> vec.y >> vec.z;
				obj->inputNormals.push_back(vec);
				break;
			}
			default: // vector
			{
				Vec3Graphics vec;
				stream >> vec.x >> vec.y >> vec.z;
				obj->inputVertices.push_back(vec);
				break;
			}
			}
		}
		case 'f':   // face
		{
			std::string line;
			std::getline(stream, line);

			if (line.length() < 5)   // ignore face errors (certain files)
				continue;

			bool handleFace = HandleOBJFace(line, obj);
			assert(("ModelLoader::HandleOBJFace Failed!", handleFace));
			break;
		}
		case 'o':
		case 'g':
		{
			if (obj->finalIndices.size() > 0)
			{
				if (!finalMesh)
					finalMesh = CreateMesh(obj, mtlMap);
				else
					finalMesh->AddChild(CreateMesh(obj, mtlMap));

				obj->idCounter = 0;
				obj->finalIndices.clear();
				obj->objVertexList.clear();
				obj->mtlReference = "";
			}
			//create object
			break;
		}

		default:
			if (token == "mtllib")
			{
				stream >> token;
				HandleObjMtlLib(File::AppendPath(workingDirectory, token), mtlMap);
			}
			else if (token == "usemtl")
			{
				stream >> token;
				obj->mtlReference = token;
			}
			break;
		}
	}

	//getting final data
	if (!finalMesh)
		finalMesh = CreateMesh(obj, mtlMap);
	else
		finalMesh->AddChild(CreateMesh(obj, mtlMap));

	//clean
	delete obj;

	for (auto& tempMtl : mtlMap)
	{
		for (size_t i = 0; i < ReservedMeshTextures.size; ++i)
			if(tempMtl.second->textureMaps[i])
				tempMtl.second->textureMaps[i]->Clear();
		delete tempMtl.second;
	}

	if (bufferData)
		finalMesh->BufferData();

	return finalMesh;
}

void ModelLoader::HandleObjMtlLib(const std::string& filepath, std::unordered_map<std::string, MeshMtlData*>& mtlMap)
{
	std::string workingDirectory = File::GetContainingFolder(filepath);
	std::ifstream file(filepath);

	if (!file)   // no file
		return;
	//throw std::invalid_argument("ModelLoader::HandleObjMtlLib could not find file.");

	MeshMtlData* lastMtlData = nullptr;
	std::string token;

	while (!file.eof())
	{
		file >> token;

		switch (token[0])
		{
		case '#': // comment
		{
			std::string line;  // remove line
			std::getline(file, line);
			continue;
		}
		case 'n':
		{
			if (token == "newmtl")
			{
				file >> token;
				lastMtlData = new MeshMtlData();
				std::fill(lastMtlData->colours, lastMtlData->colours + ReservedMeshColours.size, Vec3Graphics::ZEROS);
				std::fill(lastMtlData->textureMaps, lastMtlData->textureMaps + ReservedMeshTextures.size, nullptr);
				lastMtlData->specExponent = 0.0f;
				mtlMap.insert(std::pair<std::string, MeshMtlData*>(token, lastMtlData));
			}
			break;
		}
		case 'K':
		{
			switch (token[1])
			{
			case 'a': // ambient colour
			{
				Vec3Graphics colour;
				file >> colour.x >> colour.y >> colour.z;
				lastMtlData->colours[ReservedMeshColours.AMBIENT.index] = colour;
				break;
			}
			case 'd': // diffuse colour
			{
				Vec3Graphics colour;
				file >> colour.x >> colour.y >> colour.z;
				lastMtlData->colours[ReservedMeshColours.DIFFUSE.index] = colour;
				break;
			}
			case 's': // specular colour
			{
				Vec3Graphics colour;
				file >> colour.x >> colour.y >> colour.z;
				lastMtlData->colours[ReservedMeshColours.SPECULAR.index] = colour;
				break;
			}
			}
			break;
		}
		case 'N':
		{
			if (token == "Ns") // specular exponent
			{
				file >> lastMtlData->specExponent;
			}
			break;
		}
		case 'm':
		{
			if (token == "map_Kd") // diffuse map
			{
				file >> token;
				lastMtlData->textureMaps[ReservedMeshTextures.DIFFUSE.index] = Texture::Get(File::AppendPath(workingDirectory, token));
			}
			else if (token == "map_Ks") // specular colour map
			{
				file >> token;
				lastMtlData->textureMaps[ReservedMeshTextures.SPECULAR_COLOUR.index] = Texture::Get(File::AppendPath(workingDirectory, token));
			}
			else if (token == "map_Ns") // specular highlight map
			{
				file >> token;
				lastMtlData->textureMaps[ReservedMeshTextures.SPECULAR_HIGHLIGHT.index] = Texture::Get(File::AppendPath(workingDirectory, token));
			}
			else if (token == "map_d") // alpha map
			{
				file >> token;
				lastMtlData->textureMaps[ReservedMeshTextures.ALPHA.index] = Texture::Get(File::AppendPath(workingDirectory, token));
			}
			else if (token == "map_bump") // bump map
			{
				file >> token;
				lastMtlData->textureMaps[ReservedMeshTextures.BUMP.index] = Texture::Get(File::AppendPath(workingDirectory, token));
			}
			break;
		}
		}
	}
}

bool ModelLoader::HandleOBJFace(std::string line, ObjMeshData* obj)
{
	std::vector<unsigned int> indices;
	unsigned data = 0;
	unsigned spaceCounter = 0;
	unsigned slashCounter = 0;

	bool skipTex = false;
	bool skipNorm = false;
	bool lastCharSpace = false;
	bool isPolygon = false;

	if (line.find("//") != std::string::npos)   // no textures
	{
		skipTex = true;
	}

	for (char& c : line)
	{
		if (c == ' ')   // count spaces ( = number of vertices in face)
		{
			lastCharSpace = true;
		}
		else if (c == '/')   // count slashes
		{
			c = ' ';
			slashCounter++;
		}
		else if (lastCharSpace)   // valid spaces have numbers after
		{
			spaceCounter++;
			lastCharSpace = false;
		}
	}

	if (slashCounter / spaceCounter == 1 && !skipTex)   // 2*slashes if normals are skipped
	{
		skipNorm = true;
	}
	if (slashCounter == 0)
	{
		skipTex = true;
		skipNorm = true;
	}

	if (spaceCounter == 4) // 4 sets of vertices in face
		isPolygon = true;

	// create easier data handle
	std::stringstream stream(line);
	while (stream >> data)
	{
		indices.push_back(data);
	}


	// check if the correct amount of indices have been loaded
	size_t numIndices = indices.size();
	size_t verts = isPolygon ? 4 : 3;
	size_t skip = verts - skipTex - skipNorm;
	size_t shouldBe = verts * skip;
	if (numIndices != shouldBe)
		return false;

	if (skipTex && skipNorm)
	{
		unsigned vertices = isPolygon ? 4 : 3;
		std::vector<unsigned int> ids;

		for (size_t i = 0; i < vertices; ++i)
		{
			ObjVertData vert = ObjVertData();
			vert.vertex = indices[i];
			vert.texture = 0;
			vert.normals = 0;

			// get id of unique vertex
			ids.push_back(AddOBJVertToList(obj, vert));
		}

		obj->finalIndices.push_back(ids[0]);
		obj->finalIndices.push_back(ids[1]);
		obj->finalIndices.push_back(ids[2]);

		if (isPolygon)
		{
			obj->finalIndices.push_back(ids[0]);
			obj->finalIndices.push_back(ids[2]);
			obj->finalIndices.push_back(ids[3]);
		}
	}
	else if (skipTex)
	{
		AddFaceMissing(obj, indices, isPolygon, true);
	}
	else if (skipNorm)
	{
		AddFaceMissing(obj, indices, isPolygon, false);
	}
	else   // v1/vt1/nv1 v1/vt2/nv2
	{
		AddFace(obj, indices, isPolygon);
	}

	return true;
}

void ModelLoader::AddFace(ObjMeshData* obj, std::vector<unsigned int>& indices, bool isPolygon)
{
	unsigned vertices = isPolygon ? 4 : 3;
	std::vector<unsigned int> ids;

	for (size_t i = 0; i < vertices * 3; i += 3)
	{
		ObjVertData vert = ObjVertData();
		vert.vertex = indices[i];
		vert.texture = indices[i + 1];
		vert.normals = indices[i + 2];

		// get id of unique vertex
		ids.push_back(AddOBJVertToList(obj, vert));
	}

	obj->finalIndices.push_back(ids[0]);
	obj->finalIndices.push_back(ids[1]);
	obj->finalIndices.push_back(ids[2]);

	if (isPolygon)
	{
		obj->finalIndices.push_back(ids[0]);
		obj->finalIndices.push_back(ids[2]);
		obj->finalIndices.push_back(ids[3]);
	}
}

void ModelLoader::AddFaceMissing(ObjMeshData* obj, std::vector<unsigned int>& indices, bool isPolygon, bool skipTex)
{

	const GLuint vertices = isPolygon ? 4 : 3;

	std::vector<unsigned int> ids;

	for (size_t i = 0; i < vertices * 2; i += 2)
	{
		ObjVertData vert = ObjVertData();
		vert.vertex = indices[i];
		vert.texture = skipTex ? 0 : indices[i + 1];
		vert.normals = skipTex ? indices[i + 1] : 0;

		// get id of unique vertex
		unsigned id = AddOBJVertToList(obj, vert);
		ids.push_back(id);
	}

	obj->finalIndices.push_back(ids[0]);
	obj->finalIndices.push_back(ids[1]);
	obj->finalIndices.push_back(ids[2]);

	if (isPolygon)
	{
		obj->finalIndices.push_back(ids[0]);
		obj->finalIndices.push_back(ids[2]);
		obj->finalIndices.push_back(ids[3]);
	}
}

GLuint ModelLoader::AddOBJVertToList(ObjMeshData* obj, ObjVertData& vert)
{
	bool vertFound = false;
	unsigned idOfVert = 0;

	// find a vertex with the same data
	for (ObjVertData& v : obj->objVertexList)
	{
		// if found, return id of found vertex
		if (v.Compare(vert))
		{
			vertFound = true;
			idOfVert = v.id;
		}
	}

	// if one can't be found create a new one
	if (!vertFound)
	{
		vert.id = ++obj->idCounter;
		obj->objVertexList.push_back(vert);
		idOfVert = vert.id;
	}

	return idOfVert;
}

Mesh* ModelLoader::CreateMesh(ObjMeshData* obj, std::unordered_map<std::string, MeshMtlData*>& mtls)
{
	size_t numVertices = obj->objVertexList.size();

	Mesh* mesh = new Mesh();
	mesh->numVertices = numVertices;
	mesh->vertices = new Vec3Graphics[numVertices];
	mesh->normals = new Vec3Graphics[numVertices];
	mesh->textureCoords = new Vec2Graphics[numVertices];

	auto match = mtls.find(obj->mtlReference);
	if (match != mtls.end())
		mesh->SetMtlData(*match->second);

	for (size_t i = 0; i < numVertices; ++i)
	{
		mesh->vertices[i] = obj->inputVertices[obj->objVertexList[i].vertex];
		mesh->normals[i] = obj->inputNormals[obj->objVertexList[i].normals];
		mesh->textureCoords[i] = obj->inputTexCoords[obj->objVertexList[i].texture];
	}

	mesh->colours = new Vec4Graphics[numVertices];
	for (size_t i = 0; i < numVertices; ++i)
	{
		mesh->colours[i] = Vec4Graphics::ONES;
	}

	size_t numIndices = obj->finalIndices.size();
	mesh->numIndices = numIndices;
	mesh->indices = new unsigned int[numIndices];
	for (size_t i = 0; i < numIndices; ++i)
	{
		mesh->indices[i] = obj->finalIndices[i] - 1;
	}

	//TODO: Add Mtl data!!!

	return mesh;
}