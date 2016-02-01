/******************************************************************************
Class: PhysicsEngine
Implements:
Author: Rich Davison <richard.davison4@newcastle.ac.uk> and YOU!
Description:
A very simple physics engine class, within which to implement the
material introduced in the Game Technologies module. This is just a rough
skeleton of how the material could be integrated into the existing codebase -
it is still entirely up to you how the specifics should work. Now C++ and
graphics are out of the way, you should be starting to get a feel for class
structures, and how to communicate data between systems.

It is worth poinitng out that the PhysicsEngine is constructed and destructed
manually using static functions. Why? Well, we probably only want a single
physics system to control the entire state of our game world, so why allow
multiple systems to be made? So instead, the constructor / destructor are
hidden, and we 'get' a single instance of a physics system with a getter.
This is known as a 'singleton' design pattern, and some developers don't like
it - but for systems that it doesn't really make sense to have multiples of,
it is fine!

		(\_/)
		( '_')
	 /""""""""""""\=========     -----D
	/"""""""""""""""""""""""\
....\_@____@____@____@____@_/

*//////////////////////////////////////////////////////////////////////////////

#pragma once
#include "TSingleton.h"
#include "PhysicsObject.h"
#include "Constraint.h"
#include "Manifold.h"
#include <vector>
#include <mutex>
#include "OctreeSpacePartition.h"
#include <future>
#include <atomic>
#include "constants.h"
#include <functional>

class PhysicsEngine : public TSingleton<PhysicsEngine> {
	friend class TSingleton < PhysicsEngine > ;

	static std::future<void> g_RenderTransformUpdateHandle;
	static std::future<void> g_NewObjectProcessinbgHandle;
#ifdef DEBUG_PHYSICS
	static bool g_NewDebugRender;
#endif

 public:
	static std::mutex g_RenderTransformMutex;
	static std::mutex g_AddPhysicsObjectMutex;

	std::function<void()> PhysicsCycleCallback;

	std::vector<PhysicsObject*> m_AwokenObjects;

	//Add/Remove Physics Objects
	void AddPhysicsObject(PhysicsObject* obj);
	void RemovePhysicsObject(PhysicsObject* obj);

	//Add Constraints
	void AddConstraint(Constraint* c) { m_Constraints.push_back(c); }
	void RemoveConstraint(Constraint* c) { FindSwapErase(m_Constraints, c); }


	//Update Physics Engine
	void Update(float deltaTime);			//Remember DeltaTime is 'seconds' since last update not milliseconds

	//Debug draw all physics objects, manifolds and constraints
	void DebugRender();

	void StartNewObjectProcessing();
	void EndNewObjectProcessing();

	//Getters / Setters
	bool IsPaused()						{ return m_IsPaused; }
	void SetPaused(bool paused)			{ m_IsPaused = paused; }


	void SetUpdateTimestep(float updateTimestep) { m_UpdateTimestep = updateTimestep; }
	float GetUpdateTimestep()			{ return m_UpdateTimestep; }

	const Vec3Physics& GetGravity()			{ return m_Gravity; }
	void SetGravity(const Vec3Physics& g)	{ m_Gravity = g; }

	float GetDampingFactor()			{ return m_DampingFactor; }
	void  SetDampingFactor(float d)		{ m_DampingFactor = d; }

	float GetDeltaTime()				{ return m_UpdateTimestep; }

	inline OctreeSpacePartition* GetSpacePartition() const { return m_SpacePartition; }
	inline void SetSpacePartition(OctreeSpacePartition* partition) { m_SpacePartition = partition; }

	static void Run();
	static bool g_Running;
	static volatile float g_LastStepTime;

 protected:
	PhysicsEngine();
	~PhysicsEngine();

	//The actual time-independant update function
	void UpdatePhysics();

	void BruteForceBroadPhase();

	//Handles narrowphase collision detection
	void NarrowPhaseCollisions();

	/// <summary>
	/// Resolving the obtained collision pairs
	/// </summary>
	void NarrowPhaseCollisionCycle();
	/// <summary>
	/// A batch of collision pairs to resolve
	/// </summary>
	void NarrowPhaseCollisionsBatch(size_t batch_start, size_t batch_end, std::vector<Manifold*>* out, std::vector<PhysicsObject*>* wakeupOut); //<--- The worker function for multithreading

	void ProcessNewPhysicsObjects();

	//Updates all physics objects position, orientation, velocity etc (default method uses symplectic euler integration)
	void UpdatePhysicsObjects();
	void UpdatePhysicsObjectsBatch(size_t batch_start, size_t batch_end, std::vector<PhysicsObject*>* activeObjs);  //<--- The worker function for multithreading
	void UpdatePhysicsObject(PhysicsObject* obj);						   //<--- The actual code to update the given physics object

	/// <summary>
	/// Moving Current state to temp holders for transfer onto the render thread
	/// </summary>
	void UpdateGameObjectStates();
	/// <summary>
	/// Update transforms for the render thread
	/// </summary>
	void UpdateRenderTransforms();

	void PreSolveManifoldBatch(size_t batch_start, size_t batch_end);
	void PreSolveConstraintBatch(size_t batch_start, size_t batch_end);

	void WakeUp(PhysicsObject* obj, std::vector<PhysicsObject*>& out);
	void ProcessAwokenObjects();

	//Solves all engine constraints (constraints and manifolds)
	void SolveConstraints();

	void CheckDebugRender();

	bool		m_IsPaused;
	float		m_UpdateTimestep, m_UpdateAccum;

	Vec3Physics		m_Gravity;
	float		m_DampingFactor;

	OctreeSpacePartition* m_SpacePartition;



	std::vector<CollisionPair>* m_CollisionPairs;

	std::vector<CollisionPair> m_BruteForceCollisionPairs;
	std::vector<PhysicsObject*> m_PhysicsObjects;
	std::vector<PhysicsObject*> m_TempAddPhysicsObjects;
	std::vector<PhysicsObject*> m_TempRemovePhysicsObjects;

	std::vector<Constraint*>	m_Constraints;			// Misc constraints between pairs of objects
	std::vector<Manifold*>		m_Manifolds;			// Contact constraints between pairs of objects
};