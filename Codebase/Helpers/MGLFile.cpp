#include "stdafx.h"
#include "MGLFile.h"

#include "MGLMesh.h"
#include "MGLExceptions.h"
#include "MGLDebug.h"

/*
TODO: Combine AddFace + AddFaceMissing
TODO: General Tidy 
TODO: Encapsulate try-catch
*/

/*******************************/
/*********** MGLFile ***********/
/*******************************/

std::stringstream* MGLFile::LoadFileToSS(std::string fileName) {
	// read in file
	std::ifstream file(fileName, std::ios::in);

	// test for errors
#ifdef MGL_USER_INCLUDE_FILETC
	try {
		MGLException_FileError::Test(file.is_open(), fileName);
		MGLException_File_FileType::Test(fileName, m_fileEXT);
	}
	catch (MGLException& e) {
		//std::cerr << e.what() << std::endl;
		MGLH_Log->AddLog(MGL_LOG_ERROR, GL_TRUE, e.what());

		return nullptr;
	}
#endif // MGL_USER_INCLUDE_FILETC

	// read file into stream
	std::stringstream* stream = new std::stringstream{};
	*stream << file.rdbuf();

	return stream;
}

template <typename T>
std::vector<T>* MGLFile::LoadFileToVec(std::string fileName) {
	// create binary file
	std::ifstream file(fileName, std::ios::in | std::ios::binary);

	file.seekg(0, std::ios_base::end);
	std::size_t size = (std::size_t)file.tellg(); // get file size
	file.seekg(0, std::ios_base::beg);

#ifdef MGL_USER_INCLUDE_FILETC
	try {
		// file is open and has correct extension
		MGLException_FileError::Test(file.is_open(), fileName);
		MGLException_File_FileType::Test(fileName, m_fileEXT);
	}
	catch (MGLException& e) {
		//std::cerr << e.what() << std::endl;
		MGLH_Log->AddLog(MGL_LOG_ERROR, GL_TRUE, e.what());

		return nullptr;
	}
#endif // MGL_USER_INCLUDE_FILETC

	// read file buffer into vector (dirty method)
	std::vector<T>* buffer = new std::vector < GLfloat >((size / sizeof(T)) + 1);
	file.read((GLchar*)&buffer->at(0), size);

	return buffer;
}

void MGLFile::SaveMeshToMGL(MGLMesh* mesh, std::string fileName, GLboolean saveColours) {
	// open file
	std::ofstream out(fileName + ".mgl", std::ios::binary);

#ifdef MGL_USER_INCLUDE_FILETC
	try {
		MGLException_FileError::Test(out.is_open(), fileName);
		MGLException_Null::Test(mesh);
	}
	catch (MGLException_FileError& e) {
		//std::cerr << e.what() << std::endl;
		MGLH_Log->AddLog(MGL_LOG_ERROR, GL_TRUE, e.what());

		return;
	}
	catch (MGLException_Null& e) {
		MGLH_Log->AddLog(MGL_LOG_ERROR, GL_TRUE, "%s%s", e.what(), "SMTM Mesh");

		return;
	}
#endif // MGL_USER_INCLUDE_FILETC

	GLfloat val = 0;
	// file version
	val = (GLfloat)MGL_FILE_CURRENTVERSION;
	out.write((GLchar*)&val, sizeof(GLfloat));
	// type
	val = (GLfloat)mesh->GetType();
	out.write((GLchar*)&val, sizeof(GLfloat));
	// numVertices
	val = (GLfloat)mesh->GetNumVertices();
	out.write((GLchar*)&val, sizeof(GLfloat));
	// numIndices
	val = (GLfloat)mesh->GetNumIndices();
	out.write((GLchar*)&val, sizeof(GLfloat));
	// determine colour val
	GLfloat colourVal = (GLfloat)saveColours;
	if (colourVal) {
		// if all the colours are the same, set colourVal to -1
		if (std::adjacent_find(mesh->GetColours()->begin(), mesh->GetColours()->end(),
			std::not_equal_to<glm::vec4>()) == mesh->GetColours()->end()) {

			colourVal = -1;
		}
	}
	out.write((GLchar*)&colourVal, sizeof(GLfloat));

	// Vertices
	for (glm::vec3 const& vec : *mesh->GetVertices()) {
		out.write((GLchar*)&vec.x, sizeof(GLfloat));
		out.write((GLchar*)&vec.y, sizeof(GLfloat));
		out.write((GLchar*)&vec.z, sizeof(GLfloat));
	}

	// TexCoords
	for (glm::vec2 const& vec : *mesh->GetTexCoords()) {
		out.write((GLchar*)&vec.x, sizeof(GLfloat));
		out.write((GLchar*)&vec.y, sizeof(GLfloat));
	}

	// Normals
	for (glm::vec3 const& vec : *mesh->GetNormals()) {
		out.write((GLchar*)&vec.x, sizeof(GLfloat));
		out.write((GLchar*)&vec.y, sizeof(GLfloat));
		out.write((GLchar*)&vec.z, sizeof(GLfloat));
	}

	// Colours
	if (colourVal > 1) {
		for (glm::vec4 const& vec : *mesh->GetColours()) {
			out.write((GLchar*)&vec.x, sizeof(GLfloat));
			out.write((GLchar*)&vec.y, sizeof(GLfloat));
			out.write((GLchar*)&vec.z, sizeof(GLfloat));
			out.write((GLchar*)&vec.w, sizeof(GLfloat));
		}
	}
	else if (colourVal == -1) {
		glm::vec4 vec = mesh->GetColours()->at(0);
		out.write((GLchar*)&vec.x, sizeof(GLfloat));
		out.write((GLchar*)&vec.y, sizeof(GLfloat));
		out.write((GLchar*)&vec.z, sizeof(GLfloat));
		out.write((GLchar*)&vec.w, sizeof(GLfloat));
	}

	// Indices
	for (GLuint const& ind : *mesh->GetIndices()) {
		GLfloat val = (GLfloat)ind;
		out.write((GLchar*)&val, sizeof(GLfloat));
	}

}

