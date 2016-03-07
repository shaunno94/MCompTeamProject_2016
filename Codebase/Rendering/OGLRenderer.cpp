/*
Class:OGLRenderer
Author:Rich Davison	<richard.davison4@newcastle.ac.uk>
Description:Abstract base class for the graphics tutorials. Creates an OpenGL
3.2 CORE PROFILE rendering context. Each lesson will create a renderer that
inherits from this class - so all context creation is handled automatically,
but students still get to see HOW such a context is created.

-_-_-_-_-_-_-_,------,
_-_-_-_-_-_-_-|   /\_/\   NYANYANYAN
-_-_-_-_-_-_-~|__( ^ .^) /
_-_-_-_-_-_-_-""  ""

*/
#ifndef ORBIS
#include "OGLRenderer.h"
#include "Helpers/degrees.h"
#include "constants.h"
#include "Renderer.h"
#include "OGLShader.h"

/*
Creates an OpenGL 3.2 CORE PROFILE rendering context. Sets itself
as the current renderer of the passed 'parent' Window. Not the best
way to do it - but it kept the Tutorial code down to a minimum!
*/

Renderer* OGLRenderer::child = nullptr;

OGLRenderer::OGLRenderer(std::string title, int sizeX, int sizeY, bool fullScreen)
{
	init = false;
	if (!(&Window::GetWindow()))
	{
		if (!Window::Initialise(title, sizeX, sizeY, fullScreen))
		{
			Window::Destroy();
			return;
		}
	}

	HWND windowHandle = Window::GetWindow().GetHandle();

	// Did We Get A Device Context?
	if (!(deviceContext = GetDC(windowHandle)))
	{
		std::cout << "OGLRenderer::OGLRenderer(): Failed to create window!" << std::endl;
		return;
	}

	//A pixel format descriptor is a struct that tells the Windows OS what type of front / back buffers we want to create etc
	PIXELFORMATDESCRIPTOR pfd;
	memset(&pfd, 0, sizeof(PIXELFORMATDESCRIPTOR));

	pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_DOUBLEBUFFER | PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW;   //It must be double buffered, it must support OGL(!), and it must allow us to draw to it...
	pfd.iPixelType = PFD_TYPE_RGBA;	//We want our front / back buffer to have 4 channels!
	pfd.cColorBits = 32;				//4 channels of 8 bits each!
	pfd.cDepthBits = 24;				//24 bit depth buffer
	pfd.cStencilBits = 8;				//plus an 8 bit stencil buffer
	pfd.iLayerType = PFD_MAIN_PLANE;

	GLuint		PixelFormat;
	if (!(PixelFormat = ChoosePixelFormat(deviceContext, &pfd)))		 	// Did Windows Find A Matching Pixel Format for our PFD?
	{
		std::cout << "OGLRenderer::OGLRenderer(): Failed to choose a pixel format!" << std::endl;
		return;
	}

	if (!SetPixelFormat(deviceContext, PixelFormat, &pfd))			 		// Are We Able To Set The Pixel Format?
	{
		std::cout << "OGLRenderer::OGLRenderer(): Failed to set a pixel format!" << std::endl;
		return;
	}

	HGLRC		tempContext;		//We need a temporary OpenGL context to check for OpenGL 3.2 compatibility...stupid!!!
	if (!(tempContext = wglCreateContext(deviceContext)))				 	// Are We Able To get the temporary context?
	{
		std::cout << "OGLRenderer::OGLRenderer(): Cannot create a temporary context!" << std::endl;
		wglDeleteContext(tempContext);
		return;
	}

	if (!wglMakeCurrent(deviceContext, tempContext))					 	// Try To Activate The Rendering Context
	{
		std::cout << "OGLRenderer::OGLRenderer(): Cannot set temporary context!" << std::endl;
		wglDeleteContext(tempContext);
		return;
	}

	//Now we have a temporary context, we can find out if we support OGL 3.x
	char* ver = (char*)glGetString(GL_VERSION); // ver must equal "3.2.0" (or greater!)
	int major = ver[0] - '0';		//casts the 'correct' major version integer from our version string
	int minor = ver[2] - '0';		//casts the 'correct' minor version integer from our version string

	if (major < 3)  					//Graphics hardware does not support OGL 3! Erk...
	{
		std::cout << "OGLRenderer::OGLRenderer(): Device does not support OpenGL 3.x!" << std::endl;
		wglDeleteContext(tempContext);
		return;
	}

	if (major == 3 && minor < 2)  	//Graphics hardware does not support ENOUGH of OGL 3! Erk...
	{
		std::cout << "OGLRenderer::OGLRenderer(): Device does not support OpenGL 3.2!" << std::endl;
		wglDeleteContext(tempContext);
		return;
	}
	//We do support OGL 3! Let's set it up...
	int attribs[] =
	{
		WGL_CONTEXT_MAJOR_VERSION_ARB, major,	//TODO: Maybe lock this to 3? We might actually get an OpenGL 4.x context...
		WGL_CONTEXT_MINOR_VERSION_ARB, minor,
		WGL_CONTEXT_FLAGS_ARB, WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB,
		WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,		//We want everything OpenGL 3.2 provides...
		0
	};

	//Everywhere else in the Renderers, we use function pointers provided by GLEW...but we can't initialise GLEW yet! So we have to use the 'Wiggle' API
	//to get a pointer to the function that will create our OpenGL 3.2 context...
	PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB = (PFNWGLCREATECONTEXTATTRIBSARBPROC)wglGetProcAddress("wglCreateContextAttribsARB");
	renderContext = wglCreateContextAttribsARB(deviceContext, 0, attribs);

	// Check for the context, and try to make it the current rendering context
	if (!renderContext || !wglMakeCurrent(deviceContext, renderContext))
	{
		std::cout << "OGLRenderer::OGLRenderer(): Cannot set OpenGL 3 context!" << std::endl;	//It's all gone wrong!
		wglDeleteContext(renderContext);
		wglDeleteContext(tempContext);
		return;
	}

	wglDeleteContext(tempContext);	//We don't need the temporary context any more!

	glewExperimental = GL_TRUE;	//This forces GLEW to give us function pointers for everything (gets around GLEW using 'old fashioned' methods
	//for determining whether a OGL context supports a particular function or not

	if (glewInit() != GLEW_OK)  	//Try to initialise GLEW
	{
		std::cout << "OGLRenderer::OGLRenderer(): Cannot initialise GLEW!" << std::endl;	//It's all gone wrong!
		return;
	}

	//If we get this far, everything's going well!
	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);			//When we clear the screen, we want it to be dark grey

	currentShader = nullptr;							//0 is the 'null' object name for shader programs...

	Window::GetWindow().SetRenderer(this);					//Tell our window about the new renderer! (Which will in turn resize the renderer window to fit...)

	RECT clientRect, windowRect;
	if (GetClientRect(windowHandle, &clientRect) && GetWindowRect(windowHandle, &windowRect))
	{
		int widthDiff = (windowRect.right - windowRect.left) - (clientRect.right - clientRect.left);
		int heightDiff = (windowRect.bottom - windowRect.top) - (clientRect.bottom - clientRect.top);
		Resize(int(width + widthDiff), int(height + heightDiff));
	}

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_DEPTH_CLAMP);
	glEnable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glDepthFunc(GL_LEQUAL);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	if (initFBO())
		init = true;
}

