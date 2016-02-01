#include "CubeRobot.h"

//cube is a static class variable, and so must be initialised outside of the 
//class...here's as good a place as any!
Mesh* CubeRobot::cube = NULL;

CubeRobot::CubeRobot(void)	{	
	if(!cube) {
		CreateCube();
	}
	SetMesh(cube);

	//Make the body
	SceneNode*body = new SceneNode(cube,Vec4Graphics(1,0,0,1));
	body->SetModelScale(Vec3Graphics(10,15,5));
	body->SetTransform(Mat4Graphics::Translation(Vec3Graphics(0,35,0)));
	AddChild(body);

	//Add the head
	head = new SceneNode(cube,Vec4Graphics(0,1,0,1));
	head->SetModelScale(Vec3Graphics(5,5,5));
	head->SetTransform(Mat4Graphics::Translation(Vec3Graphics(0,30,0)));
	body->AddChild(head);

	//Add the left arm
	leftArm = new SceneNode(cube,Vec4Graphics(0,0,1,1));
	leftArm->SetModelScale(Vec3Graphics(3,-18,3));
	leftArm->SetTransform(Mat4Graphics::Translation(Vec3Graphics(-12,30,-1)));
	body->AddChild(leftArm);

	//Add the right arm
	rightArm = new SceneNode(cube,Vec4Graphics(0,0,1,1));
	rightArm->SetModelScale(Vec3Graphics(3,-18,3));
	rightArm->SetTransform(Mat4Graphics::Translation(Vec3Graphics(12,30,-1)));
	body->AddChild(rightArm);

	//Add the left leg
	leftLeg = new SceneNode(cube,Vec4Graphics(0,0,1,1));
	leftLeg->SetModelScale(Vec3Graphics(3,-17.5,3));
	leftLeg->SetTransform(Mat4Graphics::Translation(Vec3Graphics(-8,0,0)));
	body->AddChild(leftLeg);

	//Finally the right leg!
	rightLeg = new SceneNode(cube,Vec4Graphics(0,0,1,1));
	rightLeg->SetModelScale(Vec3Graphics(3,-17.5,3));
	rightLeg->SetTransform(Mat4Graphics::Translation(Vec3Graphics(8,0,0)));
	body->AddChild(rightLeg);

	//Giant CubeRobot!
	//transform = Mat4Graphics::Scale(Vec3Graphics(10,10,10));

	//The Scene Management Tutorial introduces these, as cheap culling tests
	body->SetBoundingRadius(15.0f);
	head->SetBoundingRadius(5.0f);

	leftArm->SetBoundingRadius(18.0f);
	rightArm->SetBoundingRadius(18.0f);

	leftLeg->SetBoundingRadius(18.0f);
	rightLeg->SetBoundingRadius(18.0f);
}

void CubeRobot::Update(float msec) {
	transform = transform * Mat4Graphics::Rotation(msec / 10.0f,Vec3Graphics(0,1,0));

	head->SetTransform(Mat4Graphics::Rotation(-msec / 10.0f,Vec3Graphics(0,1,0))* head->GetTransform());
	leftArm->SetTransform(leftArm->GetTransform() * Mat4Graphics::Rotation(-msec / 10.0f,Vec3Graphics(1,0,0)));
	rightArm->SetTransform(rightArm->GetTransform() * Mat4Graphics::Rotation(msec / 10.0f,Vec3Graphics(1,0,0)));

	SceneNode::Update(msec);
}