/**********************************/
/*********** MGLFileMGL ***********/
/**********************************/

MGLMesh* MGLFileMGL::Load(std::string fileName, GLboolean bufferData) {
	// speed tests
	MGLTimer timer = MGLTimer();
	timer.Start();

	std::vector<GLfloat>* buffer = LoadFileToVec<GLfloat>(fileName);

#ifdef MGL_USER_INCLUDE_FILETC
	try {
		// buffers not null
		MGLException_Null::Test(buffer);
		// buffer size is large enough (for DetermineFileSize())
		MGLException_IsLessThan::Test(buffer->size(), (std::size_t)MGL_FILE_BUFFERMINSIZE);
		// buffer size is what is expected
		MGLException_IsNotEqual::Test(DetermineFileSize(
			(GLuint)buffer->at(2), (GLuint)buffer->at(3), (GLint)buffer->at(4)) + 1, buffer->size());
	}
	catch (MGLException_Null& e) {
		//std::cerr << e.what() << ": FILE SIZE ERROR " << fileName << std::endl;
		MGLH_Log->AddLog(MGL_LOG_ERROR, GL_TRUE, "%s%s", e.what(), ": Null Buffer");
		delete buffer;
		return new MGLMesh(MGL_MESH_TRIANGLE);
	}
	catch (MGLException& e) {
		//std::cerr << e.what() << ": FILE SIZE ERROR " << fileName << std::endl;
		MGLH_Log->AddLog(MGL_LOG_ERROR, GL_TRUE, "%s%s%s", e.what(), ": FILE SIZE ERROR ", fileName.c_str());
		delete buffer;
		return new MGLMesh(MGL_MESH_TRIANGLE);
	}
#endif // MGL_USER_INCLUDE_FILETC

	MGLMesh* mesh = LoadMesh(buffer);

	if (bufferData)
		mesh->BufferAllData();

	delete buffer;

	timer.End();
	MGLH_Log->AddLog(MGL_LOG_MAIN, GL_TRUE, "MGL Load: %s : %i vertices in %f", fileName.c_str(), mesh->GetNumVertices(), timer.Time());

	return mesh;
}

std::size_t MGLFileMGL::DetermineFileSize(const GLuint numVertices, const GLuint numIndices, const GLint colourVal) {
	// number of floats in vectors
	GLuint vec4Size = 4;
	GLuint vec3Size = 3;
	GLuint vec2Size = 2;

	GLuint total = 5; // fileVersion, type, numVertices, numIndices, colourVal

	total += numVertices * (vec3Size * 2); // vertices, normals
	total += numVertices * vec2Size; // texCoords

	if (colourVal == -1) {
		total += vec4Size; // colours
	}
	else if (colourVal > 0) {
		total += colourVal * vec4Size; // colours
	}

	total += numIndices;

	return (std::size_t)total;
}

