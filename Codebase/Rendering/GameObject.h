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
#include <Math\nclglMath.h>
#include "PhysicsEngine\PhysicsEngineInstance.h"
#include "Material.h"
#include "Mesh.h"
#include <vector>

enum CollisionShape
{
	SPHERE, CUBOID, CYLINDER, CONE, PLANE
};

enum PhysicsType
{
	PARTICLE, SOFT, RIGID 
};

class GameObject
{
	//This is the only class that can manually set the world transform
	friend class Scene;

public:
	GameObject(const std::string& name = "");
	~GameObject();
	
	btRigidBody* Physics() { return m_RigidPhysicsObject; }

	const std::string&	GetName()			{ return m_Name; }
	std::vector<GameObject*>& GetChildren() { return m_Children; }

	GameObject*			FindGameObject(const std::string& name);
	void				AddChildObject(GameObject* child);


	void			SetLocalTransform(const Mat4Graphics& transform)			{ m_LocalTransform = transform; }
	const Mat4Graphics&  GetLocalTransform()									{ return m_LocalTransform; }


	void SetColour(const Vec4Graphics& colour)	{ m_Colour = colour; }
	const Vec4Graphics&	GetColour()							{ return m_Colour; }

	//Initialise physics body: mass (a mass of 0 will result in a static object), inertia, orientation, position
	void InitPhysics(double mass = 1.0, Vec3Physics inertia = Vec3Physics(0, 0, 0), QuatPhysics orientation = QuatPhysics(0, 0, 0, 1),
		Vec3Physics position = Vec3Physics(0, 0, 0), PhysicsType type = RIGID);

	void			SetBoundingRadius(float radius)		{ m_BoundingRadius = radius; }
	float			GetBoundingRadius()					{ return m_BoundingRadius; }

protected:
	virtual void OnRenderObject()				{};				//Handles OpenGL calls to Render the object
	virtual void OnUpdateObject(float dt)		{};				//Override to handle things like AI etc on update loop


protected:
	std::string					m_Name;
	Scene*						m_Scene;
	GameObject*					m_Parent;
	std::vector<GameObject*>	m_Children;

	btRigidBody*				m_RigidPhysicsObject;
	btDefaultMotionState*		m_MotionState;
	btCollisionShape*			m_ColShape;
	btRigidBody::btRigidBodyConstructionInfo* m_ConstructionInfo;

	Mesh* mesh;
	Material* mat;

	Vec4Graphics						m_Colour;
	float						m_BoundingRadius;	//For Frustum Culling
	Mat4Graphics				m_LocalTransform;
	Mat4Graphics				m_WorldTransform;
};