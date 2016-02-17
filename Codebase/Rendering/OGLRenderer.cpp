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

#include "OGLRenderer.h"
#include "Helpers/degrees.h"
#include "constants.h"
#include "LightMaterial.h"

DebugDrawData* OGLRenderer::orthoDebugData = NULL;
DebugDrawData* OGLRenderer::perspectiveDebugData = NULL;
OGLRenderer*   OGLRenderer::debugDrawingRenderer = NULL;
Shader*		   OGLRenderer::debugDrawShader = NULL;

bool		   OGLRenderer::drawnDebugOrtho = false;
bool		   OGLRenderer::drawnDebugPerspective = false;


/*
Creates an OpenGL 3.2 CORE PROFILE rendering context. Sets itself
as the current renderer of the passed 'parent' Window. Not the best
way to do it - but it kept the Tutorial code down to a minimum!
*/
OGLRenderer::OGLRenderer(Window& window)
{
	init = false;
	drawnDebugOrtho = false;
	drawnDebugPerspective = false;

	HWND windowHandle = window.GetHandle();

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
		WGL_CONTEXT_FLAGS_ARB, WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB
#ifdef OPENGL_DEBUGGING
		| WGL_CONTEXT_DEBUG_BIT_ARB
#endif		//No deprecated stuff!! DIE DIE DIE glBegin!!!!
		, WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,		//We want everything OpenGL 3.2 provides...
		0					//That's enough attributes...
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



#ifdef OPENGL_DEBUGGING
#if _DEBUG
	//PFNWGLCREATECONTEXTATTRIBSARBPROC glDebugMessageCallbackTEMP = (PFNWGLCREATECONTEXTATTRIBSARBPROC) wglGetProcAddress("glDebugMessageCallbackARB");
	glDebugMessageCallbackARB(&OGLRenderer::DebugCallback, NULL);
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS_ARB);
#endif
#endif

	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);			//When we clear the screen, we want it to be dark grey

	currentShader = 0;							//0 is the 'null' object name for shader programs...

	window.SetRenderer(this);					//Tell our window about the new renderer! (Which will in turn resize the renderer window to fit...)

	if (!debugDrawingRenderer)
	{
		debugDrawShader = new Shader(SHADER_DIR"/DebugVertex.glsl", SHADER_DIR"DebugFragment.glsl");
		orthoDebugData = new DebugDrawData();
		perspectiveDebugData = new DebugDrawData();
		debugDrawingRenderer = this;

		if (!debugDrawShader->IsOperational())
			return;
	}

	currentScene = nullptr;

	//GL flags
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_DEPTH_CLAMP);
	glEnable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glDepthFunc(GL_LEQUAL);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	initFBO();
}

/*
Destructor. Deletes the default shader, and the OpenGL rendering context.
*/
OGLRenderer::~OGLRenderer(void)
{
	delete orthoDebugData;
	delete perspectiveDebugData;
	//delete currentShader; - Should be handled by Renderer/Scene class
	delete debugDrawShader;
	wglDeleteContext(renderContext);

	delete quad;
	glDeleteTextures(1, &bufferColourTex);
	glDeleteTextures(1, &bufferNormalTex);
	glDeleteTextures(1, &bufferDepthTex);
	glDeleteTextures(1, &lightEmissiveTex);
	glDeleteTextures(1, &lightSpecularTex);
	glDeleteTextures(1, &ShadowTex2D);

	glDeleteFramebuffers(1, &bufferFBO);
	glDeleteFramebuffers(1, &pointLightFBO);
	glDeleteFramebuffers(1, &shadowFBO);
}

void OGLRenderer::initFBO()
{
	glGenFramebuffers(1, &bufferFBO);
	glGenFramebuffers(1, &pointLightFBO);
	glGenFramebuffers(1, &shadowFBO);

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
	glGenTextures(1, &ShadowTex2D);
	glBindTexture(GL_TEXTURE_2D, ShadowTex2D);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
		SHADOWSIZE, SHADOWSIZE, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);

	glBindTexture(GL_TEXTURE_2D, 0);

	//attach to shadow FBO
	glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, ShadowTex2D, 0);
	glDrawBuffer(GL_NONE);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glEnable(GL_BLEND);

	quad = new GameObject();
	quad->SetRenderComponent(new RenderComponent(new LightMaterial(new Shader(SHADER_DIR"combinevert.glsl", SHADER_DIR"combinefrag.glsl")), Mesh::GenerateQuad()));
	((LightMaterial*)quad->GetRenderComponent()->m_Material)->Set(ReservedOtherTextures.EMISSIVE.name, (int)ReservedOtherTextures.EMISSIVE.index);
	((LightMaterial*)quad->GetRenderComponent()->m_Material)->Set(ReservedOtherTextures.SPECULAR.name, (int)ReservedOtherTextures.SPECULAR.index);
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