MGLMesh* MGLFileMGL::LoadMesh(const MGLvecf* buffer) {
	GLuint fileVersion, type, numVertices, numIndices;
	GLint colourVal;

	GLuint loc = 0;

	// build mesh from vector
	fileVersion = (GLuint)buffer->at(loc);
	type = (GLuint)buffer->at(++loc);
	numVertices = (GLuint)buffer->at(++loc);
	numIndices = (GLuint)buffer->at(++loc);
	colourVal = (GLint)buffer->at(++loc);

	MGLvecv3* vertices = new MGLvecv3(numVertices);
	MGLvecv2* texCoords = new MGLvecv2(numVertices);
	MGLvecv3* normals = new MGLvecv3(numVertices);
	MGLvecv4* colours = new MGLvecv4(numVertices);
	MGLvecu* indices = new MGLvecu(numIndices);

	MGLMesh* mesh = new MGLMesh();

	mesh->SetNumIndices(numIndices);
	mesh->SetNumVertices(numVertices);
	mesh->SetType(type);

	// vertices
	for (GLuint i = 0; i < numVertices; ++i) {
		glm::vec3 vec;
		vec.x = buffer->at(++loc);
		vec.y = buffer->at(++loc);
		vec.z = buffer->at(++loc);
		vertices->at(i) = vec;
	}
	mesh->SetVertices(vertices);

	// TexCoords
	for (GLuint i = 0; i < numVertices; ++i) {
		glm::vec2 vec;
		vec.x = buffer->at(++loc);
		vec.y = buffer->at(++loc);
		texCoords->at(i) = vec;
	}
	mesh->SetTexCoords(texCoords);

	// Normals
	for (GLuint i = 0; i < numVertices; ++i) {
		glm::vec3 vec;
		vec.x = buffer->at(++loc);
		vec.y = buffer->at(++loc);
		vec.z = buffer->at(++loc);
		normals->at(i) = vec;
	}
	mesh->SetNormals(normals);

	// Colours 
	if (colourVal < 1)
	{
		glm::vec4 vec;
		if (colourVal == -1) {
			vec.x = buffer->at(++loc);
			vec.y = buffer->at(++loc);
			vec.z = buffer->at(++loc);
			vec.w = buffer->at(++loc);
		}
		else if (colourVal == 0) {
			vec = MGL::WHITE;
		}

		mesh->SetColours(colours);
		mesh->SetNewColours(vec, GL_FALSE);

	}
	else {
		for (GLuint i = 0; i < numVertices; ++i) {
			glm::vec4 vec;
			vec.x = buffer->at(++loc);
			vec.y = buffer->at(++loc);
			vec.z = buffer->at(++loc);
			vec.w = buffer->at(++loc);
			colours->at(i) = vec;
		}
		mesh->SetColours(colours);
	}

	// Indices
	for (GLuint i = 0; i < numIndices; ++i) {
		GLuint temp;
		temp = (GLuint)buffer->at(++loc);
		indices->at(i) = temp;
	}
	mesh->SetIndices(indices);

	// tidy up
	vertices = nullptr;
	texCoords = nullptr;
	normals = nullptr;
	colours = nullptr;
	indices = nullptr;

	return mesh;
}

/**********************************/
/*********** MGLFileOBJ ***********/
/**********************************/

