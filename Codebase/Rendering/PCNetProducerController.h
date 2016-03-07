#ifndef ORBIS

#pragma once
#include "PCNetController.h"

#ifndef ORBIS
#include "KeyboardController.h"
#else
#include "PS4Controller.h"
#endif

class PCNetProducerController : public PCNetController,
#ifndef ORBIS
	protected KeyboardController
#else
	protected PS4Controller
#endif
{
public:
	PCNetProducerController(ControllerComponent* object, unsigned int id);
	~PCNetProducerController();

	virtual void Sync(NetSessionWriter& writer) override;

};

#endif