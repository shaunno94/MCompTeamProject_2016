#pragma once
#include <stddef.h>
#include <mutex>
#include <btBulletDynamicsCommon.h>
#include <vector>


# define BIT(x) 1 << x
enum CollisionMasks
{
	COL_NOTHING = 0,
	COL_WALL = BIT(0),
	COL_CAR = BIT(1),
	COL_AI_CAR = BIT(2),
	COL_BALL = BIT(3),
	COL_GOAL1 = BIT(4),
	COL_GOAL2 = BIT(5),
	COL_PICKUP = BIT(6)
};
enum CollisionGroups
{
	COL_GROUP_ALL = ~0,
	COL_GROUP_DEFAULT = 1,
	/*GROUP_WALL_BALL = COL_WALL | COL_BALL,
	GROUP_CAR_WALL = COL_CAR | COL_WALL,
	GROUP_CAR_BALL = COL_CAR | COL_BALL*/
};

class ParticleFilterStep
{
public:
	ParticleFilterStep(unsigned int groupId = 0, unsigned int maskTest = ~0)
	{
		group = groupId;
		mask = maskTest;
	}

	virtual bool callback(btBroadphaseProxy* proxy0, btBroadphaseProxy* proxy1) const
	{
		return true;
	}

	unsigned int group;
	unsigned int mask;
};

//Collision callback which should return false if the entity is a particle i.e. group ID == 0 and mask == 0.
//This will tell bullet to not consider particles in collision detection / response (hopefully - untested)
class ParticleFilterCallback : public btOverlapFilterCallback
{
public:
	~ParticleFilterCallback()
	{
		for (auto step : steps)
			delete step;
		steps.clear();
	}

	virtual bool needBroadphaseCollision(btBroadphaseProxy* proxy0, btBroadphaseProxy* proxy1) const override
	{/*
		if ((proxy0->m_collisionFilterMask & (COL_PICKUP | COL_CAR)) && (proxy1->m_collisionFilterMask & (COL_PICKUP | COL_CAR)))
		{
		int test = 0;
		}*/

		bool result = true;
		if (proxy0->m_collisionFilterGroup & proxy1->m_collisionFilterGroup)
		{
			for (auto step : steps)
			{
				if ((proxy0->m_collisionFilterGroup & step->group) && (proxy1->m_collisionFilterGroup & step->group))
				{
					if ((proxy0->m_collisionFilterMask & step->mask) && (proxy1->m_collisionFilterMask & step->mask))
					{
						result = result && step->callback(proxy0, proxy1);
					}
				}
			}
		}
		return result;
	}

	std::vector<ParticleFilterStep*> steps;
};

class PhysicsEngineInstance
{
public:
	//Provide global access to the only instance of this class
	static btSoftRigidDynamicsWorld* Instance()
	{
		if (!m_pInstance)	//This if statement prevents the costly Lock-step being required each time the instance is requested
		{
			std::lock_guard<std::mutex> lock(m_mConstructed);		//Lock is required here though, to prevent multiple threads initialising multiple instances of the class when it turns out it has not been initialised yet
			if (!m_pInstance) //Check to see if a previous thread has already initialised an instance in the time it took to acquire a lock.
			{
				//Broadphase collision object - Dynamic tree AABB
				bf = new btDbvtBroadphase();

				//Narrowphase collision configuration
				collisionConfiguration = new btDefaultCollisionConfiguration();
				dispatcher = new btCollisionDispatcher(collisionConfiguration);

				//Constraint solver - impulse
				solver = new btSequentialImpulseConstraintSolver;
				//Soft body solver
				softSolver = new btDefaultSoftBodySolver();

				//Physics world object
				m_pInstance = new btSoftRigidDynamicsWorld(dispatcher, bf, solver, collisionConfiguration, softSolver);

				filter = new ParticleFilterCallback();
				m_pInstance->getPairCache()->setOverlapFilterCallback(filter);
				//m_pInstance->getPairCache()->setOverlapFilterCallback(filter);
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
			delete bf;
			delete collisionConfiguration;
			delete dispatcher;
			delete solver;
			delete softSolver;
			delete filter;
			bf = nullptr;
			collisionConfiguration = nullptr;
			dispatcher = nullptr;
			solver = nullptr;
			filter = nullptr;
			m_pInstance = nullptr;
		}
	}

	static ParticleFilterCallback& GetFilter()
	{
		return *filter;
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
	static btSoftRigidDynamicsWorld* m_pInstance;
	static btBroadphaseInterface* bf;
	static btDefaultCollisionConfiguration* collisionConfiguration;
	static btCollisionDispatcher* dispatcher;
	static btSequentialImpulseConstraintSolver* solver;
	static btSoftBodySolver* softSolver;
	static ParticleFilterCallback* filter;
};