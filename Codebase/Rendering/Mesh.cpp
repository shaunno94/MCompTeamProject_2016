#include "Mesh.h"
#include <map>
#include <cstdint>
#include <iostream>
#include <cmath>

Mesh::Mesh(void)	{
	glGenVertexArrays(1, &arrayObject);

	for(int i = 0; i < MAX_BUFFER; ++i)
		bufferObject[i] = 0;

	texture		 = 0;
	numVertices  = 0;
	type		 = GL_TRIANGLES;

	//Later tutorial stuff
	numIndices    = 0;
	bumpTexture	  = 0;
	vertices	  = NULL;
	textureCoords = NULL;
	normals		  = NULL;
	tangents	  = NULL;
	indices		  = NULL;
	colours		  = NULL;

	transformCoords = true;
}

Mesh::~Mesh(void)	{
	glDeleteVertexArrays(1, &arrayObject);			//Delete our VAO
	glDeleteBuffers(MAX_BUFFER, bufferObject);		//Delete our VBOs

	glDeleteTextures(1,&texture);					//We'll be nice and delete our texture when we're done with it
	glDeleteTextures(1,&bumpTexture);				//We'll be nice and delete our texture when we're done with it

	//Later tutorial stuff
	delete[]vertices;
	delete[]indices;
	delete[]textureCoords;
	delete[]tangents;
	delete[]normals;
	delete[]colours;
}

void Mesh::Draw(bool update)	{
	if(update) {
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, bumpTexture);
	}

	glBindVertexArray(arrayObject);
	if(bufferObject[INDEX_BUFFER])
		glDrawElements(type, numIndices, GL_UNSIGNED_INT, 0);
	else {
		glDrawArrays(type, 0, numVertices);	//Draw the triangle!
	}
	glBindVertexArray(0);
}

Mesh* Mesh::GenerateTriangle()	{
	Mesh* m = new Mesh();
	m->numVertices = 3;

	m->vertices = new Vec3Graphics[m->numVertices];
	m->vertices[0] = Vec3Graphics(0.0f,	0.5f,	0.0f);
	m->vertices[1] = Vec3Graphics(0.5f,  -0.5f,	0.0f);
	m->vertices[2] = Vec3Graphics(-0.5f, -0.5f,	0.0f);

	m->textureCoords = new Vec2Graphics[m->numVertices];
	m->textureCoords[0] = Vec2Graphics(0.5f,	0.0f);
	m->textureCoords[1] = Vec2Graphics(1.0f,	1.0f);
	m->textureCoords[2] = Vec2Graphics(0.0f,	1.0f);

	m->colours = new Vec4Graphics[m->numVertices];
	m->colours[0] = Vec4Graphics(1.0f, 0.0f, 0.0f,1.0f);
	m->colours[1] = Vec4Graphics(0.0f, 1.0f, 0.0f,1.0f);
	m->colours[2] = Vec4Graphics(0.0f, 0.0f, 1.0f,1.0f);

	m->GenerateNormals();
	m->GenerateTangents();
	m->BufferData();

	return m;
}

unsigned int IcosphereGetMidPoint(int p1, int p2, std::map<int64_t, unsigned int>& savedPoints, std::vector<Vec3Graphics>& vertices, unsigned int& currentIndex) {
	// first check if we have it already
	bool firstIsSmaller = p1 < p2;
	int64_t smallerIndex = firstIsSmaller ? p1 : p2;
	int64_t greaterIndex = firstIsSmaller ? p2 : p1;
	int64_t key = (smallerIndex << 32) + greaterIndex;

	auto savedMatch = savedPoints.find(key);
	if(savedMatch != savedPoints.end())
		return savedMatch->second;

	//calculate new point
	Vec3Graphics middle = (vertices[p1] + vertices[p2]) * 0.5f;

	// add vertex makes sure point is on unit sphere
	vertices.push_back(middle.Normalize());

	// store it, return index
	savedPoints.insert(std::pair<uint64_t, unsigned int>(key, currentIndex));
	return currentIndex++;
}

