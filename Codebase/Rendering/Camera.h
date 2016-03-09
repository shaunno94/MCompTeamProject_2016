/******************************************************************************
Class:Camera
Implements:
Author:Rich Davison	<richard.davison4@newcastle.ac.uk>
Description:FPS-Style camera. Uses the mouse and keyboard from the Window
class to get movement values!

-_-_-_-_-_-_-_,------,   
_-_-_-_-_-_-_-|   /\_/\   NYANYANYAN
-_-_-_-_-_-_-~|__( ^ .^) /
_-_-_-_-_-_-_-""  ""   

*//////////////////////////////////////////////////////////////////////////////
#pragma once

#include "Window.h"
#include "Math/nclglMath.h"
#include "GameObject.h"

/// @ingroup Rendering
/// <summary>
/// 
/// </summary>
class Camera	{
public:
	Camera(void){
		position = Vec3Graphics(0.0f, 0.0f, 0.0f);
		yaw		= 0.0f;
		pitch	= 0.0f;
	};

	Camera(float pitch, float yaw, Vec3Graphics position){
		this->pitch		= pitch;
		this->yaw		= yaw;
		this->position	= position;
	}

	Camera(GameObject* player){
		this->player = player;
		this->pitch = 0.0;
		this->yaw = 90.0;
		this->position = player->GetWorldTransform().GetTranslation() + player->GetWorldTransform().GetRotation() * Vector3Simple(0, 5, -5);
	}


	~Camera(void){};

	void reset(){
		pitch = -10.0;
		yaw = 90.0;
		if (player){
			yaw = acos((player->GetWorldTransform().GetRotation() * Vec3Graphics(1, 0, 0)).Normalize().Dot(Vec3Graphics(1, 0, 0)))* 57.2958;

			if ((player->GetWorldTransform().GetRotation() * Vec3Graphics(0, 0, 1)).Normalize().Dot(Vec3Graphics(1, 0, 0)) < 0)
				yaw = 360 - yaw;		
		}
	}

	void UpdateCamera(float msec = 10.0f);

	//Builds a view matrix for the current camera variables, suitable for sending straight
	//to a vertex shader (i.e it's already an 'inverse camera matrix').
	Mat4Graphics BuildViewMatrix();

	//Gets position in world space
	Vec3Graphics GetPosition() const;
	//Sets position in world space
	void	SetPosition(Vec3Graphics val) { position = val;}

	//Gets yaw, in degrees
	float	GetYaw()   const { return yaw;}
	//Sets yaw, in degrees
	void	SetYaw(float y) {yaw = y;}

	//Gets pitch, in degrees
	float	GetPitch() const { return pitch;}
	//Sets pitch, in degrees
	void	SetPitch(float p) {pitch = p;}

	bool autocam = false;

protected:
	float	yaw;
	float	pitch;
	Vec3Graphics position;
	GameObject* player;
};