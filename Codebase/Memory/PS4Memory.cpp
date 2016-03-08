#ifdef ORBIS
#include "PS4Memory.h"

sce::Gnmx::Toolkit::IAllocator		PS4Memory::onionAllocator;
sce::Gnmx::Toolkit::IAllocator		PS4Memory::garlicAllocator;
sce::Gnmx::Toolkit::IAllocator		PS4Memory::meshGarlicAllocator;

sce::Gnm::OwnerHandle				PS4Memory::ownerHandle;
#endif