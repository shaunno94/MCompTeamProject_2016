#pragma once
#include "GameObject.h"
class WheelObject :
	public GameObject
{
public:
	WheelObject(const std::string& name = "");
	~WheelObject();

	void OnUpdateObject(float dt) override;
	void SetLocalTransform(const Mat4Graphics& transform) override
	{
		LocalTrasformWORotate = transform;
	}

	void setRotationFactor(float& factor){
		
		rotFactor = std::min(std::max(factor, -1.0f), 1.0f);
	}
protected:
	Mat4Graphics LocalTrasformWORotate;
	
	float rotFactor;
};

