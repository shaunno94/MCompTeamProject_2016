#pragma once
#include <stddef.h>
#include <mutex>
#include <btBulletDynamicsCommon.h>

struct ParticleFilterCallback : public btOverlapFilterCallback
{
	virtual bool needBroadphaseCollision(btBroadphaseProxy* proxy0, btBroadphaseProxy* proxy1) const override
	{
		bool collides = (proxy0->m_collisionFilterGroup & proxy1->m_collisionFilterMask) != 0;
		collides = collides && (proxy1->m_collisionFilterGroup & proxy0->m_collisionFilterMask);

		return collides;
	}
};

class PhysicsEngineInstance
{

public:
	//Provide global access to the only instance of this class
	static btDiscreteDynamicsWorld* Instance()
	{
		if (!m_pInstance)	//This if statement prevents the costly Lock-step being required each time the instance is requested
		{
			std::lock_guard<std::mutex> lock(m_mConstructed);		//Lock is required here though, to prevent multiple threads initialising multiple instances of the class when it turns out it has not been initialised yet
			if (!m_pInstance) //Check to see if a previous thread has already initialised an instance in the time it took to acquire a lock.
			{
				//Broadphase collision
				bf = new btDbvtBroadphase();

				//Narrowphase collision configuration
				collisionConfiguration = new btDefaultCollisionConfiguration();
				dispatcher = new btCollisionDispatcher(collisionConfiguration);

				//Constraint solver - impulse
				solver = new btSequentialImpulseConstraintSolver;

				//Physics world object
				m_pInstance = new btDiscreteDynamicsWorld(dispatcher, bf, solver, collisionConfiguration);

				filter = new ParticleFilterCallback();
				m_pInstance->getPairCache()->setOverlapFilterCallback(filter);
			}
		}
		return m_pInstance;
	}

	//Provide global access to release/delete this class
	static void Release()
	{
		//Technically this could have another enclosing if statement, but speed is much less of a problem as this should only be called once in the entire program.
		std::lock_guard<std::mutex> lock(m_mConstructed);
		if (m_pInstance)
		{
			delete m_pInstance;
			m_pInstance = NULL;
		}
	}

protected:
	//Only allow the class to be created and destroyed by itself
	PhysicsEngineInstance() {}
	~PhysicsEngineInstance() {}


private:

	//Prevent the class from being copied either by '=' operator or by copy constructor
	PhysicsEngineInstance(PhysicsEngineInstance const&)				{}
	PhysicsEngineInstance& operator=(PhysicsEngineInstance const&)	{}

	//Keep a static instance pointer to refer to as required by the rest of the program
	static std::mutex m_mConstructed;
	static btDiscreteDynamicsWorld* m_pInstance;
	static btBroadphaseInterface* bf;
	static btDefaultCollisionConfiguration* collisionConfiguration;
	static btCollisionDispatcher* dispatcher;
	static btSequentialImpulseConstraintSolver* solver;
	static ParticleFilterCallback* filter;
};

//Finally make sure that the instance is initialised to NULL at the start of the program
std::mutex PhysicsEngineInstance::m_mConstructed;
btDiscreteDynamicsWorld* PhysicsEngineInstance::m_pInstance = nullptr;
btBroadphaseInterface* PhysicsEngineInstance::bf = nullptr;
btDefaultCollisionConfiguration* PhysicsEngineInstance::collisionConfiguration = nullptr;
btCollisionDispatcher* PhysicsEngineInstance::dispatcher = nullptr;
btSequentialImpulseConstraintSolver* PhysicsEngineInstance::solver = nullptr;
ParticleFilterCallback* PhysicsEngineInstance::filter = nullptr;