Mesh* Mesh::GenerateIcosphere(unsigned int tessalationLevel) {
	struct TriangleIndices {
	 public:
		unsigned int v1;
		unsigned int v2;
		unsigned int v3;

		TriangleIndices(unsigned int v1, unsigned int v2, unsigned int v3) : v1(v1), v2(v2), v3(v3) {}
	};
	std::map<int64_t, unsigned int> tessalatedPoints;
	std::vector<Vec3Graphics> vertices;
	std::vector<unsigned int> indices;
	unsigned int currentIndex = 0;

	if(tessalationLevel == 0) return nullptr;

	// create 12 vertices of a icosahedron
	float offset = (1.0f + sqrt(5.0f)) * 0.5f;

	vertices.push_back(Vec3Graphics(-1,  offset,  0));
	vertices.push_back(Vec3Graphics(1,  offset,  0));
	vertices.push_back(Vec3Graphics(-1, -offset,  0));
	vertices.push_back(Vec3Graphics(1, -offset,  0));

	vertices.push_back(Vec3Graphics(0, -1,  offset));
	vertices.push_back(Vec3Graphics(0,  1,  offset));
	vertices.push_back(Vec3Graphics(0, -1, -offset));
	vertices.push_back(Vec3Graphics(0,  1, -offset));

	vertices.push_back(Vec3Graphics(offset,  0, -1));
	vertices.push_back(Vec3Graphics(offset,  0,  1));
	vertices.push_back(Vec3Graphics(-offset,  0, -1));
	vertices.push_back(Vec3Graphics(-offset,  0,  1));


	// create 20 triangles of the icosahedron
	auto faces = new std::vector<TriangleIndices>();

	// 5 faces around point 0
	faces->push_back(TriangleIndices(0, 11, 5));
	faces->push_back(TriangleIndices(0, 5, 1));
	faces->push_back(TriangleIndices(0, 1, 7));
	faces->push_back(TriangleIndices(0, 7, 10));
	faces->push_back(TriangleIndices(0, 10, 11));

	// 5 adjacent faces
	faces->push_back(TriangleIndices(1, 5, 9));
	faces->push_back(TriangleIndices(5, 11, 4));
	faces->push_back(TriangleIndices(11, 10, 2));
	faces->push_back(TriangleIndices(10, 7, 6));
	faces->push_back(TriangleIndices(7, 1, 8));

	// 5 faces around point 3
	faces->push_back(TriangleIndices(3, 9, 4));
	faces->push_back(TriangleIndices(3, 4, 2));
	faces->push_back(TriangleIndices(3, 2, 6));
	faces->push_back(TriangleIndices(3, 6, 8));
	faces->push_back(TriangleIndices(3, 8, 9));

	// 5 adjacent faces
	faces->push_back(TriangleIndices(4, 9, 5));
	faces->push_back(TriangleIndices(2, 4, 11));
	faces->push_back(TriangleIndices(6, 2, 10));
	faces->push_back(TriangleIndices(8, 6, 7));
	faces->push_back(TriangleIndices(9, 8, 1));


	// refine triangles
	for(int i = 0; i < tessalationLevel; i++) {
		auto tempFaces = new std::vector<TriangleIndices>();
		for(auto tri : *faces) {
			// replace triangle by 4 triangles
			int a = IcosphereGetMidPoint(tri.v1, tri.v2, tessalatedPoints, vertices, currentIndex);
			int b = IcosphereGetMidPoint(tri.v2, tri.v3, tessalatedPoints, vertices, currentIndex);
			int c = IcosphereGetMidPoint(tri.v3, tri.v1, tessalatedPoints, vertices, currentIndex);

			tempFaces->push_back(TriangleIndices(tri.v1, a, c));
			tempFaces->push_back(TriangleIndices(tri.v2, b, a));
			tempFaces->push_back(TriangleIndices(tri.v3, c, b));
			tempFaces->push_back(TriangleIndices(a, b, c));
		}
		delete faces;
		faces = tempFaces;
	}

	// done, now add triangles to mesh
	for(auto tri : *faces) {
		indices.push_back(tri.v1);
		indices.push_back(tri.v2);
		indices.push_back(tri.v3);
	}

	Mesh* m = new Mesh();
	m->numVertices = vertices.size();
	m->vertices = new Vec3Graphics[m->numVertices];
	for(unsigned int i = 0; i < m->numVertices; ++i) {
		m->vertices[i] = vertices[i];
	}
	m->numIndices = indices.size();
	m->indices = new unsigned int[m->numIndices];
	unsigned int counter = 0;
	for(auto face : *faces) {
		m->indices[counter++] = face.v1;
		m->indices[counter++] = face.v2;
		m->indices[counter++] = face.v3;
	}

	m->GenerateNormals();
	m->GenerateTangents();
	m->BufferData();

	return m;
}


