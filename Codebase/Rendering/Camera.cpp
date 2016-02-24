#include "Camera.h"

/*
Polls the camera for keyboard / mouse movement.
Should be done once per frame! Pass it the msec since
last frame (default value is for simplicities sake...)
*/
void Camera::UpdateCamera(float msec)	{
	if (player){
		//position = player->GetWorldTransform().GetTranslation() + player->GetWorldTransform().GetRotation() * Vector3Simple(0, 5, -10);
		position = 
			Mat4Graphics::Translation(player->GetWorldTransform().GetTranslation()) *
			Mat4Graphics::Rotation(yaw, Vec3Graphics(0, 1, 0)) *
			Mat4Graphics::Translation(Vector3Simple(0, 15, 25)) *
			Mat4Graphics::Rotation(pitch, Vec3Graphics(1, 0, 0)) *
			Vector3Simple(0, 1, 1);

		//position.y = 10;
	}


		float dt = msec * 0.001f;
		float speed = 7.0f * dt; //1.5m per second
	
		//Update the mouse by how much
		player->GetControllerComponent()->getCameraControl(pitch, yaw);
	
}

/*
Generates a view matrix for the camera's viewpoint. This matrix can be sent
straight to the shader...it's already an 'inverse camera' matrix.
*/
Mat4Graphics Camera::BuildViewMatrix()	{
	//Why do a complicated matrix inversion, when we can just generate the matrix
	//using the negative values ;). The matrix multiplication order is important!

	/*if (player)
		return Mat4Graphics::View(position, player->GetWorldTransform().GetTranslation());
	else*/{
		return	Mat4Graphics::Rotation(-pitch, Vec3Graphics(1, 0, 0)) *
			Mat4Graphics::Rotation(-yaw, Vec3Graphics(0, 1, 0)) *
			Mat4Graphics::Translation(-position);
	}
};
