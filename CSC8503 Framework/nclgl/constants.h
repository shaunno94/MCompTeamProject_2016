#pragma once

//4201-nameless structure;
#pragma warning(disable: 4201)

#include "../Dependencies/glew-1.13.0/include/GL/glew.h"
#include "../Dependencies/glew-1.13.0/include/GL/wglew.h"

#define SHADER_VERTEX   0
#define SHADER_FRAGMENT 1
#define SHADER_GEOMETRY 2
#define SHADER_TCS 3
#define SHADER_TES 4
#define SHADER_MAX_STAGE_COUNT 5

#define SHADER_DIR	"../Shaders/"
#define MESH_DIR	"../Meshes/"
#define TEXTURE_DIR  "../Textures/"
#define SOUNDS_DIR	"../Sounds/"

//using raw primitive arrays to prevent usage of classes with alignment and creating gaps in memory that will be sent to the GPU
//if all structures can be 16 byte aligned, (std140) can be taken out of the shaders:
//http://www.opentk.com/node/2926
struct ShaderCameraUBOData {
	float viewMatrix[16];
	float projMatrix[16];
	float vpMatrix[16];
	float camPos[3];
};

struct ShaderUniformInfo {
	unsigned int id;
	char* name;
};

static unsigned int shaderUniformCounter = 0;

static const struct ShaderUniformsStruct {
	const union {
		const struct  {
			ShaderUniformInfo MVP_MATRIX;
			ShaderUniformInfo MODEL_MATRIX;
			ShaderUniformInfo TEXTURE_MATRIX;

			ShaderUniformInfo TEXTURE_DIFFUSE;
			ShaderUniformInfo TEXTURE_BUMP;
			ShaderUniformInfo TEXTURE_HEIGHT;
			ShaderUniformInfo TEXTURE_EMISSIVE;
			ShaderUniformInfo TEXTURE_SPECULAR;
			ShaderUniformInfo TEXTURE_SKYBOX;
			ShaderUniformInfo TEXTURE_1;
			ShaderUniformInfo TEXTURE_2;
			ShaderUniformInfo TEXTURE_3;

			ShaderUniformInfo LIGHT_POINT;
			ShaderUniformInfo LIGHT_COLOR;

			ShaderUniformInfo BOUNDING_RADIUS;

			ShaderUniformInfo TESSALATION_INNER_DETAIL;
			ShaderUniformInfo TESSALATION_OUTER_DETAIL;
			ShaderUniformInfo HEIGHTMAP_OFFSET_MAX;

			ShaderUniformInfo COLOR;
		};
		ShaderUniformInfo values[19];
	};
	static const unsigned int SIZE = 19;
} ShaderUniforms = {
	shaderUniformCounter++, "_mvpMat4",
	shaderUniformCounter++, "_modelMat4",
	shaderUniformCounter++, "_texMat4",
	shaderUniformCounter++, "_texDiff",
	shaderUniformCounter++, "_texBump",
	shaderUniformCounter++, "_texHeight",
	shaderUniformCounter++, "_texEmiss",
	shaderUniformCounter++, "_texSpec",
	shaderUniformCounter++, "_texSkybox",
	shaderUniformCounter++, "_tex1",
	shaderUniformCounter++, "_tex2",
	shaderUniformCounter++, "_tex3",
	shaderUniformCounter++, "_lightPoint",
	shaderUniformCounter++, "_lightColor",
	shaderUniformCounter++, "_boundRadius",

	shaderUniformCounter++, "_tessInner",
	shaderUniformCounter++, "_tessOuter",
	shaderUniformCounter++, "_heightmapOffsetMax",
	shaderUniformCounter++, "_color"
};

static const struct ShaderUBOStruct {
	const union {
		const struct  {
			ShaderUniformInfo CAMERA;
		};
		ShaderUniformInfo values[1];
	};
	static const unsigned int SIZE = 1;
} ShaderUBOs = {
	shaderUniformCounter++, "ShaderUBOPerFrameData"
};

struct VBOInfo {
	unsigned int id;
	char* attributeName;
};

static const struct VBOsStruct {
	const union {
		const struct  {
			VBOInfo POSITION;
			VBOInfo TEXTURE_COORD;
			VBOInfo COLOR;
			VBOInfo NORMAL;
			VBOInfo TANGENT;
			VBOInfo INDEX;
		};
		VBOInfo values[6];
	};
	//disregard index buffer
	static const unsigned int SHADER_ATTRIB_COUNT = 5;
	static const unsigned int SIZE = 6;
} VBOs = {
	0, "position",
	1, "texCoord",
	2, "color",
	3, "normal",
	4, "tangent",
	5, "index"
};