void OGLRenderer::updateGlobalUniforms(Material* material)
{
	auto lightMat = dynamic_cast<LightMaterial*>(material);
	if (lightMat)
	{
		Vec3Graphics camPos = currentScene->getCamera()->GetPosition();
		auto test = lightMat->Set("cameraPos", camPos);
		lightMat->Set("pixelSize", Vec2Graphics(1.0f / width, 1.0f / height));
		int i = 0;
	}
}

void OGLRenderer::FillBuffers()
{
	glDisable(GL_CULL_FACE);
	glBindFramebuffer(GL_FRAMEBUFFER, bufferFBO);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	//SetCurrentShader(sceneShader);

	UpdateShaderMatrices();

#if DEBUG_DRAW
	PhysicsEngineInstance::Instance()->debugDrawWorld();
#endif

	for (unsigned int i = 0; i < currentScene->getNumOpaqueObjects(); ++i)
		currentScene->getOpaqueObject(i)->OnRenderObject();
	for (unsigned int i = 0; i < currentScene->getNumTransparentObjects(); ++i)
		currentScene->getTransparentObject(i)->OnRenderObject();

	glUseProgram(0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glEnable(GL_CULL_FACE);
}

void OGLRenderer::DrawPointLights()
{
	//SetCurrentShader(pointlightShader);

	glBindFramebuffer(GL_FRAMEBUFFER, pointLightFBO);

	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT);

	glBlendFunc(GL_ONE, GL_ONE);

	glActiveTexture(GL_TEXTURE0 + ReservedOtherTextures.DEPTH.index);
	glBindTexture(GL_TEXTURE_2D, bufferDepthTex);

	glActiveTexture(GL_TEXTURE0 + ReservedOtherTextures.NORMALS.index);
	glBindTexture(GL_TEXTURE_2D, bufferNormalTex);

	for (unsigned int i = 0; i < currentScene->getNumLightObjects(); ++i)
	{
		GameObject* light = currentScene->getLightObject(i);
		DrawShadow(light);
		LightMaterial* lm = (LightMaterial*)light->GetRenderComponent()->m_Material;
		lm->Set("lightPos", light->GetWorldTransform().GetTranslation());
		lm->Set("lightRadius", light->GetBoundingRadius());
		lm->Set("lightColour", Vec4Graphics(1, 0.7, 0.5, 1));
		lm->Set("cameraPos", currentScene->getCamera()->GetPosition());
		lm->Set("shadowBias", lm->shadowBias);

		UpdateShaderMatrices();

		float dist = (light->GetWorldTransform().GetTranslation() - currentScene->getCamera()->GetPosition()).Length();

		if (dist < light->GetBoundingRadius())  // camera is inside the light volume !
			glCullFace(GL_FRONT);
		else
			glCullFace(GL_BACK);

		light->OnRenderObject();
	}

	glCullFace(GL_BACK);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glClearColor(0.2f, 0.2f, 0.2f, 1);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glUseProgram(0);
}

