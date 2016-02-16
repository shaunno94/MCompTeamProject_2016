#include "PhysicsEngineInstance.h"

std::mutex PhysicsEngineInstance::m_mConstructed;
btSoftRigidDynamicsWorld* PhysicsEngineInstance::m_pInstance = nullptr;
btBroadphaseInterface* PhysicsEngineInstance::bf = nullptr;
btDefaultCollisionConfiguration* PhysicsEngineInstance::collisionConfiguration = nullptr;
btCollisionDispatcher* PhysicsEngineInstance::dispatcher = nullptr;
btSequentialImpulseConstraintSolver* PhysicsEngineInstance::solver = nullptr;
ParticleFilterCallback* PhysicsEngineInstance::filter = nullptr;
btSoftBodySolver* PhysicsEngineInstance::softSolver = nullptr;