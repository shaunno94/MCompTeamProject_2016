/******************************************************************************
Class:GameObject
Implements:
Author: Rich Davison	<richard.davison4@newcastle.ac.uk> and YOU!
Description: This is the base class for all of the objects in your game - the
player character, enemies, pickups etc would all be derived classes of the
GameObject.

This class 'wraps up' all of the communication between the subsystems of your
game - it has a renderer component (The SceneNode you are used to using), a
'physics' component (The PhysicsNode you've been introduced to in this code
download), and eventually you'll add a 'sound' component, in the short sound
workshop in semester 2.

The sub systems handle the updating of the 'sub components' of a GameEntity,
so in the Update function of this class will just be the 'gameplay' specific
type functionality (has the player picked up some health / has he shot an enemy
and so forth).

-_-_-_-_-_-_-_,------,
_-_-_-_-_-_-_-|   /\_/\   NYANYANYAN
-_-_-_-_-_-_-~|__( ^ .^) /
_-_-_-_-_-_-_-""  ""

*//////////////////////////////////////////////////////////////////////////////
#pragma once
#include <vector>
#include "Math/nclglMath.h"


#include <btBulletDynamicsCommon.h>

class PhysicsEngine;



class GameObject
{
protected:
	Mat4Physics					m_LocalTransform;
	Mat4Physics					m_WorldTransform;
	Vec4Graphics				m_Colour;
	Vec3Physics					m_Scale;
	Vec3Physics					m_BoundingHalfVolume;

public:
	GameObject(const std::string& name = "", PhysicsObject* physicsObj = new PhysicsObject());
	~GameObject();

	PhysicsObject*		Physics() { return m_PhysicsObject; }

	const std::string&	GetName()			{ return m_Name; }
	std::vector<GameObject*>& GetChildren() { return m_Children; }

	inline const Vec3Physics& GetScale() { return m_Scale; }
	inline void	SetScale(const Vec3Physics& scale) { m_Scale = scale; }

	GameObject*			FindGameObject(const std::string& name);
	void				AddChildObject(GameObject* child);

	void AddPhysics(PhysicsObject* physicsObj = new PhysicsObject());
	void RemovePhysics();


	void			SetLocalTransform(const Mat4Physics& transform)			{ m_LocalTransform = transform; }
	const Mat4Physics&  GetLocalTransform()									{ return m_LocalTransform; }


	void			SetColour(const Vec4Graphics& colour)	{ m_Colour = colour; }
	const Vec4Graphics&	GetColour()							{ return m_Colour; }

	void				SetBoundingRadius(float radius)					{ m_BoundingRadius = radius; }
	float				GetBoundingRadius()	const						{ return m_BoundingRadius; }
	void				SetBoundingHalfVolume(const Vec3Physics& vec)	{ m_BoundingHalfVolume = vec; }
	const Vec3Physics&	GetBoundingHalfVolume()	const					{ return m_BoundingHalfVolume; }

	bool IsBoundingSphere() const { return m_IsBoundingSphere; }
	void IsBoundingSphere(bool sphereOrAABB) { m_IsBoundingSphere = sphereOrAABB; }

	Scene* GetScene() { return m_Scene; }
	void SetScene(Scene* scene) { m_Scene = scene; }

	StateMachine* GetStateMachine() { return m_StateMachine; }
	void SetStateMachine(StateMachine* sm) { m_StateMachine = sm; }

	void Ditach();

	MEM_ALIGN_NEW_DELETE

protected:
	virtual void OnRenderObject()				{};				//Handles OpenGL calls to Render the object
	virtual void OnUpdateObject(float dt)		{
		if(m_StateMachine)
			m_StateMachine->Update(dt);
	};				//Override to handle things like AI etc on update loop

	bool m_IsBoundingSphere;

	std::string					m_Name;
	Scene*						m_Scene;
	GameObject*					m_Parent;
	std::vector<GameObject*>	m_Children;

	PhysicsObject*				m_PhysicsObject;
	StateMachine*				m_StateMachine;

	float						m_BoundingRadius;	//For Frustum Culling
};