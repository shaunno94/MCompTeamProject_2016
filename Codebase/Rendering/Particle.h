#pragma once
#include "GameObject.h"

class Particle : public GameObject
{
public:
	Particle(GameObject* parent, Matrix4Simple initialTransform, const std::string& name = "Particle");
	virtual ~Particle() {}

	const Vec3Graphics& GetPosition() const { return m_Position; }
	const Vec3Graphics& GetOldPosition() const { return m_OldPos; }
	const Vec3Physics& GetVelocity() const { return m_Velocity; }

	void SetPosition(const Vec3Graphics& v) { m_Position = v; }
	void SetOldPosition(const Vec3Graphics& v) { m_OldPos = v; }
	void SetVelocity(const Vec3Graphics& v) { m_Velocity = v; }

	float GetLife() const { return m_Life; }
	float GetAge() const { return m_Age; }

	void SetLife(float f) { m_Life = f; }
	void SetAge(float f) { m_Age = f; }	
	bool spawned = false;
	Matrix4Simple carGhostLoc;

protected:
	virtual void OnUpdateObject(float dt) override;
	virtual void OnRenderObject() override;

private:
	Vec3Graphics m_Position;
	Vec3Graphics m_OldPos;
	Vec3Physics m_Velocity;

	const float MAX_LIFE = 1000;
	const float MIN_LIFE = 700;
	float m_Life;
	float m_Age;

};