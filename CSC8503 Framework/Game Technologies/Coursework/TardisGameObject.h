#pragma once
#include "ncltech\SimpleMeshObject.h"
class TardisGameObject : public SimpleMeshObject {
 private:
	float m_Alpha;
	float m_MaxAlpha;
	float m_Period;
	float m_Accumulator;

public:
	GameObject* m_Roof;
	GameObject* m_Base;
	GameObject* m_Light;

	TardisGameObject(void);
	~TardisGameObject(void);

	void OnUpdateObject(float dt) override;
};

