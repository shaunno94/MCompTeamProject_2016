#include "Shader.h"

#ifndef ORBIS 
Shader::Shader(string VERTEX, string FRAG, string GEO, string TCS_HULL, string TES_DOMAIN) : OGLShader(VERTEX, FRAG, GEO, TCS_HULL, TES_DOMAIN){}
#else 
Shader::Shader(string VERTEX, string FRAG, string GEO, string TCS_HULL, string TES_DOMAIN)  {}
#endif