void OGLRenderer::CombineBuffers() {
	//SetCurrentShader(combineShader);

	projMatrix = Mat4Graphics::Orthographic(-1, 1, 1, -1, -1, 1);
	UpdateShaderMatrices();

	glActiveTexture(GL_TEXTURE0 + ReservedMeshTextures.DIFFUSE.index);
	glBindTexture(GL_TEXTURE_2D, bufferColourTex);

	glActiveTexture(GL_TEXTURE0 + ReservedOtherTextures.EMISSIVE.index);
	glBindTexture(GL_TEXTURE_2D, lightEmissiveTex);

	glActiveTexture(GL_TEXTURE0 + ReservedOtherTextures.SPECULAR.index);
	glBindTexture(GL_TEXTURE_2D, lightSpecularTex);
	quad->m_RenderComponent->Draw();

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
		//case _CUBE:
	}
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
	Mat4Graphics model = Mat4Graphics::Scale(Vec3Graphics(80, 40, 40)) * Mat4Graphics::Rotation(90.0f, Vec3Graphics(1, 0, 0));
	((LightMaterial*)light->GetRenderComponent()->m_Material)->shadowBias = biasMatrix *(projMatrix*viewMatrix);

	//draw game objects
	lightFrustrum.FromMatrix(projMatrix * viewMatrix);
	currentScene->UpdateNodeLists(0, lightFrustrum, light->GetWorldTransform().GetTranslation());

	glDisable(GL_CULL_FACE);
	for (unsigned int i = 0; i < currentScene->getNumOpaqueObjects(); ++i)
		currentScene->getOpaqueObject(i)->OnRenderObject();
	for (unsigned int i = 0; i < currentScene->getNumTransparentObjects(); ++i)
		currentScene->getTransparentObject(i)->OnRenderObject();
	glEnable(GL_CULL_FACE);

	glUseProgram(0);
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	glViewport(0, 0, width, height);

	glBindFramebuffer(GL_FRAMEBUFFER, pointLightFBO);
	viewMatrix = currentScene->getCamera()->BuildViewMatrix();

	glActiveTexture(GL_TEXTURE0 + ReservedOtherTextures.SHADOW_2D.index);
	glBindTexture(GL_TEXTURE_2D, ShadowTex2D);
	glActiveTexture(GL_TEXTURE0);

	projMatrix = Mat4Graphics::Perspective(1.0f, 15000.0f, aspectRatio, 45.0f);
	viewMatrix = currentScene->getCamera()->BuildViewMatrix();
	frameFrustrum.FromMatrix(projMatrix * viewMatrix);
	currentScene->UpdateNodeLists(0, frameFrustrum, currentScene->getCamera()->GetPosition());
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
	if (debugDrawingRenderer == this)
	{
		if (!drawnDebugOrtho)
			DrawDebugOrtho();
		if (!drawnDebugPerspective)
			DrawDebugPerspective();
		drawnDebugOrtho = false;
		drawnDebugPerspective = false;
	}

	//We call the windows OS SwapBuffers on win32. Wrapping it in this
	//function keeps all the tutorial code 100% cross-platform (kinda).
	::SwapBuffers(deviceContext);

	//TODO: check if this is even needed
	Sleep(0);
}
/*
Used by some later tutorials when we want to have framerate-independent
updates on certain datatypes. Really, OGLRenderer should have its own
timer, so it can just sit and look after itself (potentially even in
another thread...), but it's fine for the tutorials.

STUDENTS: Don't put your entity update routine in this, or anything like
that - it's just asking for trouble! Strictly speaking, even the camera
shouldn't be in here...(I'm lazy)
*/
void OGLRenderer::UpdateScene(float msec)
{

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
		//part of the RenderComponent now
		//glUniformMatrix4fv(glGetUniformLocation(currentShader->GetProgram(), "modelMatrix"), 1, false, (float*)&modelMatrix);
		glUniformMatrix4fv(glGetUniformLocation(currentShader->GetProgram(), "viewMatrix"), 1, false, (float*)&viewMatrix);
		glUniformMatrix4fv(glGetUniformLocation(currentShader->GetProgram(), "projMatrix"), 1, false, (float*)&projMatrix);
		//part of the material now
		//glUniformMatrix4fv(glGetUniformLocation(currentShader->GetProgram(), "textureMatrix"), 1, false, (float*)&textureMatrix);
	}
}


void OGLRenderer::SetCurrentShader(Shader* s)
{
	currentShader = s;

	glUseProgram(s->GetProgram());
}

void OGLRenderer::SetTextureRepeating(GLuint target, bool repeating)
{
	glBindTexture(GL_TEXTURE_2D, target);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, repeating ? GL_REPEAT : GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, repeating ? GL_REPEAT : GL_CLAMP);
	glBindTexture(GL_TEXTURE_2D, 0);
}

//void OGLRenderer::SetShaderLight(const Light &l) {
//	glUniform3fv(glGetUniformLocation(currentShader->GetProgram(), "lightPos")   ,1,(float*)&l.GetPosition());
//	glUniform4fv(glGetUniformLocation(currentShader->GetProgram(), "lightColour"),1,(float*)&l.GetColour());
//	glUniform1f(glGetUniformLocation(currentShader->GetProgram() , "lightRadius"),l.GetRadius());
//}