/*
Destructor. Deletes the default shader, and the OpenGL rendering context.
*/
OGLRenderer::~OGLRenderer(void)
{
	delete quad;
	glDeleteTextures(1, &bufferColourTex);
	glDeleteTextures(1, &bufferNormalTex);
	glDeleteTextures(1, &bufferDepthTex);
	glDeleteTextures(1, &lightEmissiveTex);
	glDeleteTextures(1, &lightSpecularTex);

	glDeleteFramebuffers(1, &bufferFBO);
	glDeleteFramebuffers(1, &pointLightFBO);
	//delete currentShader; - Should be handled by Renderer/Scene class
	wglDeleteContext(renderContext);
	Window::Destroy();
}

/*
Returns TRUE if everything in the constructor has gone to plan.
Check this to end the application if necessary...
*/
bool OGLRenderer::HasInitialised() const
{
	return init;
}

/*
Resizes the rendering area. Should only be called by the Window class!
Does lower bounds checking on input values, so should be reasonably safe
to call.
*/
void OGLRenderer::Resize(int x, int y)
{
	width = max(x, 1);
	height = max(y, 1);
	glViewport(0, 0, width, height);
}

/*
Swaps the buffers, ready for the next frame's rendering. Should be called
every frame, at the end of RenderScene(), or whereever appropriate for
your application.
*/
void OGLRenderer::SwapBuffers()
{
	glUseProgram(0);
	//We call the windows OS SwapBuffers on win32. Wrapping it in this
	//function keeps all the tutorial code 100% cross-platform (kinda).
	::SwapBuffers(deviceContext);

	//TODO: check if this is even needed
	Sleep(0);
}

