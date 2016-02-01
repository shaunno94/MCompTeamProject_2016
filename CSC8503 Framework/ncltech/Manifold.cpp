#include "Manifold.h"
#include "GameObject.h"
#include "Math/nclglMath.h"
#include <ncltech/NCLDebug.h>
#include "PhysicsEngine.h"

#define persistentThresholdSq 0.025f

typedef std::list<Contact> ContactList;
typedef ContactList::iterator ContactListItr;

Manifold::Manifold(PhysicsObject* nodeA, PhysicsObject* nodeB) : m_NodeA(nodeA), m_NodeB(nodeB) {
}

Manifold::~Manifold() {

}

void Manifold::ApplyImpulse() {
	float softness = (m_NodeA->GetInverseMass() + m_NodeB->GetInverseMass()) / m_Contacts.size();

	for (Contact& contact : m_Contacts) {
		contact.normal.softness = softness;
		contact.normal.ApplyImpulse();

		float friction_limit = contact.normal.impulseSum;
		contact.friction1.impulseSumMin = -friction_limit;
		contact.friction1.impulseSumMax = friction_limit;

		contact.friction2.impulseSumMin = -friction_limit;
		contact.friction2.impulseSumMax = friction_limit;

		contact.friction1.ApplyImpulse();
		contact.friction2.ApplyImpulse();
	}
}

void Manifold::PreSolverStep(float dt) {
	for (Contact& conatct : m_Contacts)
		UpdateConstraint(conatct);
}

void Manifold::UpdateConstraint(Contact& contact) {
	Vec3Physics r1 = contact.relPosA;
	Vec3Physics r2 = contact.relPosB;

	Vec3Physics v1 = m_NodeA->GetLinearVelocity() + (m_NodeA->GetAngularVelocity().Cross(r1));
	Vec3Physics v2 = m_NodeB->GetLinearVelocity() + (m_NodeB->GetAngularVelocity().Cross(r2));

	Vec3Physics dv = v2 - v1;
	Vec3Physics tangent1 = dv - (contact.collisionNormal * dv.Dot(contact.collisionNormal));
	if (tangent1.LengthSq() < 0.001f) {
		tangent1 = contact.collisionNormal.Cross(Vec3Physics(1.0f, 0.0f, 0.0f));
		if (tangent1.LengthSq() < 0.001f)
			tangent1 = contact.collisionNormal.Cross(Vec3Physics(0.0f, 0.0f, 1.0f));
	}
	tangent1.Normalize();

	Vec3Physics tangent2 = contact.collisionNormal.Cross(tangent1);
	tangent2.Normalize();
	float b = 0.0f;

	//Baumgarte Offset
	{
		const float baumgarte_scalar = 0.1f;
		const float baumgarte_slop = 0.02f;

		float penetration_slop = std::min(contact.collisionPenetration + baumgarte_slop, 0.0f);
		b += (baumgarte_scalar / PhysicsEngine::Instance()->GetDeltaTime()) * penetration_slop;
	}

	{
		const float elasticity = 0.6f;
		const float elasticity_slop = 0.5f;

		float elasticity_term = elasticity * contact.collisionNormal.Dot(
		                          - m_NodeA->GetLinearVelocity()
		                          - (r1.Cross(m_NodeA->GetAngularVelocity()))
		                          + m_NodeB->GetLinearVelocity()
		                          + r2.Cross(m_NodeB->GetAngularVelocity())
		                        );
		b += std::min(elasticity_term + elasticity_slop, 0.0f);
	}

	float friction = m_NodeA->GetFriction() * m_NodeB->GetFriction();
	contact.normal = Constraint(m_NodeA, m_NodeB,
	                            -contact.collisionNormal,
	                            (-r1).Cross(contact.collisionNormal),
	                            contact.collisionNormal,
	                            r2.Cross(contact.collisionNormal),
	                            b
	                           );
	contact.normal.impulseSumMin = 0.0f;
	contact.friction1 = Constraint(m_NodeA, m_NodeB,
	                               -tangent1 * friction,
	                               (-r1).Cross(tangent1) * friction,
	                               tangent1 * friction,
	                               r2.Cross(tangent1) * friction,
	                               0.0f
	                              );
	contact.friction2 = Constraint(m_NodeA, m_NodeB,
	                               -tangent2 * friction,
	                               (-r1).Cross(tangent2) * friction,
	                               tangent2 * friction,
	                               r2.Cross(tangent2) * friction,
	                               0.0f
	                              );
}

void Manifold::AddContact(const Vec3Physics& globalOnA, const Vec3Physics& globalOnB, const Vec3Physics& normal, const float& penetration) {
	Vec3Physics r1 = (globalOnA - m_NodeA->GetPosition());
	Vec3Physics r2 = (globalOnB - m_NodeB->GetPosition());


	Contact contact;
	contact.relPosA = r1;
	contact.relPosB = r2;
	contact.collisionNormal = normal;
	contact.collisionPenetration = penetration;

	m_Contacts.push_back(contact);
}

void Manifold::DebugDraw() const {
	if (m_Contacts.size() > 0) {

		Vec3Physics globalOnA1 = m_NodeA->GetPosition() +  m_Contacts.back().relPosA;
		for (const Contact& contact : m_Contacts) {
			Vec3Physics globalOnA2 = m_NodeA->GetPosition() + contact.relPosA;
			Vec3Physics globalOnB = m_NodeB->GetPosition() + contact.relPosB;

			NCLDebug::DrawThickLine(globalOnA1, globalOnA2, 0.02f, Vec4Graphics(0.0f, 1.0f, 0.0f, 1.0f));
			NCLDebug::DrawPoint(globalOnA2, 0.05f, Vec4Graphics(0.0f, 0.5f, 0.0f, 1.0f));

			NCLDebug::DrawThickLine(globalOnB, globalOnA2, 0.01f, Vec4Graphics(1.0f, 0.0f, 1.0f, 1.0f));

			globalOnA1 = globalOnA2;
		}
	}
}