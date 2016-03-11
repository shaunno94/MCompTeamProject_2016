#pragma once

enum SCENES
{
	MENU_SCENE,
	GAME_SCENE,
	SERVER_SETUP_SCENE,
	CLIENT_SETUP_SCENE,
	SERVER_GAME_SCENE,
	CLIENT_GAME_SCENE
};


#ifndef ORBIS
#include "Rendering\KeyboardController.h"
const std::string SIMPLESHADER_VERT = SHADER_DIR"textureVertex.glsl";
const std::string SIMPLESHADER_FRAG = SHADER_DIR"textureFragment.glsl";
const std::string COLOURSHADER_FRAG = SHADER_DIR"colourTextureFragment.glsl";
const std::string POINTLIGHTSHADER_VERT = SHADER_DIR"2dShadowLightvertex.glsl";
const std::string POINTLIGHTSHADER_FRAG = SHADER_DIR"2dShadowLightfragment.glsl";
const std::string GUI_VERT = SHADER_DIR"TexturedVertex.glsl";
const std::string GUI_FRAG = SHADER_DIR"TexturedFragment.glsl";
#else
#include "Rendering\PS4Controller.h"
const std::string SIMPLESHADER_VERT = SHADER_DIR"textureVertex.sb";
const std::string SIMPLESHADER_FRAG = SHADER_DIR"textureFragment.sb";
const std::string COLOURSHADER_FRAG = SHADER_DIR"colourTextureFragment.sb";
const std::string POINTLIGHTSHADER_VERT = SHADER_DIR"2dShadowLightvertex.sb";
const std::string POINTLIGHTSHADER_FRAG = SHADER_DIR"2dShadowLightfragment.sb";
const std::string GUI_VERT = SHADER_DIR"TexturedVertex.sb";
const std::string GUI_FRAG = SHADER_DIR"TexturedFragment.sb";
#endif