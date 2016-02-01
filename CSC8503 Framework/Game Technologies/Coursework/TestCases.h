#pragma once

#include "ncltech\Scene.h"
#include "ncltech\GameObject.h"

static class TestCases
{
	static std::vector<GameObject* > m_Resources;
public:
	static void AddSimpleStackedRestingTestcase(Scene* scene, const Vec3Physics& pos);
	static void AddSimpleSwingingTestcase(Scene* scene, const Vec3Physics& pos);
	static void AddPlaneTestcase(Scene* scene, const Vec3Physics& pos);
	static void AddStackedPyramidTestcase(Scene* scene, const Vec3Physics& pos, bool resting);
	static void AddWall(Scene* scene, const Vec3Physics& pos, const Vec3Physics& scale);
	static void AddSpheresCuboid(Scene* scene, const Vec3Physics& pos, const Vec3Physics& count);
	static void AddCuboidStack(Scene* scene, const Vec3Physics& pos, unsigned int size);

	static void ReleaseResources();
};

