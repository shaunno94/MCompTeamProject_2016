#ifndef ORBIS

#pragma once
#include "Networking\NetServer.h"
#include "GameScene.h"

struct GameCollisionFilter;

class NetClientGameScene : public GameScene
{
public:
	NetClientGameScene();
	~NetClientGameScene();

	void UpdateScene(float dt) override;

	void Setup() override;
	void Cleanup() override;
};

#endif