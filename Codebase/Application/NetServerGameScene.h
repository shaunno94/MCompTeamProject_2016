#ifndef ORBIS

#pragma once
#include "Networking\NetServer.h"
#include "GameScene.h"

struct GameCollisionFilter;

class NetServerGameScene : public GameScene
{
public:
	NetServerGameScene();
	~NetServerGameScene();

	void Setup() override;
	void Cleanup() override;
};

#endif