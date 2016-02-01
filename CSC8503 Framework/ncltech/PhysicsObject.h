/******************************************************************************
Class: PhysicsObject
Implements:
Author: Rich Davison	<richard.davison4@newcastle.ac.uk>, Pieran Marris<p.marris@newcastle.ac.uk>
Description: This class represents the physical properties of your game's
entities - their position, orientation, mass, collision volume, and so on.

		(\_/)								-_-_-_-_-_-_-_,------,
		( '_')								_-_-_-_-_-_-_-|   /\_/\   NYANYANYAN
	 /""""""""""""\=========     -----D		-_-_-_-_-_-_-~|__( ^ .^) /
	/"""""""""""""""""""""""\				_-_-_-_-_-_-_-""  ""
....\_@____@____@____@____@_/

*//////////////////////////////////////////////////////////////////////////////
#pragma once
#include "Math/nclglMath.h"
#include "CollisionShape.h"
#include <functional>
#include <mutex>

#define REST_TESTS 300
#define REST_MAX_DISPLACEMENT_SQ 0.003f

class PhysicsEngine;

class PhysicsObject {
protected:
	mutable Mat4Physics		m_wsTransform;
	mutable Mat4Graphics	m_wsTempTransform;
	mutable Mat4Graphics	m_wsRenderTransform;
	mutable QuatPhysics		m_OldOrientation;
	mutable Vec3Physics		m_OldPosition;

	//<----------ANGULAR-------------->
	QuatPhysics  m_Orientation;
	QuatPhysics m_ExternalRotate;
	Mat3Physics     m_InvInertia;
	Vec3Physics		m_AngularVelocity;
	Vec3Physics		m_Torque;
	Vec3Physics m_ExternalMove;

	friend class PhysicsEngine;
	friend class GameObject; //Just to allow construction

 public:
	static std::mutex g_ExternalChanges;

	PhysicsObject(GameObject* parent = nullptr);
	~PhysicsObject();

	//<--------- GETTERS ------------->
	bool				IsEnabled()				const 	{ return m_Enabled; }
	void				IsEnabled(bool val)		 	{ m_Enabled = val; if (!val) m_AtRest = false; }

	float				GetFriction()			const 	{ return m_Friction; }

	const Vec3Physics&		GetPosition()			const 	{ return m_Position; }
	const Vec3Physics&		GetLinearVelocity()		const 	{ return m_LinearVelocity; }
	const Vec3Physics&		GetForce()				const 	{ return m_Force; }
	float				GetInverseMass()		const 	{ return m_InvMass; }

	const QuatPhysics&	GetOrientation()		const 	{ return m_Orientation; }
	const Vec3Physics&		GetAngularVelocity()	const 	{ return m_AngularVelocity; }
	const Vec3Physics&		GetTorque()				const 	{ return m_Torque; }
	const Mat3Physics&		GetInverseInertia()		const 	{ return m_InvInertia; }

	CollisionShape*		GetCollisionShape()		const 	{ return m_colShape; }

	const Mat4Physics& GetWorldSpaceTransform()     const;

	const Mat4Graphics& GetWorldRenderTransform()     const;

	inline bool AtRest() const { return (m_AtRest || m_InvMass <= 0.00001f) && !m_NeverRest; }
	inline void AtRest(bool val) { m_AtRest = val; }
	inline void NeverRest(bool val = true) { m_NeverRest = val; }

	//<--------- SETTERS ------------->
	inline void SetFriction(float friction)				{ m_Friction = friction; }

	inline void SetPosition(const Vec3Physics& v)				{ m_Position = v;	m_wsTransformInvalidated = true; }
	inline void SetLinearVelocity(const Vec3Physics& v)		{ m_LinearVelocity = v; }
	inline void SetForce(const Vec3Physics& v)					{ m_Force = v; }
	inline void SetInverseMass(const float& v)				{ m_InvMass = v; }

	inline void SetOrientation(const QuatPhysics& v)		{ m_Orientation = v; m_wsTransformInvalidated = true; }
	inline void SetAngularVelocity(const Vec3Physics& v)		{ m_AngularVelocity = v; }
	inline void SetTorque(const Vec3Physics& v)				{ m_Torque = v; }
	inline void SetInverseInertia(const Mat3Physics& v)		{ m_InvInertia = v; }

	inline void SetCollisionShape(CollisionShape* colShape)	{ m_colShape = colShape; }

	inline bool IsTransformInvalidated() { return m_wsTransformInvalidated; }

	inline void SetOnCollisionCallback(std::function<bool(PhysicsObject*)> onCollisionFunction) { m_OnCollisionCallback = onCollisionFunction; }
	inline std::function<bool(PhysicsObject*)> GetOnCollisionCallback() { return m_OnCollisionCallback; }

	void ExternalRotate(const QuatPhysics& rotation) { m_ExternalRotate = m_ExternalRotate * rotation; }
	void ExternalMove(const Vec3Physics& displacement) { m_ExternalMove += displacement; }
	void UpdateExternalChange();

	inline GameObject* GetParent() const { return m_Parent; }

	bool RestTest();

	MEM_ALIGN_NEW_DELETE

 protected:
	unsigned int m_RestTestCounter;
	bool m_AtRest;
	bool m_NeverRest;

	inline void SetParent(GameObject& parent) { m_Parent = &parent; }

	void PrepareRenderTransformUpdate();
	void UpdateRenderTransform();

	GameObject*			m_Parent;

	bool				m_Enabled;

	mutable bool		m_wsTransformInvalidated;

	float				m_Friction;

	//<---------LINEAR-------------->
	Vec3Physics		m_Position;
	Vec3Physics		m_LinearVelocity;
	Vec3Physics		m_Force;
	float			m_InvMass;

	

	//<----------COLLISION------------>
	CollisionShape* m_colShape;
	std::function<bool(PhysicsObject*)> m_OnCollisionCallback;  //Returns true to process the collision, false to ignore
};