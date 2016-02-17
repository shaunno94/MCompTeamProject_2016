#include "Camera.h"

/*
Polls the camera for keyboard / mouse movement.
Should be done once per frame! Pass it the msec since
last frame (default value is for simplicities sake...)
*/
void Camera::UpdateCamera(float msec)	{
	float dt = msec * 0.001f;
	float speed = 7.0f * dt; //1.5m per second
#ifndef ORBIS
	//Update the mouse by how much
	pitch -= (Window::GetMouse()->GetRelativePosition().y);
	yaw -= (Window::GetMouse()->GetRelativePosition().x);


	//Bounds check the pitch, to be between straight up and straight down ;)
	pitch = fmin(pitch, 90.0f);
	pitch = fmax(pitch, -90.0f);

	if (yaw < 0)
		yaw += 360.0f;
	if (yaw > 360.0f)
		yaw -= 360.0f;


	if (Window::GetKeyboard()->KeyDown(KEYBOARD_W))
		position += Mat4Graphics::Rotation(yaw, Vec3Graphics(0, 10, 0)) * Vec3Graphics(0, 0, -10) * speed;
	if (Window::GetKeyboard()->KeyDown(KEYBOARD_S))
		position -= Mat4Graphics::Rotation(yaw, Vec3Graphics(0, 10, 0)) * Vec3Graphics(0, 0, -10) * speed;

	if (Window::GetKeyboard()->KeyDown(KEYBOARD_A))
		position += Mat4Graphics::Rotation(yaw, Vec3Graphics(0, 10, 0)) * Vec3Graphics(-10, 0, 0) * speed;
	if (Window::GetKeyboard()->KeyDown(KEYBOARD_D))
		position -= Mat4Graphics::Rotation(yaw, Vec3Graphics(0, 10, 0)) * Vec3Graphics(-10, 0, 0) * speed;

	if (Window::GetKeyboard()->KeyDown(KEYBOARD_SHIFT))
		position.y += speed * 5.0f;
	if (Window::GetKeyboard()->KeyDown(KEYBOARD_SPACE))
		position.y -= speed * 5.0f;
#endif 		
}

/*
Generates a view matrix for the camera's viewpoint. This matrix can be sent
straight to the shader...it's already an 'inverse camera' matrix.
*/
Mat4Graphics Camera::BuildViewMatrix()	{
	//Why do a complicated matrix inversion, when we can just generate the matrix
	//using the negative values ;). The matrix multiplication order is important!
	return	Mat4Graphics::Rotation(-pitch, Vec3Graphics(1, 0, 0)) *
	        Mat4Graphics::Rotation(-yaw, Vec3Graphics(0, 1, 0)) *
	        Mat4Graphics::Translation(-position);
};