Mesh* Mesh::GenerateQuad()	{
	Mesh* m = new Mesh();

	m->numVertices = 4;
	m->type = GL_TRIANGLE_STRIP;

	m->vertices			= new Vec3Graphics[m->numVertices];
	m->textureCoords	= new Vec2Graphics[m->numVertices];
	m->colours			= new Vec4Graphics[m->numVertices];
	m->normals			= new Vec3Graphics[m->numVertices];
	m->tangents			= new Vec3Graphics[m->numVertices];

	m->vertices[0] = 	Vec3Graphics(-1.0f, -1.0f, 0.0f);
	m->vertices[1] = 	Vec3Graphics(-1.0f,	1.0f, 0.0f);
	m->vertices[2] = 	Vec3Graphics(1.0f, -1.0f, 0.0f);
	m->vertices[3] = 	Vec3Graphics(1.0f,  1.0f, 0.0f);

	m->textureCoords[0] = Vec2Graphics(0.0f,	1.0f);
	m->textureCoords[1] = Vec2Graphics(0.0f,	0.0f);
	m->textureCoords[2] = Vec2Graphics(1.0f,	1.0f);
	m->textureCoords[3] = Vec2Graphics(1.0f,	0.0f);

	for(int i = 0; i < 4; ++i) {
		m->colours[i] = Vec4Graphics(1.0f, 1.0f,1.0f,1.0f);
		m->normals[i] = Vec3Graphics(0.0f, 0.0f,-1.0f);
		m->tangents[i] = Vec3Graphics(1.0f, 0.0f,0.0f);
	}

	m->BufferData();

	return m;
}


Mesh* Mesh::GenerateQuadAlt()	{
	Mesh* m = new Mesh();

	m->numVertices = 4;
	m->type = GL_TRIANGLE_STRIP;

	m->vertices			= new Vec3Graphics[m->numVertices];
	m->textureCoords	= new Vec2Graphics[m->numVertices];
	m->colours			= new Vec4Graphics[m->numVertices];
	m->normals			= new Vec3Graphics[m->numVertices];
	m->tangents			= new Vec3Graphics[m->numVertices];

	m->vertices[0] = 	Vec3Graphics(0.0f, 0.0f, 0.0f);
	m->vertices[1] = 	Vec3Graphics(0.0f, 1.0f, 0.0f);
	m->vertices[2] = 	Vec3Graphics(1.0f, 0.0f, 0.0f);
	m->vertices[3] = 	Vec3Graphics(1.0f,  1.0f, 0.0f);

	m->textureCoords[0] = Vec2Graphics(0.0f,	0.0f);
	m->textureCoords[1] = Vec2Graphics(0.0f,	1.0f);
	m->textureCoords[2] = Vec2Graphics(1.0f,	0.0f);
	m->textureCoords[3] = Vec2Graphics(1.0f,	1.0f);

	for(int i = 0; i < 4; ++i) {
		m->colours[i] = Vec4Graphics(1.0f, 1.0f,1.0f,1.0f);
		m->normals[i] = Vec3Graphics(0.0f, 0.0f,-1.0f);
		m->tangents[i] = Vec3Graphics(1.0f, 0.0f,0.0f);
	}

	m->BufferData();

	return m;
}
//NX 01/11/2012
Mesh* Mesh::GenerateQuadTexCoordCol(Vec2Graphics scale, Vec2Graphics texCoord, Vec4Graphics colour)	{
	Mesh* m = new Mesh();

	m->numVertices = 4;
	m->type = GL_TRIANGLE_STRIP;

	m->vertices			= new Vec3Graphics[m->numVertices];
	m->textureCoords	= new Vec2Graphics[m->numVertices];
	m->colours			= new Vec4Graphics[m->numVertices];
	m->normals			= new Vec3Graphics[m->numVertices];
	m->tangents			= new Vec3Graphics[m->numVertices];

	m->vertices[0] = 	Vec3Graphics(0.0f, 0.0f, 0.0f);
	m->vertices[2] = 	Vec3Graphics(0.0f, scale.y, 0.0f);
	m->vertices[1] = 	Vec3Graphics(scale.x, 0.0f, 0.0f);
	m->vertices[3] = 	Vec3Graphics(scale.x,  scale.y, 0.0f);

	m->textureCoords[0] = Vec2Graphics(texCoord.x,	texCoord.x);
	m->textureCoords[2] = Vec2Graphics(texCoord.x,	texCoord.y);
	m->textureCoords[1] = Vec2Graphics(texCoord.y,	texCoord.x);
	m->textureCoords[3] = Vec2Graphics(texCoord.y,	texCoord.y);

	for(int i = 0; i < 4; ++i) {
		m->colours[i] = colour;
		m->normals[i] = Vec3Graphics(0.0f, 0.0f, 1.0f);
		m->tangents[i] = Vec3Graphics(1.0f, 0.0f,0.0f);
	}

	m->BufferData();

	return m;
}