/*
Updates the uniform matrices of the current shader. Assumes that
the shader has uniform matrices called modelMatrix, viewMatrix,
projMatrix, and textureMatrix. Updates them with the relevant
matrix data. Sanity checks currentShader, so is always safe to
call.
*/
void OGLRenderer::UpdateShaderMatrices()
{
	if (currentShader)
	{
		//Model Matrix in RenderComponent class.
		//Texture matrix in material class.
		glUniformMatrix4fv(glGetUniformLocation(currentShader->GetProgram(), "viewMatrix"), 1, false, (float*)&viewMatrix);
		glUniformMatrix4fv(glGetUniformLocation(currentShader->GetProgram(), "projMatrix"), 1, false, (float*)&projMatrix);
	}
}


void OGLRenderer::SetCurrentShader(BaseShader* s)
{
	currentShader = static_cast<OGLShader*>(s);

	glUseProgram(currentShader->GetProgram());
}

void OGLRenderer::SetTextureRepeating(GLuint target, bool repeating)
{
	glBindTexture(GL_TEXTURE_2D, target);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, repeating ? GL_REPEAT : GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, repeating ? GL_REPEAT : GL_CLAMP);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void OGLRenderer::UpdateUniform(GLint location, const Mat4Graphics& mat4)
{
	if (location >= 0) glUniformMatrix4fv(location, 1, false, (float*)&mat4);
}
void OGLRenderer::UpdateUniform(GLint location, const Mat3Graphics& mat3)
{
	if (location >= 0) glUniformMatrix3fv(location, 1, false, (float*)&mat3);
}
void OGLRenderer::UpdateUniform(GLint location, const Vec4Graphics& vec4)
{
	if (location >= 0) glUniform4fv(location, 1, (float*)&vec4);
}
void OGLRenderer::UpdateUniform(GLint location, const Vec3Graphics& vec3)
{
	if (location >= 0) glUniform3fv(location, 1, (float*)&vec3);
}
void OGLRenderer::UpdateUniform(GLint location, const Vec2Graphics& vec2)
{
	if (location >= 0) glUniform2fv(location, 1, (float*)&vec2);
}
void OGLRenderer::UpdateUniform(GLint location, float f)
{
	if (location >= 0) glUniform1f(location, f);
}
void OGLRenderer::UpdateUniform(GLint location, double d)
{
	if (location >= 0) glUniform1d(location, d);
}
void OGLRenderer::UpdateUniform(GLint location, int i)
{
	if (location >= 0) glUniform1i(location, i);
}
void OGLRenderer::UpdateUniform(GLint location, unsigned int u)
{
	if (location >= 0) glUniform1ui(location, u);
}

unsigned int OGLRenderer::TextureMemoryUsage(unsigned int id)
{
	int width;
	int height;
	int r, g, b, a;

	glBindTexture(GL_TEXTURE_2D, id);

	glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_RED_SIZE, &r);
	glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_GREEN_SIZE, &g);
	glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_BLUE_SIZE, &b);
	glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_ALPHA_SIZE, &a);
	glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &width);
	glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &height);

	glBindTexture(GL_TEXTURE_2D, 0);
	return (width * height * ((r + g + b + a) / 8.0f));
}

