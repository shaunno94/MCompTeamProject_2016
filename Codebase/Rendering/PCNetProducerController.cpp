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
	this->CheckInput();
	PCNetController::Sync(writer);
}