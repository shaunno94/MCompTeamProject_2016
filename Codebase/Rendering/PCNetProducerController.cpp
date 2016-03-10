#ifndef ORBIS
#include "PCNetProducerController.h"

PCNetProducerController::PCNetProducerController(ControllerComponent* object, unsigned int id) : PCNetController(object, id),
#ifndef ORBIS
	KeyboardController(object)
#else
	PS4Controller(object)
#endif
{
}


PCNetProducerController::~PCNetProducerController()
{
}


void PCNetProducerController::Sync(NetSessionWriter& writer)
{
	//if not the ball
	if (m_id != 1)
	{
		this->CheckInput();
	}
	PCNetController::Sync(writer);
}
#endif
