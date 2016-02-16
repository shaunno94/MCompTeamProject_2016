#include "GoalkeeperGameObject.h"
#include "ncltech\CommonMeshes.h"
#include "ncltech\SphereCollisionShape.h"
#include "ncltech\PhysicsEngine.h"
#include "ncltech\DistanceConstraint.h"
#include "ncltech\TrackerGameObject.h"


GoalkeeperGameObject::GoalkeeperGameObject(PhysicsObject* guarding, float guardRadius) : SimpleMeshObject("Goalkeeper") {
	SetMesh(CommonMeshes::Sphere(), false);
	SetColour(Vec4Graphics::ONES);
	SetBoundingRadius(1.0f);
	Physics()->SetCollisionShape(new SphereCollisionShape(1.0f));
	Physics()->NeverRest(true);
	m_Speed = 15.0f;
	m_Guarding = guarding;
	m_Target = nullptr;
	PhysicsObject* keeper = Physics();
	Physics()->SetInverseMass(1.0f / 100.0f);

	CollisionListener* listener = new CollisionListener();
	listener->callback = [&, keeper](PhysicsObject * obj) {
		if (obj != keeper) {
			std::lock_guard<std::mutex> guard(m_ThreatUpdateMutex);
			auto match = std::find(m_ThreatAccumulator.begin(), m_ThreatAccumulator.end(), obj);
			if (match == m_ThreatAccumulator.end()) {
				m_ThreatAccumulator.push_back(obj);
				InsertionSort(m_ThreatAccumulator.begin(), m_ThreatAccumulator.end(), GoalkeeperTargetCompare(m_Guarding));
			}
		}
	};

	m_Tracker = new TrackerGameObject(m_Guarding, guardRadius);
	m_Tracker->AddCollisionListener(listener);

	Physics()->SetPosition(m_Tracker->Physics()->GetPosition() + Vec3Physics(0.0f, guardRadius * 0.3f, 0.0f));
	m_Constraint = new DistanceConstraint(Physics(), m_Tracker->Physics(), Physics()->GetPosition(), m_Tracker->Physics()->GetPosition());

	PhysicsEngine::Instance()->AddPhysicsObject(m_Tracker->Physics());
	PhysicsEngine::Instance()->AddConstraint(m_Constraint);

	SetTexture(SOIL_load_OGL_texture(TEXTUREDIR"bricks1.jpg", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT), true);
}


GoalkeeperGameObject::~GoalkeeperGameObject(void) {
	PhysicsEngine::Instance()->RemoveConstraint(m_Constraint);
	delete m_Constraint;
}

void GoalkeeperGameObject::OnUpdateObject(float dt) {
	m_Target = nullptr;
	Physics()->SetLinearVelocity(Vec3Physics::ZEROS);
	if (m_Guarding) {
		if (m_ThreatAccumulator.size() > 0) {
			{
				std::lock_guard<std::mutex> guard(m_ThreatUpdateMutex);
				m_Threats = m_ThreatAccumulator;
				m_ThreatAccumulator.clear();
			}
			if (Physics()->IsEnabled()) {
				const Vec3Physics& myPos = Physics()->GetPosition();
				const Vec3Physics& guardingPos = m_Guarding->GetPosition();
				float myDist = (guardingPos - myPos).LengthSq();
				for (PhysicsObject* threat : m_Threats) {
					Vec3Physics threatToGuarding = guardingPos - threat->GetPosition();
					float threatDist = threatToGuarding.LengthSq();
					threatToGuarding.Normalize();
					float projectedMovingDir = threatToGuarding.Dot(threat->GetLinearVelocity().Normal());
					//has to not be passed goalkeeper reach and moving in the direction of the tardis
					if (threatDist > myDist && projectedMovingDir > 0.3f) {
						m_Target = threat;
						Fallow();
						break;
					}
				}
			}
		}
	}
};


void GoalkeeperGameObject::Fallow() {
	static const Mat4Physics rotationY = Mat4Physics::RotationY(90);
	const Vec3Physics& myPos = Physics()->GetPosition();
	Vec3Physics toTarget = m_Target->GetPosition() - myPos;
	toTarget.Normalize();
	Vec3Physics toGuarding = m_Guarding->GetPosition() - myPos;
	toGuarding.Normalize();
	float projection = toGuarding.Dot(toTarget);
	//on the other side
	if (0.8f <= projection && projection <= 1.1f) {
		Vec3Physics movingDirection = rotationY * toGuarding;
		Physics()->SetLinearVelocity(movingDirection * m_Speed);
		//NCLDebug::DrawHairLine(myPos , myPos + movingDirection);
		return;
	}
	//if in front
	if (-1.1f <= projection && projection <= -0.9f)
		return;

	toGuarding *= projection;
	Vec3Physics movingDirection = toTarget - toGuarding;
	movingDirection.Normalize();

	Physics()->SetLinearVelocity(movingDirection * (m_Speed /** (projection + 1.0)*/));
}