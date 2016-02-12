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
#include "RenderComponent.h"
#include <vector>

class Renderer;
class Scene;

/// @ingroup Rendering
/// <summary>
/// 
/// </summary>
enum CollisionShape
{
	SPHERE, CUBOID, CYLINDER, CONE, CAPSULE
};

/// @ingroup Rendering
/// <summary>
/// 
/// </summary>
enum BodyType
{
	PARTICLE, SOFT, RIGID
};

/// @ingroup Rendering
/// <summary>
/// 
/// </summary>
class GameObject
{
	//Allows these classes to call OnUpdateObject / OnRenderObject functions
	friend class Renderer;
	friend class Scene;

public:
	GameObject(const std::string& name = "");
	~GameObject();

	btRigidBody* Physics()
	{
		return m_RigidPhysicsObject;
	}

	const std::string& GetName()
	{
		return m_Name;
	}
	std::vector<GameObject*>& GetChildren()
	{
		return m_Children;
	}

	GameObject*	FindGameObject(const std::string& name);
	void AddChildObject(GameObject* child);

	void SetWorldTransform(const Mat4Graphics& transform)
	{
		m_WorldTransform = transform;
	}

	const Mat4Graphics&  GetWorldTransform() const
	{
		return m_WorldTransform;
	}

	//Set local transform for object.
	void SetLocalTransform(const Mat4Graphics& transform)
	{
		m_LocalTransform = transform;
	}

	const Mat4Graphics&  GetLocalTransform() const
	{
		return m_LocalTransform;
	}
	
	//Initialise physics body: mass (a mass of 0 will result in a static object), position, orientation, inertia, type (Rigid, Particle, Soft) 
	bool InitPhysics(double mass, const Vec3Physics& position, const QuatPhysics& orientation, const Vec3Physics& inertia = Vec3Physics(0, 0, 0), BodyType type = RIGID);
	
	//Sphere collision shape.
	bool CreateCollisionShape(double radius); 
	//Cuboid or Cylinder collsion shapes.
	bool CreateCollisionShape(const Vec3Physics& half_extents, CollisionShape shape); 
	//Cone or Capsule collision shapes.
	bool CreateCollisionShape(double radius, double height, CollisionShape shape); 
	//Static Plane collision shape.
	bool CreateCollisionShape(double distance, const Vec3Physics& normal, bool normalised); 

	void SetBoundingRadius(float radius)
	{
		m_BoundingRadius = radius;
	}
	float GetBoundingRadius()
	{
		return m_BoundingRadius;
	}

	void SetRenderComponent(RenderComponent* comp)
	{
		m_RenderComponent = comp;
		m_RenderComponent->SetParent(this);
	}
	RenderComponent* GetRenderComponent() const
	{
		return m_RenderComponent;
	}

protected:
	virtual void OnRenderObject();			//Handles OpenGL calls to Render the object
	virtual void OnUpdateObject(float dt);	//Override to handle things like AI etc on update loop
	void UpdateTransform();	//Updates local transform matrix with positional & rotational data from bullet physics

	std::string					m_Name;
	GameObject*					m_Parent;
	std::vector<GameObject*>	m_Children;

	btRigidBody*				m_RigidPhysicsObject;
	btDefaultMotionState*		m_MotionState;
	btCollisionShape*			m_ColShape;
	btRigidBody::btRigidBodyConstructionInfo* m_ConstructionInfo;

	RenderComponent*			m_RenderComponent;

	float						m_BoundingRadius;	//Unused
	Mat4Graphics				m_WorldTransform;
	Mat4Graphics				m_LocalTransform;

	float m_CamDist; //For ordering of rendering lists.
};