#include <iostream>
#include <btBulletDynamicsCommon.h>
#include "Rendering\Window.h"
#include "Rendering\Renderer.h"

int main()
{
	//-------------------
	//--- MAIN ENGINE ---
	//-------------------

	//Initialise the Window
	if (!Window::Initialise("Game Technologies - Framework Example", 1280, 800, false))
	{
		Window::Destroy();
		return -1;
	}

	Window::GetWindow().LockMouseToWindow(true);
	Window::GetWindow().ShowOSPointer(false);

	/*CommonMeshes::InitializeMeshes();
	NCLDebug::LoadShaders();*/

	Renderer renderer(Window::GetWindow());
	if (!renderer.HasInitialised())
	{
		return -1;
	}
	//Broadphase collision
	btBroadphaseInterface* bf = new btDbvtBroadphase();

	//Narrowphase collision configuration
	btDefaultCollisionConfiguration* collisionConfiguration = new btDefaultCollisionConfiguration();
	btCollisionDispatcher* dispatcher = new btCollisionDispatcher(collisionConfiguration);

	//Constraint solver - impulse
	btSequentialImpulseConstraintSolver* solver = new btSequentialImpulseConstraintSolver;

	//Physics world object
	btDiscreteDynamicsWorld* dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, bf, solver, collisionConfiguration);
	dynamicsWorld->setGravity(btVector3(0, -10, 0));

	//Static plane collision object
	btCollisionShape* groundShape = new btStaticPlaneShape(btVector3(0, 1, 0), 1);

	//Sphere collision object
	btCollisionShape* fallShape = new btSphereShape(1);

	//Motion state (http://www.bulletphysics.org/mediawiki-1.5.8/index.php/MotionStates) - for plane object
	btDefaultMotionState* groundMotionState =
	  new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(0, -1, 0)));

	//Construct rigid body object
	btRigidBody::btRigidBodyConstructionInfo groundRigidBodyCI(0, groundMotionState, groundShape, btVector3(0, 0, 0));
	btRigidBody* groundRigidBody = new btRigidBody(groundRigidBodyCI);

	//Add plane to physics world
	dynamicsWorld->addRigidBody(groundRigidBody);

	//Motion state - for sphere object
	btDefaultMotionState* fallMotionState =
	  new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(0, 50, 0)));

	//Calculate inertia tensor
	fallShape->calculateLocalInertia(1.0, btVector3(0, 0, 0));
	//Construct rigid body object
	btRigidBody::btRigidBodyConstructionInfo fallRigidBodyCI(1.0, fallMotionState, fallShape, btVector3(0, 0, 0));
	btRigidBody* fallRigidBody = new btRigidBody(fallRigidBodyCI);

	//Add sphere to physics world
	dynamicsWorld->addRigidBody(fallRigidBody);

	while (Window::GetWindow().UpdateWindow() && !Window::GetKeyboard()->KeyDown(KEYBOARD_ESCAPE))
	{
		//Step simulation
		dynamicsWorld->stepSimulation(1.0 / 60.0, 10);
		renderer.UpdateScene(Window::GetWindow().GetTimer()->Get(1000.0f));
		renderer.RenderScene();
	}

	/*for (unsigned int i = 0; i < 300; ++i)
	{

		//Model matrix
		btTransform trans;
		fallRigidBody->getMotionState()->getWorldTransform(trans);
		std::cout << "Sphere height" << trans.getOrigin().getY() << std::endl;
	}*/

	//Clean-up rigid body objects
	dynamicsWorld->removeRigidBody(fallRigidBody);
	delete fallRigidBody->getMotionState();
	delete fallRigidBody;

	dynamicsWorld->removeRigidBody(groundRigidBody);
	delete groundRigidBody->getMotionState();
	delete groundRigidBody;

	//Clean-up collision shapes
	delete groundShape;
	delete fallShape;

	//Clean-up physics environment
	delete dynamicsWorld;
	delete solver;
	delete dispatcher;
	delete collisionConfiguration;
	delete bf;

	Window::Destroy();
	return 0;
}