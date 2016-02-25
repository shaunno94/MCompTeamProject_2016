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
	virtual int GetReservedMeshTextureUniformLocation(unsigned int index) const = 0;
	virtual int GetReservedMeshColourUniformLocation(unsigned int index) const = 0;
	virtual int GetModelMatrixLocation() const = 0;
protected:
};