#pragma once
#include <string>
#include "constants.h"
class BaseShader
{
public:
	BaseShader(){}
	virtual ~BaseShader(){}

	virtual bool IsOperational() const = 0;
	virtual int GetResourceByName(const std::string& name) const = 0;
	virtual int GetReservedMeshTextureUniformLocation(size_t index) const = 0;
protected:
};