
#include <iostream>
#include "Helpers/common.h"
#include "Helpers/File.h"
#include "ModelLoader.h"
#include "Rendering/constants.h"

int main()
{

	std::cout << "Give filepath starting from " << MODEL_DIR << ":" << LINE_SEPARATOR;
	std::string filepath;
	std::cin >> filepath;

	filepath = MODEL_DIR + filepath;
	string newFilepath = File::RemoveFileExt(filepath) + ".mgl";

	Mesh* mesh = ModelLoader::LoadOBJ(filepath, false);
	if (!mesh->GetTangents())
	{
		std::cout << "Generate Tangents? (yes 1 / 0 no)" << LINE_SEPARATOR;
		int genTangents;
		std::cin >> genTangents;
		if (genTangents)
			mesh->GenerateTangents();
	}

	ModelLoader::SaveMeshToMGL(mesh, newFilepath);
}