void	Mesh::BufferData()	{
	//GenerateNormals();
	//GenerateTangents();

	glBindVertexArray(arrayObject);

	//Buffer vertex data
	glGenBuffers(1, &bufferObject[VERTEX_BUFFER]);
	glBindBuffer(GL_ARRAY_BUFFER, bufferObject[VERTEX_BUFFER]);
	glBufferData(GL_ARRAY_BUFFER, numVertices*sizeof(Vec3Graphics), vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(VERTEX_BUFFER, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(VERTEX_BUFFER);

	//Buffer texture data
	if(textureCoords) {
		glGenBuffers(1, &bufferObject[TEXTURE_BUFFER]);
		glBindBuffer(GL_ARRAY_BUFFER, bufferObject[TEXTURE_BUFFER]);
		glBufferData(GL_ARRAY_BUFFER, numVertices*sizeof(Vec2Graphics), textureCoords, GL_STATIC_DRAW);
		glVertexAttribPointer(TEXTURE_BUFFER, 2, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(TEXTURE_BUFFER);
	}

	//buffer colour data
	if(colours)	{
		glGenBuffers(1, &bufferObject[COLOUR_BUFFER]);
		glBindBuffer(GL_ARRAY_BUFFER, bufferObject[COLOUR_BUFFER]);
		glBufferData(GL_ARRAY_BUFFER, numVertices*sizeof(Vec4Graphics), colours, GL_STATIC_DRAW);
		glVertexAttribPointer(COLOUR_BUFFER, 4, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(COLOUR_BUFFER);
	}

	//Buffer normal data
	if(normals) {
		glGenBuffers(1, &bufferObject[NORMAL_BUFFER]);
		glBindBuffer(GL_ARRAY_BUFFER, bufferObject[NORMAL_BUFFER]);
		glBufferData(GL_ARRAY_BUFFER, numVertices*sizeof(Vec3Graphics), normals, GL_STATIC_DRAW);
		glVertexAttribPointer(NORMAL_BUFFER, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(NORMAL_BUFFER);
	}

	//Buffer tangent data
	if(tangents) {
		glGenBuffers(1, &bufferObject[TANGENT_BUFFER]);
		glBindBuffer(GL_ARRAY_BUFFER, bufferObject[TANGENT_BUFFER]);
		glBufferData(GL_ARRAY_BUFFER, numVertices*sizeof(Vec3Graphics), tangents, GL_STATIC_DRAW);
		glVertexAttribPointer(TANGENT_BUFFER, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(TANGENT_BUFFER);
	}

	//buffer index data
	if(indices) {
		glGenBuffers(1, &bufferObject[INDEX_BUFFER]);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufferObject[INDEX_BUFFER]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, numIndices*sizeof(GLuint), indices, GL_STATIC_DRAW);
	}

	glBindVertexArray(0);
}

/*
Stuff for later tutorials...
*/

void	Mesh::GenerateNormals()	{
	if(!normals)
		normals = new Vec3Graphics[numVertices];
	for(GLuint i = 0; i < numVertices; ++i)
		normals[i] = Vec3Graphics();

	if(indices) {
		GLuint i = 0;

		int test = 0;
		for(i = 0; i < numIndices; i+=3) {
			int a = indices[i];
			int b = indices[i+1];
			int c = indices[i+2];

			Vec3Graphics normal = (vertices[b]-vertices[a]).Cross(vertices[c]-vertices[a]);

			normals[a] += normal;
			normals[b] += normal;
			normals[c] += normal;

			test+=3;
		}
		bool asdf = true;
	} else {
		//It's just a list of triangles, so generate face normals
		for(GLuint i = 0; i < numVertices; i+=3) {
			Vec3Graphics& a = vertices[i];
			Vec3Graphics& b = vertices[i+1];
			Vec3Graphics& c = vertices[i+2];

			Vec3Graphics normal = (b-a).Cross(c-a);

			normals[i]	 = normal;
			normals[i+1] = normal;
			normals[i+2] = normal;
		}
	}

	for(GLuint i = 0; i < numVertices; ++i)
		normals[i].Normalize();
}

void Mesh::GenerateTangents() {
	//Extra! stops rare occurrence of this function being called
	//on a mesh without tex coords, which would break quite badly!
	if(!textureCoords)
		return;

	if(!tangents)
		tangents = new Vec3Graphics[numVertices];
	for(GLuint i = 0; i < numVertices; ++i)
		tangents[i] = Vec3Graphics();

	if(indices) {
		for(GLuint i = 0; i < numIndices; i+=3) {
			int a = indices[i];
			int b = indices[i+1];
			int c = indices[i+2];

			Vec3Graphics tangent = GenerateTangent(vertices[a],vertices[b],vertices[c],textureCoords[a],textureCoords[b],textureCoords[c]);

			tangents[a] += tangent;
			tangents[b] += tangent;
			tangents[c] += tangent;
		}
	} else {
		for(GLuint i = 0; i < numVertices; i+=3) {
			Vec3Graphics tangent = GenerateTangent(vertices[i],vertices[i+1],vertices[i+2],textureCoords[i],textureCoords[i+1],textureCoords[i+2]);

			tangents[i]   += tangent;
			tangents[i+1] += tangent;
			tangents[i+2] += tangent;
		}
	}
	for(GLuint i = 0; i < numVertices; ++i)
		tangents[i].Normalize();
}

Vec3Graphics Mesh::GenerateTangent(const Vec3Graphics& a,const Vec3Graphics& b,const Vec3Graphics& c,const Vec2Graphics& ta,const Vec2Graphics& tb,const Vec2Graphics& tc)	 {
	Vec2Graphics coord1  = tb-ta;
	Vec2Graphics coord2  = tc-ta;

	Vec3Graphics vertex1 = b-a;
	Vec3Graphics vertex2 = c-a;

	Vec3Graphics axis = Vec3Graphics(vertex1*coord2.y - vertex2*coord1.y);

	float factor = 1.0f / (coord1.x * coord2.y - coord2.x * coord1.y);

	return axis * factor;
}

void Mesh::DrawDebugNormals(float length)	{
	if(numVertices > 0) {
		GLuint array;
		GLuint buffer;
		GLuint cbuffer;

		glGenVertexArrays(1, &array);
		glBindVertexArray(array);

		Vec3Graphics* tempV = new Vec3Graphics[numVertices*2];
		Vec3Graphics* tempC = new Vec3Graphics[numVertices*2];

		for(GLuint i = 0; i < numVertices; ++i) {
			tempC[(i*2)]  = Vec3Graphics(1,0,0);
			tempC[(i*2)+1] = Vec3Graphics(1,1,1);

			tempV[(i*2)]   = vertices[i];
			tempV[(i*2)+1] = vertices[i] + (normals[i] * length);
		}

		glGenBuffers(1, &buffer);
		glBindBuffer(GL_ARRAY_BUFFER, buffer);
		glBufferData(GL_ARRAY_BUFFER, numVertices*sizeof(Vec3Graphics)*2, tempV, GL_STREAM_DRAW);
		glVertexAttribPointer(VERTEX_BUFFER, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(VERTEX_BUFFER);

		glGenBuffers(1, &cbuffer);
		glBindBuffer(GL_ARRAY_BUFFER, cbuffer);
		glBufferData(GL_ARRAY_BUFFER, numVertices*sizeof(Vec3Graphics)*2, tempC, GL_STREAM_DRAW);
		glVertexAttribPointer(COLOUR_BUFFER, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(COLOUR_BUFFER);

		glPointSize(4.0f);
		glLineWidth(2.0f);
		glDrawArrays(GL_POINTS,  0, numVertices*2);	// draw ordered list of vertices
		glDrawArrays(GL_LINES,  0, numVertices*2);	// draw ordered list of vertices
		glLineWidth(1.0f);
		glPointSize(1.0f);

		glBindVertexArray(0);

		glDeleteVertexArrays(1, &array);
		glDeleteBuffers(1, &buffer);
		glDeleteBuffers(1, &cbuffer);

		delete[] tempV;
		delete[] tempC;
	}

	//for(unsigned int i = 0; i < children.size(); ++i) {
	//	children.at(i)->DrawDebugNormals();
	//}
}

void Mesh::DrawDebugTangents(float length)	{
	if(numVertices > 0) {
		GLuint array;
		GLuint buffer;
		GLuint cbuffer;

		glGenVertexArrays(1, &array);
		glBindVertexArray(array);

		Vec3Graphics* tempV = new Vec3Graphics[numVertices*2];
		Vec3Graphics* tempC = new Vec3Graphics[numVertices*2];

		for(GLuint i = 0; i < numVertices; ++i) {
			tempC[(i*2)]  = Vec3Graphics(1,1,1);
			tempC[(i*2)+1] = Vec3Graphics(0,1,0);

			tempV[(i*2)]   = vertices[i];
			tempV[(i*2)+1] = vertices[i] + (tangents[i] * length);
		}

		glGenBuffers(1, &buffer);
		glBindBuffer(GL_ARRAY_BUFFER, buffer);
		glBufferData(GL_ARRAY_BUFFER, numVertices*sizeof(Vec3Graphics)*2, tempV, GL_STREAM_DRAW);
		glVertexAttribPointer(VERTEX_BUFFER, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(VERTEX_BUFFER);

		glGenBuffers(1, &cbuffer);
		glBindBuffer(GL_ARRAY_BUFFER, cbuffer);
		glBufferData(GL_ARRAY_BUFFER, numVertices*sizeof(Vec3Graphics)*2, tempC, GL_STREAM_DRAW);
		glVertexAttribPointer(COLOUR_BUFFER, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(COLOUR_BUFFER);

		glPointSize(4.0f);
		glLineWidth(2.0f);
		glDrawArrays(GL_POINTS,  0, numVertices*2);	// draw ordered list of vertices
		glDrawArrays(GL_LINES,  0, numVertices*2);	// draw ordered list of vertices
		glLineWidth(1.0f);
		glPointSize(1.0f);

		glBindVertexArray(0);

		glDeleteVertexArrays(1, &array);
		glDeleteBuffers(1, &buffer);
		glDeleteBuffers(1, &cbuffer);

		delete[] tempV;
		delete[] tempC;
	}

	//for(unsigned int i = 0; i < children.size(); ++i) {
	//	children.at(i)->DrawDebugTangents();
	//}
}