#if _DEBUG
#ifdef OPENGL_DEBUGGING
void OGLRenderer::DebugCallback(GLuint source, GLuint type, GLuint id, GLuint severity,
                                int length, const char* message, void* userParam)
{

	string sourceName;
	string typeName;
	string severityName;

	switch (source)
	{
	case GL_DEBUG_SOURCE_API_ARB:
		sourceName = "Source(OpenGL)";
		break;
	case GL_DEBUG_SOURCE_WINDOW_SYSTEM_ARB:
		sourceName = "Source(Window System)";
		break;
	case GL_DEBUG_SOURCE_SHADER_COMPILER_ARB:
		sourceName = "Source(Shader Compiler)";
		break;
	case GL_DEBUG_SOURCE_THIRD_PARTY_ARB:
		sourceName = "Source(Third Party)";
		break;
	case GL_DEBUG_SOURCE_APPLICATION_ARB:
		sourceName = "Source(Application)";
		break;
	case GL_DEBUG_SOURCE_OTHER_ARB:
		sourceName = "Source(Other)";
		break;
	}

	switch (type)
	{
	case GL_DEBUG_TYPE_ERROR_ARB:
		typeName = "Type(Error)";
		break;
	case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR_ARB:
		typeName = "Type(Deprecated Behaviour)";
		break;
	case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR_ARB:
		typeName = "Type(Undefined Behaviour)";
		break;
	case GL_DEBUG_TYPE_PORTABILITY_ARB:
		typeName = "Type(Portability)";
		break;
	case GL_DEBUG_TYPE_PERFORMANCE_ARB:
		typeName = "Type(Performance)";
		break;
	case GL_DEBUG_TYPE_OTHER_ARB:
		typeName = "Type(Other)";
		break;
	}

	switch (severity)
	{
	case GL_DEBUG_SEVERITY_HIGH_ARB:
		severityName = "Priority(High)";
		break;
	case GL_DEBUG_SEVERITY_MEDIUM_ARB:
		severityName = "Priority(Medium)";
		break;
	case GL_DEBUG_SEVERITY_LOW_ARB:
		severityName = "Priority(Low)";
		break;
	}

	cout << "OpenGL Debug Output: " + sourceName + ", " + typeName + ", " + severityName + ", " + string(message) << endl;
}
#endif
#endif


void	OGLRenderer::DrawDebugPerspective(Mat4Graphics* matrix)
{
	glUseProgram(debugDrawShader->GetProgram());

	if (matrix)
		glUniformMatrix4fv(glGetUniformLocation(debugDrawShader->GetProgram(), "viewProjMatrix"), 1, false, (float*)matrix);
	else
	{
		Mat4Graphics temp = projMatrix*viewMatrix;
		glUniformMatrix4fv(glGetUniformLocation(debugDrawShader->GetProgram(), "viewProjMatrix"), 1, false, (float*)&temp);
	}

	perspectiveDebugData->Draw();

	perspectiveDebugData->Clear();
	drawnDebugPerspective = true;
	SetCurrentShader(currentShader);
}


void	OGLRenderer::DrawDebugOrtho(Mat4Graphics* matrix)
{
	glUseProgram(debugDrawShader->GetProgram());

	if (matrix)
		glUniformMatrix4fv(glGetUniformLocation(debugDrawShader->GetProgram(), "viewProjMatrix"), 1, false, (float*)matrix);
	else
	{
		static Mat4Graphics ortho = Mat4Graphics::Orthographic(-1, 1, 720, 0, 0, 480);
		glUniformMatrix4fv(glGetUniformLocation(debugDrawShader->GetProgram(), "viewProjMatrix"), 1, false, (float*)&ortho);
	}

	orthoDebugData->Draw();

	orthoDebugData->Clear();
	drawnDebugOrtho = true;
	SetCurrentShader(currentShader);
}

void	OGLRenderer::DrawDebugLine(DebugDrawMode mode, const Vec3Graphics& from, const Vec3Graphics& to, const Vec3Graphics& fromColour, const Vec3Graphics& toColour)
{
	DebugDrawData* target = (mode == DEBUGDRAW_ORTHO ? target = orthoDebugData : target = perspectiveDebugData);

	target->AddLine(from, to, fromColour, toColour);
}

