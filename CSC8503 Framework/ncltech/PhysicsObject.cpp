#include "PhysicsObject.h"
#include "PhysicsEngine.h"

PhysicsObject::PhysicsObject(GameObject* parent) {
	//Initialise Defaults
	m_wsTransformInvalidated = true;
	m_Enabled	= true;

	m_Position = Vec3Physics(0.0f, 0.0f, 0.0f);
	m_LinearVelocity = Vec3Physics(0.0f, 0.0f, 0.0f);
	m_Force = Vec3Physics(0.0f, 0.0f, 0.0f);
	m_InvMass = 0.0f;

	m_Orientation = QuatPhysics(0.0f, 0.0f, 0.0f, 1.0f);
	m_AngularVelocity = Vec3Physics(0.0f, 0.0f, 0.0f);
	m_Torque = Vec3Physics(0.0f, 0.0f, 0.0f);
	m_InvInertia.ToZero();

	m_colShape = nullptr;

	m_Friction = 0.5f;

	m_Parent = parent;
	m_OnCollisionCallback = [](PhysicsObject * colliding_obj) { return true; };

	m_RestTestCounter = 0;
	m_AtRest = false;
	m_NeverRest = false;
	m_OldOrientation = m_Orientation;
	m_ExternalRotate = QuatPhysics(0.0f, 0.0f, 0.0f, 1.0f);
}

PhysicsObject::~PhysicsObject() {
	//Delete ColShape
	if(m_colShape != nullptr) {
		delete m_colShape;
		m_colShape = nullptr;
	}
}

std::mutex PhysicsObject::g_ExternalChanges;

void PhysicsObject::PrepareRenderTransformUpdate() {
	m_wsTempTransform = GetWorldSpaceTransform();
}

void PhysicsObject::UpdateRenderTransform() {
	m_wsRenderTransform = m_wsTempTransform;
}

const Mat4Physics& PhysicsObject::GetWorldSpaceTransform() const {
	if(m_wsTransformInvalidated) {
		m_wsTransform = m_Orientation.ToMatrix4();
		m_wsTransform.SetTranslation(m_Position);

		m_wsTransformInvalidated = false;
	}

	return m_wsTransform;
}

const Mat4Graphics& PhysicsObject::GetWorldRenderTransform() const {
	return m_wsRenderTransform;
}

bool PhysicsObject::RestTest() {
	bool orientationChanged = abs(m_Orientation.x - m_OldOrientation.x) > REST_MAX_DISPLACEMENT_SQ || abs(m_Orientation.y - m_OldOrientation.y) > REST_MAX_DISPLACEMENT_SQ || abs(m_Orientation.y - m_OldOrientation.y) > REST_MAX_DISPLACEMENT_SQ || abs(m_Orientation.w - m_OldOrientation.w) > REST_MAX_DISPLACEMENT_SQ;
	Vec3Physics newPosition = GetWorldSpaceTransform().GetTranslation();
	//wakeup
	if(orientationChanged || (newPosition - m_OldPosition).LengthSq() > REST_MAX_DISPLACEMENT_SQ) {
		m_RestTestCounter = 0;
		m_AtRest = false;
		m_OldOrientation = m_Orientation;
		m_OldPosition = newPosition;
	} else {
		if(!m_AtRest && m_Enabled)
			if(++m_RestTestCounter >= REST_TESTS) {
				m_AtRest = true;
				m_RestTestCounter = REST_TESTS;
			}
	}
	return AtRest();
}

void PhysicsObject:: UpdateExternalChange() {
	SetOrientation(GetOrientation() * m_ExternalRotate);
	m_ExternalRotate = QuatPhysics(0.0f, 0.0f, 0.0f, 1.0f);
	SetPosition(GetPosition() + m_ExternalMove);
	m_ExternalMove = Vec3Physics::ZEROS;
}