/******************************************************************************
Class: Hull
Implements:
Author: Pieran Marris <p.marris@newcastle.ac.uk>
Description: 
This is an elaborate and 'slow' version of the Mesh class. It keeps track of
faces, vertices and edges of a given mesh aswell as all the adjancy information.

This means that you can retrieve a face and instanty have a list of all of it's 
adjancent faces without having to do expensive lookups (the expensive part is that
these are all calculated when the hull is created).

They can be quite useful for debugging shapes and experimenting with new 3D algorithms. 
In this framework they are used to represent discrete collision shapes which have clear edges, 
such as the cuboidCollisionShape.


Note: One of the big changes from a normal Mesh is that the faces can have any number of vertices,
so you could have triangles that link to a quad, or a pentagon for instance. Though I haven't actually
found a use for mixing this up yet... maybe a square based pyramid?


		(\_/)
		( '_')
	 /""""""""""""\=========     -----D
	/"""""""""""""""""""""""\
....\_@____@____@____@____@_/

*//////////////////////////////////////////////////////////////////////////////

#pragma once

#include "Math/nclglMath.h"
#include "Game Technologies/Memory/STLManagedAllocator.h"
#include <vector>

struct HullEdge;
struct HullFace;

struct HullVertex
{
	int idx;
	Vec3Physics pos;
	std::vector<int> enclosing_edges;
	std::vector<int> enclosing_faces;
};

struct HullEdge
{
	int idx;
	int vStart, vEnd;
	std::vector<int> adjoining_edge_ids;
	std::vector<int> enclosing_faces;
};

struct HullFace
{
	int idx;
	Vec3Physics normal;
	std::vector<int> vert_ids;
	std::vector<int> edge_ids;
	std::vector<int> adjoining_face_ids;
};

class Hull
{
public:
	Hull();
	~Hull();

	void AddVertex(const Vec3Physics& v);
	

	void AddFace(const Vec3Physics& normal, int nVerts, const int* verts);
	void AddFace(const Vec3Physics& normal, const std::vector<int>& vert_ids)		{ AddFace(normal, vert_ids.size(), &vert_ids[0]); }


	int FindEdge(int v0_idx, int v1_idx);
	

	const HullVertex& GetVertex(int idx)		{ return m_Vertices[idx]; }
	const HullEdge& GetEdge(int idx)			{ return m_Edges[idx]; }
	const HullFace& GetFace(int idx)			{ return m_Faces[idx]; }

	size_t GetNumVertices()					{ return m_Vertices.size(); }
	size_t GetNumEdges()					{ return m_Edges.size(); }
	size_t GetNumFaces()					{ return m_Faces.size(); }


	void GetMinMaxVerticesInAxis(const Vec3Physics& local_axis, int* out_min_vert, int* out_max_vert);


	void DebugDraw(const Mat4Physics& transform);

protected:
	int ConstructNewEdge(int parent_face_idx, int vert_start, int vert_end); //Called by AddFace
	
protected:
	std::vector<HullVertex, STLManagedAllocator<HullVertex, 16>>		m_Vertices;
	std::vector<HullEdge>		m_Edges;
	std::vector<HullFace>		m_Faces;

};