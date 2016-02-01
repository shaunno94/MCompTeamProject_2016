#include "PhysicsEngine.h"
#include "GameObject.h"
#include "CollisionDetection.h"
#include <future>
#include "NCLDebug.h"

#include <nclgl\Window.h>

PhysicsEngine::PhysicsEngine() {
	m_IsPaused			= false;
	m_UpdateTimestep	= 1.0f / 60.f;
	m_UpdateAccum		= 0.0f;
	m_Gravity			=  Vec3Physics(0.0f, -9.81f, 0.0f);
	m_DampingFactor		= 0.999f;
	m_CollisionPairs = nullptr;

	PhysicsCycleCallback = []() {};
}

PhysicsEngine::~PhysicsEngine() {
	for (PhysicsObject* obj : m_PhysicsObjects)
		delete obj;
	m_PhysicsObjects.clear();

	for (Constraint* c : m_Constraints)
		delete c;
	m_Constraints.clear();

	for (Manifold* m : m_Manifolds)
		delete m;
	m_Manifolds.clear();
}

void PhysicsEngine::AddPhysicsObject(PhysicsObject* obj) {
	m_TempAddPhysicsObjects.push_back(obj);
	obj->PrepareRenderTransformUpdate();
	obj->UpdateRenderTransform();
}

void PhysicsEngine::RemovePhysicsObject(PhysicsObject* obj) {
	m_TempRemovePhysicsObjects.push_back(obj);
}

void PhysicsEngine::Update(float deltaTime) {
	m_UpdateAccum += deltaTime;
	int updateCounter = 0;
	while (m_UpdateAccum >= m_UpdateTimestep) {
		if (++updateCounter >= 3) break;
		m_UpdateAccum -= m_UpdateTimestep;
		if (!m_IsPaused) {
			UpdatePhysics();
			CheckDebugRender();
			//TODO: add check to in game object to update render transform if needed?
		}
	}
}

void PhysicsEngine::UpdatePhysics() {
	StartNewObjectProcessing();

	//TODO: cache instances for later use? could try using object pool
	for (Manifold* m : m_Manifolds)
		delete m;
	m_Manifolds.clear();

	NarrowPhaseCollisions();

	if (m_CollisionPairs)
		m_CollisionPairs->clear();
	if (m_SpacePartition)
		m_SpacePartition->StartBuildDebug();

	//end before solvers to clean any constraints from removed objects
	EndNewObjectProcessing();

	//Solve collision constraints
	SolveConstraints();

	if (m_SpacePartition)
		m_SpacePartition->EndBuildDebug();

	//Update movement
	UpdatePhysicsObjects();

	UpdateGameObjectStates();
}


std::mutex PhysicsEngine::g_RenderTransformMutex;
std::mutex PhysicsEngine::g_AddPhysicsObjectMutex;
std::future<void> PhysicsEngine::g_RenderTransformUpdateHandle;
std::future<void> PhysicsEngine::g_NewObjectProcessinbgHandle;

void PhysicsEngine::UpdateGameObjectStates() {
	//wait for previous render transform update to finish
	if (g_RenderTransformUpdateHandle.valid())
		g_RenderTransformUpdateHandle.get();

	//block renderer from reading new transform values
	for (PhysicsObject* obj : m_PhysicsObjects)
		obj->PrepareRenderTransformUpdate();

	g_RenderTransformUpdateHandle = std::move(std::async(std::launch::async, &PhysicsEngine::UpdateRenderTransforms, this));
}
void PhysicsEngine::UpdateRenderTransforms() {
	std::lock_guard<std::mutex> guard(g_RenderTransformMutex);
	for (PhysicsObject* obj : m_PhysicsObjects)
		obj->UpdateRenderTransform();
}