MGLMesh* MGLFileOBJ::Load(std::string fileName, GLboolean bufferData) {
	MGLTimer timer = MGLTimer();
	timer.Start();

	// load file into stringstream
	std::stringstream* stream = LoadFileToSS(fileName);

#ifdef MGL_USER_INCLUDE_FILETC
	try {
		MGLException_Null::Test(stream);
	}
	catch (MGLException_Null& e) {
		//std::cerr << e.what() << ": STRINGSTREAM LoadOBJ " << std::endl;
		MGLH_Log->AddLog(MGL_LOG_ERROR, GL_TRUE, "%s%s", e.what(), "StringStream LoadOBJ ");

		return new MGLMesh(MGL_MESH_TRIANGLE);
	}
#endif // MGL_USER_INCLUDE_FILETC

	MGLObjFileData* obj = new MGLObjFileData();
	// init data with *blanks* for defaults
	obj->inputVertices.push_back(glm::vec3(0, 0, 0));
	obj->inputNormals.push_back(glm::vec3(0, 0, 0));
	obj->inputTexCoords.push_back(glm::vec2(0, 0));

	std::string token;

	// parse file from line start and build vertex data
	while (!stream->eof()) {
		token = "";
		*stream >> token;

		if (token[0] == '#') { // comment
			std::string line;  // remove line
			std::getline(*stream, line);
			continue;
		}
		else if (token[0] == 'v') {
			if (token[1] == 't') { // texture
				glm::vec2 vec;
				*stream >> (GLfloat)vec.x >> (GLfloat)vec.y;
				obj->inputTexCoords.push_back(vec);
			}
			else if (token[1] == 'n') { // normal
				glm::vec3 vec;
				*stream >> (GLfloat)vec.x >> (GLfloat)vec.y >> (GLfloat)vec.z;
				obj->inputNormals.push_back(vec);
			}
			else { // vertex
				glm::vec3 vec;
				*stream >> (GLfloat)vec.x >> (GLfloat)vec.y >> (GLfloat)vec.z;
				obj->inputVertices.push_back(vec);
			}
		}
		else if (token[0] == 'f') { // face
			std::string line;
			std::getline(*stream, line);

			if (line.length() < 5) { // ignore face errors (certain files)
				continue;
			}

			if (!HandleOBJFace(line, obj)) // exception caught?
				return new MGLMesh(MGL_MESH_TRIANGLE);
		}
	}

	// create the mesh
	MGLMesh* mesh = CreateMesh(obj);

	delete obj;
	delete stream;

#ifdef MGL_USER_INCLUDE_FILETC
	// test if mesh caused an error
	try {
		MGLException_Null::Test(mesh);
	}
	catch (MGLException& e) {
		//std::cerr << e.what() << " Error creating mesh" << std::endl;
		MGLH_Log->AddLog(MGL_LOG_ERROR, GL_TRUE, "%s%s", e.what(), " Error creating mesh");
		delete mesh;
		return new MGLMesh(MGL_MESH_TRIANGLE);
	}
#endif // MGL_USER_INCLUDE_FILETC

	if (bufferData)
		mesh->BufferAllData();

	timer.End();
	MGLH_Log->AddLog(MGL_LOG_MAIN, GL_TRUE, "OBJ Load: %s : %i vertices in %f", fileName.c_str(), mesh->GetNumVertices(), timer.Time());

	return mesh;
}

GLboolean MGLFileOBJ::HandleOBJFace(std::string line, MGLObjFileData* obj) {
	MGLvecu indices;
	GLuint data = 0;
	GLuint spaceCounter = 0;
	GLuint slashCounter = 0;

	GLboolean skipTex = GL_FALSE;
	GLboolean skipNorm = GL_FALSE;
	GLboolean lastCharSpace = GL_FALSE;
	GLboolean isPolygon = GL_FALSE;

	if (line.find("//") != std::string::npos) { // no textures
		skipTex = GL_TRUE;
	}

	for (GLchar& c : line) {
		if (c == ' ') { // count spaces ( = number of vertices in face)
			lastCharSpace = GL_TRUE;
		}
		else if (c == '/') { // count slashes
			c = ' ';
			slashCounter++;
		}
		else if (lastCharSpace) { // valid spaces have numbers after
			spaceCounter++;
			lastCharSpace = GL_FALSE;
		}
	}

	if (slashCounter / spaceCounter == 1 && !skipTex) { // 2*slashes if normals are skipped
		skipNorm = GL_TRUE;
	}

	if (spaceCounter == 4) // 4 sets of vertices in face
		isPolygon = GL_TRUE;

	// create easier data handle
	std::stringstream stream(line);
	while (stream >> data) {
		indices.push_back(data);
	}

#ifdef MGL_USER_INCLUDE_FILETC
	// check if the correct amount of indices have been loaded
	try {
		GLuint numIndices = (GLuint)indices.size();
		GLuint verts = isPolygon ? 4 : 3;
		GLuint skip = (skipTex || skipNorm) ? 2 : 3;
		GLuint shouldBe = verts * skip;

		MGLException_IsNotEqual::Test(numIndices, shouldBe);
	}
	catch (MGLException& e) {
		//std::cerr << e.what() << std::endl;
		MGLH_Log->AddLog(MGL_LOG_ERROR, GL_TRUE, e.what());

		return GL_FALSE;
	}
#endif // MGL_USER_INCLUDE_FILETC

	if (skipTex) {
		AddFaceMissing(obj, indices, isPolygon, GL_TRUE);
	}
	else if (skipNorm) {
		AddFaceMissing(obj, indices, isPolygon, GL_FALSE);
	}
	else { // v1/vt1/nv1 v1/vt2/nv2
		AddFace(obj, indices, isPolygon);
	}

	return GL_TRUE;
}

void MGLFileOBJ::AddFace(MGLObjFileData* obj, MGLvecu& indices, const GLboolean isPolygon) {

	const GLuint vertices = isPolygon ? 4 : 3;

	MGLvecu ids;

	for (GLuint i = 0; i < vertices * 3; i += 3) {
		MGLObjVertData vert = MGLObjVertData();
		vert.vertex = indices[i];
		vert.texture = indices[i + 1];
		vert.normals = indices[i + 2];

		// get id of unique vertex
		GLuint id = AddOBJVertToList(obj, vert);
		ids.push_back(id);
	}

	obj->finalIndices.push_back(ids[0]);
	obj->finalIndices.push_back(ids[1]);
	obj->finalIndices.push_back(ids[2]);

	if (isPolygon) {
		obj->finalIndices.push_back(ids[0]);
		obj->finalIndices.push_back(ids[2]);
		obj->finalIndices.push_back(ids[3]);
	}
}

