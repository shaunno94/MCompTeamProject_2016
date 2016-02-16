#pragma once
#include "ncltech\SimpleMeshObject.h"
#include "ncltech\TrackerGameObject.h"
#include "ncltech\Constraint.h"

class GoalkeeperGameObject : public SimpleMeshObject {
 protected:
	PhysicsObject* m_Guarding;
	float m_Speed;
	float m_GuardRadius;
	std::mutex m_ThreatUpdateMutex;
	std::vector<PhysicsObject*> m_ThreatAccumulator;
	std::vector<PhysicsObject*> m_Threats;
	PhysicsObject* m_Target;
	TrackerGameObject* m_Tracker;
	Constraint* m_Constraint;

	struct GoalkeeperTargetCompare {
		GoalkeeperTargetCompare(PhysicsObject* target) { referencePoint = target; }
		PhysicsObject* referencePoint;
		bool operator()(PhysicsObject* lhs, PhysicsObject* rhs) const {
			const Vec3Physics& pos = referencePoint->GetPosition();
			return  (referencePoint->GetPosition() - lhs->GetPosition()).LengthSq() < (referencePoint->GetPosition() - rhs->GetPosition()).LengthSq();
		}
	};

	void Fallow();

 public:
	GoalkeeperGameObject(PhysicsObject* guarding, float guardRadius);
	~GoalkeeperGameObject(void);

	virtual void OnUpdateObject(float dt);

	void SetSpeed(float speed) { m_Speed = speed; }
	float GetSpeed() const { return m_Speed; }

};