void OGLRenderer::SetTextureFlags(unsigned int id, unsigned int flags)
{
	glBindTexture(GL_TEXTURE_2D, GLuint(id));

	// Repeat/Clamp options
	if ((flags & REPEATING) == REPEATING)
	{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	}
	else if ((flags & CLAMPING) == CLAMPING)
	{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	}

	// Filtering options
	if ((flags & NEAREST_NEIGHBOUR_MIN_FILTERING) == NEAREST_NEIGHBOUR_MIN_FILTERING)
	{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	}
	else if ((flags & BILINEAR_MIN_FILTERING) == BILINEAR_MIN_FILTERING)
	{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	}
	else if ((flags & TRILINEAR_MIN_FILTERING) == TRILINEAR_MIN_FILTERING)
	{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	}

	if ((flags & NEAREST_NEIGHBOUR_MAX_FILTERING) == NEAREST_NEIGHBOUR_MAX_FILTERING)
	{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	}
	else if ((flags & BILINEAR_MAX_FILTERING) == BILINEAR_MAX_FILTERING)
	{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}
	else if ((flags & TRILINEAR_MAX_FILTERING) == TRILINEAR_MAX_FILTERING)
	{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	}

	if ((flags & ANISOTROPIC_FILTERING) == ANISOTROPIC_FILTERING)
	{
		GLfloat max_aniso;
		glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &max_aniso);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, max_aniso);
	}

	glBindTexture(GL_TEXTURE_2D, 0);
}

bool OGLRenderer::initFBO()
{
	glGenFramebuffers(1, &bufferFBO);
	glGenFramebuffers(1, &pointLightFBO);
	glGenFramebuffers(1, &shadowFBO);
	glGenFramebuffers(1, &cubeShadowFBO);

	GLenum buffers[2];
	buffers[0] = GL_COLOR_ATTACHMENT0;
	buffers[1] = GL_COLOR_ATTACHMENT1;

	// Generate Screen sized textures ...
	GenerateScreenTexture(bufferDepthTex, true);
	GenerateScreenTexture(bufferColourTex);
	GenerateScreenTexture(bufferNormalTex);
	GenerateScreenTexture(lightEmissiveTex);
	GenerateScreenTexture(lightSpecularTex);

	// And now attach them to our FBOs
	glBindFramebuffer(GL_FRAMEBUFFER, bufferFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, bufferColourTex, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, bufferNormalTex, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, bufferDepthTex, 0);
	glDrawBuffers(2, buffers);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		throw "!GL_FRAMEBUFFER_COMPLETE";
	}

	//Second Pass
	glBindFramebuffer(GL_FRAMEBUFFER, pointLightFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, lightEmissiveTex, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, lightSpecularTex, 0);
	glDrawBuffers(2, buffers);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		throw "!GL_FRAMEBUFFER_COMPLETE";
	}

	//Generate Shadow Texture
	glGenTextures(1, &shadowTex2D);
	glBindTexture(GL_TEXTURE_2D, shadowTex2D);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
	             SHADOWSIZE, SHADOWSIZE, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);

	glBindTexture(GL_TEXTURE_2D, 0);

	//Cube shadow Texture
	glGenTextures(1, &shadowTexCube);
	glBindTexture(GL_TEXTURE_CUBE_MAP, shadowTexCube);
	glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_DEPTH_TEXTURE_MODE, GL_LUMINANCE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	for (size_t i = 0; i < 6; ++i)
	{
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT,
		             SHADOWSIZE, SHADOWSIZE, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	}
	glEnable(GL_TEXTURE_CUBE_MAP);
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

	//attach to shadow FBO
	glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadowTex2D, 0);
	glDrawBuffer(GL_NONE);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glEnable(GL_BLEND);

	//attach to cube shadow FBO
	glBindFramebuffer(GL_FRAMEBUFFER, cubeShadowFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_CUBE_MAP_POSITIVE_X, shadowTexCube, 0);
	glDrawBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	//cube helpers
	directions[0] = Vec3Graphics(1, 0, 0);
	directions[1] = Vec3Graphics(-1, 0, 0);
	directions[2] = Vec3Graphics(0, -1, 0);
	directions[3] = Vec3Graphics(0, 1, 0);
	directions[4] = Vec3Graphics(0, 0, -1);
	directions[5] = Vec3Graphics(0, 0, 1);
	up[0] = Vec3Graphics(0, 1, 0);
	up[1] = Vec3Graphics(0, 1, 0);
	up[2] = Vec3Graphics(0, 0, -1);
	up[3] = Vec3Graphics(0, 0, -1);
	up[4] = Vec3Graphics(0, 1, 0);
	up[5] = Vec3Graphics(0, 1, 0);

	//skybox setup
	skyBoxTex = SOIL_load_OGL_cubemap(
		TEXTURE_DIR"grouse_posx.jpg",
		TEXTURE_DIR"grouse_negx.jpg",
		TEXTURE_DIR"grouse_posy.jpg",
		TEXTURE_DIR"grouse_negy.jpg",
		TEXTURE_DIR"grouse_posz.jpg",
		TEXTURE_DIR"grouse_negz.jpg",
		SOIL_LOAD_RGB, SOIL_CREATE_NEW_ID, 0);

	skyQuad = new GameObject();
	skyQuad->SetRenderComponent(new RenderComponent(new CubeMaterial(new OGLShader(SHADER_DIR"skyboxVertex.glsl", SHADER_DIR"skyboxfragment.glsl")), OGLMesh::GenerateQuad()));
	if (!skyQuad->GetRenderComponent()->m_Material->GetShader()->IsOperational())
		return false;
	((CubeMaterial*)skyQuad->GetRenderComponent()->m_Material)->Set(ReservedOtherTextures.CUBE.name, (int)ReservedOtherTextures.CUBE.index);


	//quad for final render
	quad = new GameObject();
	quad->SetRenderComponent(new RenderComponent(new LightMaterial(new OGLShader(SHADER_DIR"combinevert.glsl", SHADER_DIR"combinefrag.glsl")), OGLMesh::GenerateQuad()));
	((LightMaterial*)quad->GetRenderComponent()->m_Material)->Set(ReservedOtherTextures.EMISSIVE.name, (int)ReservedOtherTextures.EMISSIVE.index);
	((LightMaterial*)quad->GetRenderComponent()->m_Material)->Set(ReservedOtherTextures.SPECULAR.name, (int)ReservedOtherTextures.SPECULAR.index);

	if (!quad->GetRenderComponent()->m_Material->GetShader()->IsOperational())
		return false;

	return true;
}