void PhysicsEngine::ProcessNewPhysicsObjects() {
	if (m_TempAddPhysicsObjects.size() > 0 || m_TempRemovePhysicsObjects.size() > 0) {
		std::lock_guard<std::mutex> guard(g_AddPhysicsObjectMutex);

		if (m_SpacePartition) {
			for (PhysicsObject* po : m_TempAddPhysicsObjects) {
				m_PhysicsObjects.push_back(po);
				if (po->AtRest())
					m_SpacePartition->Move(po->GetParent());
			}
		} else {
			for (PhysicsObject* po : m_TempAddPhysicsObjects)
				m_PhysicsObjects.push_back(po);
		}

		for (PhysicsObject* po : m_TempRemovePhysicsObjects) {
			FindSwapErase(m_PhysicsObjects, po);
			m_SpacePartition->Remove(po->GetParent());
			bool matched = false;
			for (Constraint*& constraint : m_Constraints) {
				if (constraint->objA == po || constraint->objB == po) {
					constraint = nullptr;
					matched = true;
				}
			}
			if (matched)
				m_Constraints.erase(std::remove(begin(m_Constraints), end(m_Constraints), nullptr), end(m_Constraints));
		}

		m_TempAddPhysicsObjects.clear();
		m_TempRemovePhysicsObjects.clear();
	}
}

void PhysicsEngine::UpdatePhysicsObjects() {
	{
		std::lock_guard<std::mutex> guard(PhysicsObject::g_ExternalChanges);
		for (PhysicsObject* po : m_PhysicsObjects)
			po->UpdateExternalChange();
	}

	if (m_SpacePartition) {
		//m_SpacePartition->ClearCollisions();
		for (PhysicsObject* po : m_PhysicsObjects) {
			if (!po->AtRest() && po->IsEnabled()) {
				m_SpacePartition->Remove(po->GetParent());
				UpdatePhysicsObject(po);
				if (!po->RestTest())
					m_SpacePartition->Push(po->GetParent());
				else
					m_SpacePartition->Move(po->GetParent());
			}
		}
		m_CollisionPairs = &(m_SpacePartition->GetCollisionPairs());
	} else {
		BruteForceBroadPhase();
		m_CollisionPairs = &m_BruteForceCollisionPairs;
	}
}


void PhysicsEngine::StartNewObjectProcessing() {
	g_NewObjectProcessinbgHandle = std::move(std::async(std::launch::async, &PhysicsEngine::ProcessNewPhysicsObjects, this));
}
void PhysicsEngine::EndNewObjectProcessing() {
	if (g_NewObjectProcessinbgHandle.valid())
		g_NewObjectProcessinbgHandle.get();
}

void PhysicsEngine::UpdatePhysicsObject(PhysicsObject* obj) {
	obj->m_LinearVelocity += obj->m_Force * obj->m_InvMass * m_UpdateTimestep;

	if (obj->m_InvMass > 0.00001f)
		obj->m_LinearVelocity += m_Gravity * m_UpdateTimestep;

	obj->m_LinearVelocity *= m_DampingFactor;
	obj->m_Position += obj->m_LinearVelocity * m_UpdateTimestep;

	Vec3Physics angularAccel = obj->m_InvInertia * obj->m_Torque;
	obj->m_AngularVelocity = obj->m_AngularVelocity + angularAccel * m_UpdateTimestep;
	obj->m_AngularVelocity *= m_DampingFactor;

	obj->m_Orientation = obj->m_Orientation + obj->m_Orientation * (obj->m_AngularVelocity * m_UpdateTimestep * 0.5f);

	obj->m_Orientation.Normalize();
	obj->m_wsTransformInvalidated = true;
}


bool PhysicsEngine::g_Running = true;
volatile float PhysicsEngine::g_LastStepTime = 0.0f;
bool PhysicsEngine::g_NewDebugRender = false;

void PhysicsEngine::Run() {
	PhysicsEngine* engine = PhysicsEngine::Instance();

	GameTimer engineTimer;
	GameTimer engineTimeMeasurer;
	while (g_Running) {
		float dt = engineTimer.GetTimedMS() * 0.001f;
		engineTimeMeasurer.GetTimedMS();

		engine->Update(dt);

		g_LastStepTime = engineTimeMeasurer.GetTimedMS();
		engine->PhysicsCycleCallback();
	}
}

/*
VERY Simplistic broadphase!
Compiles a list of all possible collisions that need to be accurately checked, in this case
we just make a list of every object against every other object. Perhaps you can make this better
with spatial broadphase checks such as Octrees.
*/
void PhysicsEngine::BruteForceBroadPhase() {
	PhysicsObject* objA, *objB;
	m_BruteForceCollisionPairs.clear();

	//This is a brute force broadphase, basically compiling a list to check every object against every other object
	for (size_t i = 0; i < m_PhysicsObjects.size() - 1; ++i) {
		objA = m_PhysicsObjects[i];
		if (objA->GetCollisionShape() != NULL)
			for (size_t j = i + 1; j < m_PhysicsObjects.size(); ++j) {
				objB = m_PhysicsObjects[j];

				//Check they both have collision shapes
				if (objB->GetCollisionShape() != NULL)
					m_BruteForceCollisionPairs.push_back(CollisionPair(objA, objB));

			}
	}
}