void MGLFileOBJ::AddFaceMissing(MGLObjFileData* obj, MGLvecu& indices, const GLboolean isPolygon, const GLboolean skipTex) {

	const GLuint vertices = isPolygon ? 4 : 3;

	MGLvecu ids;

	for (GLuint i = 0; i < vertices * 2; i += 2) {
		MGLObjVertData vert = MGLObjVertData();
		vert.vertex = indices[i];
		vert.texture = skipTex ? 0 : indices[i + 1];
		vert.normals = skipTex ? indices[i + 1] : 0;

		// get id of unique vertex
		GLuint id = AddOBJVertToList(obj, vert);
		ids.push_back(id);
	}

	obj->finalIndices.push_back(ids[0]);
	obj->finalIndices.push_back(ids[1]);
	obj->finalIndices.push_back(ids[2]);

	if (isPolygon) {
		obj->finalIndices.push_back(ids[0]);
		obj->finalIndices.push_back(ids[2]);
		obj->finalIndices.push_back(ids[3]);
	}
}

GLuint MGLFileOBJ::AddOBJVertToList(MGLObjFileData* obj, MGLObjVertData& vert) {
	GLboolean vertFound = GL_FALSE;
	GLuint idOfVert = 0;

	// find a vertex with the same data
	for (MGLObjVertData& v : obj->objVertexList) {
		// if found, return id of found vertex
		if (v.Compare(vert)) {
			vertFound = GL_TRUE;
			idOfVert = v.id;
		}
	}

	// if one cant be found create a new one
	if (!vertFound) {
		vert.id = ++obj->idCounter;
		obj->objVertexList.push_back(vert);
		idOfVert = vert.id;
	}

	return idOfVert;
}

MGLMesh* MGLFileOBJ::CreateMesh(MGLObjFileData* obj) {
	GLuint numVertices = (GLuint)obj->objVertexList.size();

	// check if the correct amount of vertices have been loaded
	try {
		GLuint maxVert = 0, maxTex = 0, maxNorm = 0;

		for (MGLObjVertData& data : obj->objVertexList) {
			maxVert = (data.vertex > maxVert) ? data.vertex : maxVert;
			maxTex = (data.texture > maxTex) ? data.texture : maxTex;
			maxNorm = (data.normals > maxNorm) ? data.normals : maxNorm;
		}

		MGLException_IsLessThan::Test(obj->inputVertices.size(), ++maxVert);
		MGLException_IsLessThan::Test(obj->inputTexCoords.size(), ++maxTex);
		MGLException_IsLessThan::Test(obj->inputNormals.size(), ++maxNorm);
	}
	catch (MGLException& e) {
		//std::cerr << e.what() << "Mesh size error" << std::endl;
		MGLH_Log->AddLog(MGL_LOG_ERROR, GL_TRUE, "%s%s", e.what(), "Mesh size error");

		return nullptr;
	}

	MGLvecu* indices = new MGLvecu;
	MGLvecv3* vertices = new MGLvecv3;
	MGLvecv2* texCoords = new MGLvecv2;
	MGLvecv3* normals = new MGLvecv3;
	MGLvecv4* colours = new MGLvecv4;

	for (MGLObjVertData& data : obj->objVertexList) {
		vertices->push_back(obj->inputVertices.at(data.vertex));
		texCoords->push_back(obj->inputTexCoords.at(data.texture));
		normals->push_back(obj->inputNormals.at(data.normals));
	}

	for (GLuint i = 0; i < numVertices; ++i) {
		colours->push_back(MGL::WHITE);
	}

	for (GLuint ind : obj->finalIndices) {
		indices->push_back(ind - 1);
	}

	MGLMesh* mesh = new MGLMesh();

	if (indices->size() != 0)
		mesh->SetIndices(indices);
	if (colours->size() != 0)
		mesh->SetColours(colours);
	if (normals->size() != 0)
		mesh->SetNormals(normals);
	if (texCoords->size() != 0)
		mesh->SetTexCoords(texCoords);

	mesh->SetVertices(vertices);
	mesh->SetNumVertices(numVertices);
	mesh->SetNumIndices((GLuint)indices->size());

	return mesh;
}


