#include "Camera.h"
#include "Renderer.h"

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

		//Update the mouse by how much
				player->GetControllerComponent()->getCameraControl(pitch, yaw);
	}

	
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

	//player controled
	/*return	Mat4Graphics::Rotation(-pitch, Vec3Graphics(1, 0, 0)) *
	Mat4Graphics::Rotation(-yaw, Vec3Graphics(0, 1, 0)) *
	Mat4Graphics::Translation(-position);*/

	//lock to ball
	GameObject* ball = Renderer::GetInstance()->GetCurrentScene()->findGameObject("ball");

	if (!ball || !player || !autocam)
		return	Mat4Graphics::Rotation(-pitch, Vec3Graphics(1, 0, 0)) *
			Mat4Graphics::Rotation(-yaw, Vec3Graphics(0, 1, 0)) *
			Mat4Graphics::Translation(-position);

	Vec3Graphics ballPos = ball->GetWorldTransform().GetTranslation();
	return	 Mat4Graphics::View(GetPosition(), ballPos);

}
}


Vec3Graphics Camera::GetPosition() const
{
	GameObject* ball = Renderer::GetInstance()->GetCurrentScene()->findGameObject("ball");
	if (!ball || autocam)
	{
		Vec3Graphics ballPos = ball->GetWorldTransform().GetTranslation();
		Vec3Graphics playerPos = player->GetWorldTransform().GetTranslation() + Vec3Graphics(0, 15, 0);
		Vec3Graphics direction = playerPos - ballPos;
		direction.Normalize();
		Vec3Graphics tempPos = playerPos + (direction * 30);
		tempPos.y = std::max(tempPos.y, 15.0f);
		return tempPos;
	}
	return position;
	}