void PhysicsEngine::NarrowPhaseCollisions() {
	if (m_CollisionPairs) {
		std::vector<CollisionPair> tempCollisionRecords;
		do {
			m_AwokenObjects.clear();
			NarrowPhaseCollisionCycle();
			tempCollisionRecords.insert( tempCollisionRecords.end(), m_CollisionPairs->begin(), m_CollisionPairs->end() );
			m_CollisionPairs->clear();
			//rescan awoken objects for new collisions
			if (m_SpacePartition) {
				for (PhysicsObject* po : m_AwokenObjects) {
					m_SpacePartition->Remove(po->GetParent());
					m_SpacePartition->Push(po->GetParent());
				}
				if (m_CollisionPairs->size() > 0)
					auto test = 0;
				if (m_CollisionPairs->size() > 0) {
					m_CollisionPairs->erase(
					  std::remove_if(
					    m_CollisionPairs->begin(),
					    m_CollisionPairs->end(),
					[&](CollisionPair & pair1) {
						return std::find_if(
						         tempCollisionRecords.begin(),
						         tempCollisionRecords.end(),
						[&](CollisionPair & pair2) {
							return (pair1.first == pair2.first && pair1.second == pair2.second) || (pair1.second == pair2.first && pair1.first == pair2.second);
						}
						       ) != tempCollisionRecords.end();
					}
					  ),
					m_CollisionPairs->end()
					);
				}
			}
		} while (m_AwokenObjects.size() > 0);
	}
}

void PhysicsEngine::NarrowPhaseCollisionCycle() {
	std::vector<Manifold*> tempManiforldContainers[NARROWPHASE_THREADS];
	std::vector<PhysicsObject*> tempAwokenObjectContainers[NARROWPHASE_THREADS];
	size_t size = m_CollisionPairs->size();
	size_t objectsInBatch = size / NARROWPHASE_THREADS;

	std::future<void> asyncHandles[NARROWPHASE_THREADS];
	for (int i = 0; i < NARROWPHASE_THREADS; ++i) {
		size_t start = i * objectsInBatch;
		asyncHandles[i] = std::move(std::async(std::launch::async, &PhysicsEngine::NarrowPhaseCollisionsBatch, this, start, start + objectsInBatch, &tempManiforldContainers[i], &tempAwokenObjectContainers[i]));
	}

	NarrowPhaseCollisionsBatch(NARROWPHASE_THREADS * objectsInBatch, size, &m_Manifolds, &m_AwokenObjects);

	for (int i = 0; i < NARROWPHASE_THREADS; ++i) {
		asyncHandles[i].get();
		for (Manifold* manifold : tempManiforldContainers[i])
			m_Manifolds.push_back(manifold);
		for (PhysicsObject* awokenObject : tempAwokenObjectContainers[i])
			m_AwokenObjects.push_back(awokenObject);
	}
}


void PhysicsEngine::NarrowPhaseCollisionsBatch(size_t batch_start, size_t batch_end, std::vector<Manifold*>* out, std::vector<PhysicsObject*>* wakeupOut) {
	CollisionData coldata;
	CollisionShape* shapeA, *shapeB;

	for (size_t i = batch_start; i < batch_end; ++i) {
		CollisionPair& cp = (*m_CollisionPairs)[i];
		if (!cp.first->IsEnabled() || !cp.second->IsEnabled())
			continue;

		shapeA = cp.first->GetCollisionShape();
		shapeB = cp.second->GetCollisionShape();
		if (!shapeA || !shapeB)
			continue;

		if (CollisionDetection::Instance()->CheckCollision(cp.first, cp.second, shapeA, shapeB, &coldata)) {
			bool okA = cp.first->m_OnCollisionCallback(cp.second);
			bool okB = cp.second->m_OnCollisionCallback(cp.first);

			if (okA && okB) {
				WakeUp(cp.first, *wakeupOut);
				WakeUp(cp.second, *wakeupOut);
				//If both objects are colliding, and both callbacks allow for default collision resolution we will build a full collision manifold
				Manifold* manifold = new Manifold(cp.first, cp.second);
				if (CollisionDetection::Instance()->BuildCollisionManifold(cp.first, cp.second, shapeA, shapeB, coldata, manifold))
					out->push_back(manifold);
				else
					delete manifold;
			}
		}
	}
}

