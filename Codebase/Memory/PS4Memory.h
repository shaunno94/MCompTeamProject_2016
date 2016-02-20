#ifdef ORBIS
#pragma once

#include <gnm.h>
#include <gnmx\context.h>
#include "stack_allocator.h"
#include "allocators.h"

class PS4Memory	
{
protected:
	PS4Memory() {}
	~PS4Memory() {}

	//Slow bus - cache coherent
	static sce::Gnmx::Toolkit::IAllocator	onionAllocator;
	//Fast bus
	static sce::Gnmx::Toolkit::IAllocator	garlicAllocator;
	static sce::Gnm::OwnerHandle			ownerHandle;
};
#endif