void OGLRenderer::GenerateScreenTexture(GLuint& into, bool depth)
{
	glGenTextures(1, &into);
	glBindTexture(GL_TEXTURE_2D, into);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	glTexImage2D(GL_TEXTURE_2D, 0,
		depth ? GL_DEPTH_COMPONENT24 : GL_RGBA8,
		width, height, 0,
		depth ? GL_DEPTH_COMPONENT : GL_RGBA,
		GL_UNSIGNED_BYTE, NULL);

	glBindTexture(GL_TEXTURE_2D, 0);
}

void OGLRenderer::FillBuffers()
{
	glDisable(GL_CULL_FACE);
	glBindFramebuffer(GL_FRAMEBUFFER, bufferFBO);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	//SetCurrentShader(sceneShader);
	
	UpdateShaderMatrices();
	//DrawSky();

#if DEBUG_DRAW
	PhysicsEngineInstance::Instance()->debugDrawWorld();
#endif
	child->OnRenderScene();

	glUseProgram(0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glEnable(GL_CULL_FACE);
}

void OGLRenderer::DrawPointLights()
{
	glBindFramebuffer(GL_FRAMEBUFFER, pointLightFBO);

	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT);

	glBlendFunc(GL_ONE, GL_ONE);

	glActiveTexture(GL_TEXTURE0 + ReservedOtherTextures.DEPTH.index);
	glBindTexture(GL_TEXTURE_2D, bufferDepthTex);

	glActiveTexture(GL_TEXTURE0 + ReservedOtherTextures.NORMALS.index);
	glBindTexture(GL_TEXTURE_2D, bufferNormalTex);
	
	child->OnRenderLights();

	glCullFace(GL_BACK);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glClearColor(0.2f, 0.2f, 0.2f, 1);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glUseProgram(0);
}

void OGLRenderer::CombineBuffers() 
{
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	projMatrix = Mat4Graphics::Orthographic(-1, 1, 1, -1, -1, 1);
	UpdateShaderMatrices();

	glActiveTexture(GL_TEXTURE0 + ReservedMeshTextures.DIFFUSE.index);
	glBindTexture(GL_TEXTURE_2D, bufferColourTex);

	glActiveTexture(GL_TEXTURE0 + ReservedOtherTextures.EMISSIVE.index);
	glBindTexture(GL_TEXTURE_2D, lightEmissiveTex);

	glActiveTexture(GL_TEXTURE0 + ReservedOtherTextures.SPECULAR.index);
	glBindTexture(GL_TEXTURE_2D, lightSpecularTex);
	quad->GetRenderComponent()->Draw();

	glUseProgram(0);
}