void PhysicsEngine::WakeUp(PhysicsObject* obj, std::vector<PhysicsObject*>& out) {
	if (obj->AtRest()) {
		obj->AtRest(false);
		if (!obj->AtRest())
			out.push_back(obj);
	}
}

void PhysicsEngine::ProcessAwokenObjects() {
	while (m_AwokenObjects.size() > 0) {
		if (m_SpacePartition) {
			for (PhysicsObject* po : m_AwokenObjects) {
				m_SpacePartition->Remove(po->GetParent());
				m_SpacePartition->Push(po->GetParent());
			}
		}
		m_AwokenObjects.clear();


	}
}

void PhysicsEngine::PreSolveManifoldBatch(size_t batch_start, size_t batch_end) {
	for (size_t i = batch_start; i < batch_end; ++i) {
		Manifold* manifold = m_Manifolds[i];
		manifold->PreSolverStep(m_UpdateTimestep);
	}
}
void PhysicsEngine::PreSolveConstraintBatch(size_t batch_start, size_t batch_end) {
	for (size_t i = batch_start; i < batch_end; ++i) {
		Constraint* constraint = m_Constraints[i];
		constraint->PreSolverStep(m_UpdateTimestep);
	}
}

void PhysicsEngine::SolveConstraints() {
	size_t manifoldSize = m_Manifolds.size();
	size_t manifoldsInBatch = manifoldSize / PRESOLVER_THREADS;

	std::future<void> asyncManifoldHandles[PRESOLVER_THREADS];
	for (int i = 0; i < PRESOLVER_THREADS; ++i) {
		size_t start = i * manifoldsInBatch;
		asyncManifoldHandles[i] = std::move(std::async(std::launch::async, &PhysicsEngine::PreSolveManifoldBatch, this, start, start + manifoldsInBatch));
	}

	size_t constraintSize = m_Constraints.size();
	size_t constraintInBatch = constraintSize / PRESOLVER_THREADS;

	std::future<void> asyncConstraintHandles[PRESOLVER_THREADS];
	for (int i = 0; i < PRESOLVER_THREADS; ++i) {
		size_t start = i * constraintInBatch;
		asyncConstraintHandles[i] = std::move(std::async(std::launch::async, &PhysicsEngine::PreSolveConstraintBatch, this, start, start + constraintInBatch));
	}

	PreSolveManifoldBatch(PRESOLVER_THREADS * manifoldsInBatch, manifoldSize);
	PreSolveConstraintBatch(PRESOLVER_THREADS * constraintInBatch, constraintSize);

	for (int i = 0; i < PRESOLVER_THREADS; ++i)
		asyncManifoldHandles[i].get();
	for (int i = 0; i < PRESOLVER_THREADS; ++i)
		asyncConstraintHandles[i].get();

	for (size_t i = 0; i < SOLVER_ITERATIONS; ++i) {
		for (Manifold* m : m_Manifolds)
			m->ApplyImpulse();
		for (Constraint* c : m_Constraints)
			c->ApplyImpulse();
	}
}

void PhysicsEngine::DebugRender() {
#if DEBUG_PHYSICS
	g_NewDebugRender = true;
#endif
}

void PhysicsEngine::CheckDebugRender() {
#if DEBUG_PHYSICS
	if (!NCLDebug::g_NewData && g_NewDebugRender) {

#if DEBUGDRAW_MANIFOLDS
		for (Manifold* m : m_Manifolds)
			m->DebugDraw();
#endif

#if DEBUGDRAW_CONSTRAINTS
		for (Constraint* c : m_Constraints)
			c->DebugDraw();
#endif

#if DEBUGDRAW_COLLISIONVOLUMES
		for (PhysicsObject* obj : m_PhysicsObjects) {
			if (obj->GetCollisionShape() != NULL)
				obj->GetCollisionShape()->DebugDraw(obj);
		}
#endif
		if (m_SpacePartition)
			m_SpacePartition->DebugDraw();
		g_NewDebugRender = false;
		NCLDebug::g_NewData = true;
	}
#endif
}