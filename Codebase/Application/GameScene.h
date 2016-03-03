#pragma once

#include "Rendering\Scene.h"
#include "Rendering\Renderer.h"
#include "Rendering\OGLShader.h"
#include "Rendering\ModelLoader.h"
#include "Rendering\LocalControlManager.h"
#include "Stadium.h"
#include "Rendering\DebugDraw.h"
#include "Rendering\GUISystem.h"
#include "Rendering\WheelObject.h"
#include "CarGameObject.h"
#include "Audio\SoundSystem.h"

#ifndef ORBIS
#include "Rendering\KeyboardController.h"
const string SIMPLESHADER_VERT = SHADER_DIR"textureVertex.glsl";
const string SIMPLESHADER_FRAG = SHADER_DIR"textureFragment.glsl";
const string POINTLIGHTSHADER_VERT = SHADER_DIR"2dShadowLightvertex.glsl";
const string POINTLIGHTSHADER_FRAG = SHADER_DIR"2dShadowLightfragment.glsl";
const string GUI_VERT = SHADER_DIR"combineVert.glsl";

#else
#include "Rendering\PS4Controller.h"
const string SIMPLESHADER_VERT = SHADER_DIR"textureVertex.sb";
const string SIMPLESHADER_FRAG = SHADER_DIR"textureFragment.sb";
const string POINTLIGHTSHADER_VERT = SHADER_DIR"2dShadowLightvertex.sb";
const string POINTLIGHTSHADER_FRAG = SHADER_DIR"2dShadowLightfragment.sb";

#endif
#include "BulletCollision\CollisionDispatch\btCollisionWorld.h"

struct GameCollisionFilter : public btOverlapFilterCallback
{

public:

	int m_ballID = 0;
	int m_goal1ID = 0;
	int m_goal2ID = 0;

	virtual bool needBroadphaseCollision(btBroadphaseProxy* proxy0, btBroadphaseProxy* proxy1) const override
	{
		short int combined = COL_CAR & COL_WALL;
		/*int combinedMask = proxy0->m_collisionFilterMask | proxy1->m_collisionFilterMask;
		int test1 = (combinedMask & COL_CAR) == COL_CAR;
		int test2 = (combinedMask & COL_WALL) == COL_WALL;
		int test3;*/
		if ((proxy0->m_collisionFilterMask | proxy1->m_collisionFilterMask) & combined == combined)
		{
			int test = 0;
		}
		else
		{
			int test2 = 0;
		}
		//if (((combinedMask & COL_CAR) == COL_CAR) && ((combinedMask & COL_WALL) == COL_WALL)) {
		//	std::cout << "Car and wall collision" << std::endl;
		//	// sort out car-wall collision
		//}

		if ((proxy0->getUid() == m_ballID && proxy1->getUid() == m_goal1ID) ||
		    (proxy1->getUid() == m_ballID && proxy0->getUid() == m_goal1ID))
		{
			//TODO: Increment goals for team 1
			int ifojwe = 8;

			//TODO: Reset Scene
		}
		else if ((proxy0->getUid() == m_ballID && proxy1->getUid() == m_goal2ID) ||
		         (proxy1->getUid() == m_ballID && proxy0->getUid() == m_goal2ID))
		{
			//TODO: Increment goals for team 2
			int ifojwe = 8;

			//TODO: Reset Scene
		}
		return true;
	}
};


class GameScene : public Scene
{
public:
	GameScene(ControllerManager* controller);
	~GameScene();

	void SetControllerActor();
	void TestContact();

protected:
	ControllerManager* myControllers;
	GameCollisionFilter goalBallFilter;
};