void OGLRenderer::DrawShadow(GameObject* light){
	LightMaterial* lm = (LightMaterial*)(light->GetRenderComponent()->m_Material);
	switch (lm->shadowType)
	{
	case _NONE:
	default:
		break;
	case _2D:
		DrawShadow2D(light);
		break;
	case _CUBE:
		DrawShadowCube(light);
		break;
	}
}

void OGLRenderer::DrawShadowCube(GameObject* light){
	glBindFramebuffer(GL_FRAMEBUFFER, cubeShadowFBO);

	glViewport(0, 0, SHADOWSIZE, SHADOWSIZE);

	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

	projMatrix = Mat4Graphics::Perspective(1, 15000, 1, 90);
	glClearColor(1.0f, 0.0f, 1.0f, 1.0f);

	Vec3Graphics pos = light->GetWorldTransform().GetTranslation();
	for (size_t i = 0; i < 6; ++i) {
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
			GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, shadowTexCube, 0);
		glClear(GL_DEPTH_BUFFER_BIT);

		viewMatrix = Mat4Graphics::View(pos, pos + directions[i], up[i]); //modify;

		//textureMatrix = biasMatrix *(projMatrix * viewMatrix);

		UpdateShaderMatrices();

		glDisable(GL_CULL_FACE);
		child->OnRenderScene();
		glEnable(GL_CULL_FACE);
	}
	glUseProgram(0);
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	glViewport(0, 0, width, height);
	
	//((LightMaterial*)light->GetRenderComponent()->m_Material)->Set("lightProj", Mat4Graphics::Perspective(1, 15000, 1, 90));

	glBindFramebuffer(GL_FRAMEBUFFER, pointLightFBO);
	
	glActiveTexture(GL_TEXTURE0 + ReservedOtherTextures.SHADOW_CUBE.index);
	glBindTexture(GL_TEXTURE_CUBE_MAP, shadowTexCube);
	glActiveTexture(GL_TEXTURE0);

	projMatrix = child->localProjMat;
	viewMatrix = child->currentScene->getCamera()->BuildViewMatrix();
	child->OnUpdateScene(child->frameFrustrum, child->currentScene->getCamera()->GetPosition());
}


void OGLRenderer::DrawShadow2D(GameObject* light){
	glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);
	projMatrix = Mat4Graphics::Perspective(50.0f, 15000.0f, 1.0f, 45.0f);
	glClear(GL_DEPTH_BUFFER_BIT);

	glViewport(0, 0, SHADOWSIZE, SHADOWSIZE);

	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

	//TODO override shader
	//SetCurrentShader ( shadowShader );
	viewMatrix = Mat4Graphics::View(light->GetWorldTransform().GetTranslation(), Vec3Graphics(0, 0, 0));
	((LightMaterial*)light->GetRenderComponent()->m_Material)->shadowBias = biasMatrix *(projMatrix*viewMatrix);

	//draw game objects
	child->lightFrustrum.FromMatrix(projMatrix * viewMatrix);
	child->OnUpdateScene(child->lightFrustrum, light->GetWorldTransform().GetTranslation());

	glDisable(GL_CULL_FACE);
	child->OnRenderScene();
	glEnable(GL_CULL_FACE);

	glUseProgram(0);
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	glViewport(0, 0, width, height);

	glBindFramebuffer(GL_FRAMEBUFFER, pointLightFBO);
	viewMatrix = child->currentScene->getCamera()->BuildViewMatrix();

	glActiveTexture(GL_TEXTURE0 + ReservedOtherTextures.SHADOW_2D.index);
	glBindTexture(GL_TEXTURE_2D, shadowTex2D);
	glActiveTexture(GL_TEXTURE0);

	projMatrix = child->localProjMat;
	//viewMatrix = currentScene->getCamera()->BuildViewMatrix();
	
	child->OnUpdateScene(child->frameFrustrum, child->currentScene->getCamera()->GetPosition());
}

void OGLRenderer::DrawSky(){
	glDepthMask(GL_FALSE);

	glActiveTexture(GL_TEXTURE0 + ReservedOtherTextures.CUBE.index);
	glBindTexture(GL_TEXTURE_CUBE_MAP, skyBoxTex);
	glActiveTexture(GL_TEXTURE0);

	
	skyQuad->GetRenderComponent()->Draw();

	glUseProgram(0);
	glDepthMask(GL_TRUE);
	projMatrix = child->localProjMat;
}

#endif