void	OGLRenderer::DrawDebugBox(DebugDrawMode mode, const Vec3Graphics& at, const Vec3Graphics& scale, const Vec3Graphics& colour)
{
	DebugDrawData* target = (mode == DEBUGDRAW_ORTHO ? target = orthoDebugData : target = perspectiveDebugData);

	target->AddLine(at + Vec3Graphics(-scale.x * 0.5f, scale.y * 0.5f, 0),
		at + Vec3Graphics(-scale.x * 0.5f, -scale.y * 0.5f, 0), colour, colour);

	target->AddLine(at + Vec3Graphics(-scale.x * 0.5f, -scale.y * 0.5f, 0),
		at + Vec3Graphics(scale.x * 0.5f, -scale.y * 0.5f, 0), colour, colour);

	target->AddLine(at + Vec3Graphics(scale.x * 0.5f, -scale.y * 0.5f, 0),
		at + Vec3Graphics(scale.x * 0.5f, scale.y * 0.5f, 0), colour, colour);

	target->AddLine(at + Vec3Graphics(scale.x * 0.5f, scale.y * 0.5f, 0),
		at + Vec3Graphics(-scale.x * 0.5f, scale.y * 0.5f, 0), colour, colour);

}

void	OGLRenderer::DrawDebugCross(DebugDrawMode mode, const Vec3Graphics& at, const Vec3Graphics& scale, const Vec3Graphics& colour)
{
	DebugDrawData* target = (mode == DEBUGDRAW_ORTHO ? target = orthoDebugData : target = perspectiveDebugData);

	target->AddLine(at + Vec3Graphics(-scale.x * 0.5f, -scale.y * 0.5f, 0),
		at + Vec3Graphics(scale.x * 0.5f, scale.y * 0.5f, 0), colour, colour);

	target->AddLine(at + Vec3Graphics(scale.x * 0.5f, -scale.y * 0.5f, 0),
		at + Vec3Graphics(-scale.x * 0.5f, scale.y * 0.5f, 0), colour, colour);

}

void	OGLRenderer::DrawDebugCircle(DebugDrawMode mode, const Vec3Graphics& at, const float radius, const Vec3Graphics& colour)
{
	DebugDrawData* target = (mode == DEBUGDRAW_ORTHO ? target = orthoDebugData : target = perspectiveDebugData);

	const int stepCount = 18;
	const float divisor = 360.0f / stepCount;


	for (int i = 0; i < stepCount; ++i)
	{

		float startx = radius * (float)cos(DegToRad(i * divisor)) + at.x;
		float endx = radius * (float)cos(DegToRad((i + 1) * divisor)) + at.x;


		float starty = radius * (float)sin(DegToRad(i * divisor)) + at.y;
		float endy = radius * (float)sin(DegToRad((i + 1) * divisor)) + at.y;

		target->AddLine(Vec3Graphics(startx, starty, at.z),
			Vec3Graphics(endx, endy, at.z), colour, colour);
	}
}


void		OGLRenderer::DrawDebugMatrix(const Mat3Physics& m, const Vec3Physics& position)
{

	OGLRenderer::DrawDebugLine(DEBUGDRAW_PERSPECTIVE, position, position + m.GetCol(0), Vec3Graphics(1.0f, 0.0f, 0.0f), Vec3Graphics(1.0f, 0.0f, 0.0f));
	OGLRenderer::DrawDebugLine(DEBUGDRAW_PERSPECTIVE, position, position + m.GetCol(1), Vec3Graphics(0.0f, 1.0f, 0.0f), Vec3Graphics(0.0f, 1.0f, 0.0f));
	OGLRenderer::DrawDebugLine(DEBUGDRAW_PERSPECTIVE, position, position + m.GetCol(2), Vec3Graphics(0.0f, 0.0f, 1.0f), Vec3Graphics(0.0f, 0.0f, 1.0f));
}


DebugDrawData::DebugDrawData()
{
	glGenVertexArrays(1, &array);
	glGenBuffers(2, buffers);
}

void DebugDrawData::Draw()
{
	if (lines.empty())
		return;
	glBindVertexArray(array);
	glGenBuffers(2, buffers);

	glBindBuffer(GL_ARRAY_BUFFER, buffers[VERTEX_BUFFER]);
	glBufferData(GL_ARRAY_BUFFER, lines.size()*sizeof(Vec3Graphics), &lines[0], GL_DYNAMIC_DRAW);
	glVertexAttribPointer(VERTEX_BUFFER, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(VERTEX_BUFFER);

	glDrawArrays(GL_LINES, 0, lines.size());

	glBindVertexArray(0);
	glDeleteBuffers(2, buffers);

	Clear();
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