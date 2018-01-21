/*
	Copyright 2011-2018 Daniel S. Buckstein

	Licensed under the Apache License, Version 2.0 (the "License");
	you may not use this file except in compliance with the License.
	You may obtain a copy of the License at

		http://www.apache.org/licenses/LICENSE-2.0

	Unless required by applicable law or agreed to in writing, software
	distributed under the License is distributed on an "AS IS" BASIS,
	WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
	See the License for the specific language governing permissions and
	limitations under the License.
*/

/*
	animal3D SDK: Minimal 3D Animation Framework
	By Daniel S. Buckstein
	
	a3_DemoState.c/.cpp
	Demo state function implementations.

	********************************************
	*** THIS IS YOUR DEMO'S MAIN SOURCE FILE ***
	*** Implement your demo logic here.      ***
	********************************************
*/


#include "a3_DemoState.h"


//-----------------------------------------------------------------------------

// OpenGL
#ifdef _WIN32
#include <Windows.h>
#include <GL/GL.h>
#else	// !_WIN32
#include <OpenGL/gl3.h>
#endif	// _WIN32


#include <stdio.h>
#include <stdlib.h>


//-----------------------------------------------------------------------------
// SETUP AND TERMINATION UTILITIES

// set default GL state
void a3demo_setDefaultGraphicsState()
{
	const float lineWidth = 2.0f;
	const float pointSize = 4.0f;

	// lines and points
	glLineWidth(lineWidth);
	glPointSize(pointSize);

	// backface culling
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	// alpha blending
	// result = ( new*[new alpha] ) + ( old*[1 - new alpha] )
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// textures
	glEnable(GL_TEXTURE_2D);

	// background
	glClearColor(0.0f, 0.0f, 0.0, 0.0f);
}


//-----------------------------------------------------------------------------
// LOADING AND UNLOADING

// utility to load geometry
void a3demo_loadGeometry(a3_DemoState *demoState)
{
	// static model transformations
	static const a3mat4 downscale20x = {
		+0.05f, 0.0f, 0.0f, 0.0f,
		0.0f, +0.05f, 0.0f, 0.0f,
		0.0f, 0.0f, +0.05f, 0.0f,
		0.0f, 0.0f, 0.0f, +1.0f,
	};

	// pointer to shared vbo/ibo
	a3_VertexBuffer *vbo_ibo;
	a3_VertexArrayDescriptor *vao;
	a3_VertexDrawable *currentDrawable;
	unsigned int sharedVertexStorage = 0, sharedIndexStorage = 0;
	unsigned int numVerts = 0;
	unsigned int i;


	// file streaming (if requested)
	a3_FileStream fileStream[1] = { 0 };
	const char *const geometryStream = "./data/geom_data.dat";

	// geometry data
	a3_GeometryData sceneShapesData[3] = { 0 };
	a3_GeometryData proceduralShapesData[4] = { 0 };
	a3_GeometryData loadedModelsData[1] = { 0 };
	const unsigned int sceneShapesCount = sizeof(sceneShapesData) / sizeof(a3_GeometryData);
	const unsigned int proceduralShapesCount = sizeof(proceduralShapesData) / sizeof(a3_GeometryData);
	const unsigned int loadedModelsCount = sizeof(loadedModelsData) / sizeof(a3_GeometryData);

	// common index format
	a3_IndexFormatDescriptor sceneCommonIndexFormat[1] = { 0 };


	// procedural scene objects
	// attempt to load stream if requested
	if (demoState->streaming && a3fileStreamOpenRead(fileStream, geometryStream))
	{
		// read from stream

		// static scene objects
		for (i = 0; i < sceneShapesCount; ++i)
			a3fileStreamReadObject(fileStream, sceneShapesData + i, (a3_FileStreamReadFunc)a3geometryLoadDataBinary);

		// procedurally-generated objects
		for (i = 0; i < proceduralShapesCount; ++i)
			a3fileStreamReadObject(fileStream, proceduralShapesData + i, (a3_FileStreamReadFunc)a3geometryLoadDataBinary);

		// loaded model objects
		for (i = 0; i < loadedModelsCount; ++i)
			a3fileStreamReadObject(fileStream, loadedModelsData + i, (a3_FileStreamReadFunc)a3geometryLoadDataBinary);

		// done
		a3fileStreamClose(fileStream);
	}
	// not streaming or stream doesn't exist
	else if (!demoState->streaming || a3fileStreamOpenWrite(fileStream, geometryStream))
	{
		// create new data
		a3_ProceduralGeometryDescriptor sceneShapes[3] = { a3geomShape_none };
		a3_ProceduralGeometryDescriptor proceduralShapes[4] = { a3geomShape_none };
		a3_ProceduralGeometryDescriptor loadedModelShapes[1] = { a3geomShape_none };

		// static scene procedural objects
		//	(axes, grid, skybox)
		a3proceduralCreateDescriptorAxes(sceneShapes + 0, a3geomFlag_wireframe, 0.0f, 1);
		a3proceduralCreateDescriptorPlane(sceneShapes + 1, a3geomFlag_wireframe, a3geomAxis_default, 20.0f, 20.0f, 20, 20);
		a3proceduralCreateDescriptorBox(sceneShapes + 2, a3geomFlag_texcoords, 100.0f, 100.0f, 100.0f, 1, 1, 1);
		for (i = 0; i < sceneShapesCount; ++i)
		{
			a3proceduralGenerateGeometryData(sceneShapesData + i, sceneShapes + i);
			a3fileStreamWriteObject(fileStream, sceneShapesData + i, (a3_FileStreamWriteFunc)a3geometrySaveDataBinary);
		}

		// other procedurally-generated objects
		// ****TO-DO: create procedural geometry descriptors
		//	(tessellated plane for ground, sphere, torus, cylinder)
		//	(see above for examples)

		for (i = 0; i < proceduralShapesCount; ++i)
		{
			// ****TO-DO: generate geometry data from descriptor
			// (see above for example, be sure to use the correct data array)
		}

		// objects loaded from mesh files
		a3modelLoadOBJ(loadedModelsData + 0, "../../../../resource/obj/teapot/teapot.obj", a3model_calculateVertexTangents, downscale20x.mm);
		for (i = 0; i < loadedModelsCount; ++i)
			a3fileStreamWriteObject(fileStream, loadedModelsData + i, (a3_FileStreamWriteFunc)a3geometrySaveDataBinary);

		// done
		a3fileStreamClose(fileStream);
	}


	// GPU data upload process: 
	//	- determine storage requirements
	//	- allocate buffer
	//	- create vertex arrays using unique formats
	//	- create drawable and upload data

	// get storage size
	sharedVertexStorage = numVerts = 0;
	for (i = 0; i < sceneShapesCount; ++i)
	{
		sharedVertexStorage += a3geometryGetVertexBufferSize(sceneShapesData + i);
		numVerts += sceneShapesData[i].numVertices;
	}
	for (i = 0; i < proceduralShapesCount; ++i)
	{
		sharedVertexStorage += a3geometryGetVertexBufferSize(proceduralShapesData + i);
		numVerts += proceduralShapesData[i].numVertices;
	}
	for (i = 0; i < loadedModelsCount; ++i)
	{
		sharedVertexStorage += a3geometryGetVertexBufferSize(loadedModelsData + i);
		numVerts += loadedModelsData[i].numVertices;
	}

	// common index format required for shapes that share vertex formats
	a3geometryCreateIndexFormat(sceneCommonIndexFormat, numVerts);
	sharedIndexStorage = 0;
	for (i = 0; i < sceneShapesCount; ++i)
		sharedIndexStorage += a3indexStorageSpaceRequired(sceneCommonIndexFormat, sceneShapesData[i].numIndices);
	for (i = 0; i < proceduralShapesCount; ++i)
		sharedIndexStorage += a3indexStorageSpaceRequired(sceneCommonIndexFormat, proceduralShapesData[i].numIndices);
	for (i = 0; i < loadedModelsCount; ++i)
		sharedIndexStorage += a3indexStorageSpaceRequired(sceneCommonIndexFormat, loadedModelsData[i].numIndices);
	

	// create shared buffer
	vbo_ibo = demoState->vbo_staticSceneObjectDrawBuffer;
	a3bufferCreateSplit(vbo_ibo, a3buffer_vertex, sharedVertexStorage, sharedIndexStorage, 0, 0);
	sharedVertexStorage = 0;


	// create vertex formats and drawables
	// axes
	vao = demoState->vao_position_color;
	a3geometryGenerateVertexArray(vao, sceneShapesData + 0, vbo_ibo, sharedVertexStorage);
	currentDrawable = demoState->draw_axes;
	sharedVertexStorage += a3geometryGenerateDrawable(currentDrawable, sceneShapesData + 0, vao, vbo_ibo, sceneCommonIndexFormat, 0, 0);

	// grid: position attribute only
	// overlay objects are also just position
	vao = demoState->vao_position;
	a3geometryGenerateVertexArray(vao, sceneShapesData + 1, vbo_ibo, sharedVertexStorage);
	currentDrawable = demoState->draw_grid;
	sharedVertexStorage += a3geometryGenerateDrawable(currentDrawable, sceneShapesData + 1, vao, vbo_ibo, sceneCommonIndexFormat, 0, 0);

	// skybox: position and texture coordinates
	vao = demoState->vao_position_texcoord;
	a3geometryGenerateVertexArray(vao, sceneShapesData + 2, vbo_ibo, sharedVertexStorage);
	currentDrawable = demoState->draw_skybox;
	sharedVertexStorage += a3geometryGenerateDrawable(currentDrawable, sceneShapesData + 2, vao, vbo_ibo, sceneCommonIndexFormat, 0, 0);

	// scene objects: full tangent basis
	vao = demoState->vao_tangent_basis;
	a3geometryGenerateVertexArray(vao, proceduralShapesData + 0, vbo_ibo, sharedVertexStorage);
	currentDrawable = demoState->draw_teapot;
	sharedVertexStorage += a3geometryGenerateDrawable(currentDrawable, loadedModelsData + 0, vao, vbo_ibo, sceneCommonIndexFormat, 0, 0);

	// ****TO-DO: upload PROCEDURAL geometry data by generating drawables
	// (see above examples for the scene shapes and loaded model)
	//	for each drawable, set 'currentDrawable' pointer, then
	//	generate the drawable from the respective data

	
	// release data when done
	for (i = 0; i < sceneShapesCount; ++i)
		a3geometryReleaseData(sceneShapesData + i);
	for (i = 0; i < proceduralShapesCount; ++i)
		a3geometryReleaseData(proceduralShapesData + i);
	for (i = 0; i < loadedModelsCount; ++i)
		a3geometryReleaseData(loadedModelsData + i);
}


// utility to load shaders
void a3demo_loadShaders(a3_DemoState *demoState)
{
	// direct to demo programs
	a3_DemoStateShaderProgram *currentDemoProg;
	int *currentUnif, uLocation;
	unsigned int i, j;

	// list of uniform names: align with uniform list in demo struct!
	const char *uniformNames[demoStateMaxCount_shaderProgramUniform] = {
		// common vertex
		"uMVP",

		// common fragment
		"uColor",
	};


	// some default uniform values
	const float defaultColor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
	const int defaultTexUnits[] = { 0, 1, 2, 3, 4, 5, 6, 7 };


	// list of all unique shaders
	// this is a good idea to avoid multi-loading 
	//	those that are shared between programs
	union {
		struct {
			// vertex shaders
			a3_Shader passthru_transform_vs[1];
			a3_Shader passColor_transform_vs[1];

			// fragment shaders
			a3_Shader drawColorUnif_fs[1];
			a3_Shader drawColorAttrib_fs[1];
		};
	} shaderList = { 0 };
	a3_Shader *const shaderListPtr = (a3_Shader *)(&shaderList);
	const unsigned int numUniqueShaders = sizeof(shaderList) / sizeof(a3_Shader);

	// descriptors to help load shaders; aligned with above list
	struct {
		a3_ShaderType shaderType;
		unsigned int srcCount;
		const char *filePath[8];	// max number of source files per shader
	} shaderDescriptor[] = {

		// BINARY ENCODED SHADER FILES - FOR TESTING ONLY!!!!
		// DO NOT USE THESE FILES FOR YOUR ASSIGNMENTS
		// YOU WILL GET A ZERO IF THESE ARE LEFT UNCOMMENTED
	//	{ a3shader_vertex,		1, { "../../../../resource/glsl/4x/vs/e/passthru_transform_vs4x.glsl" } },
	//	{ a3shader_vertex,		1, { "../../../../resource/glsl/4x/vs/e/passColor_transform_vs4x.glsl" } },
	//
	//	{ a3shader_fragment,	1, { "../../../../resource/glsl/4x/fs/e/drawColorUnif_fs4x.glsl" } },
	//	{ a3shader_fragment,	1, { "../../../../resource/glsl/4x/fs/e/drawColorAttrib_fs4x.glsl" } },


		// ****TO-DO: COMPLETE THE FILES LISTED BELOW
		// (use the above commented files to test)
		// (this will be very useful when we get to multi-pass)
		// (the difference is just the '/e' before the file name)
		{ a3shader_vertex,		1, { "../../../../resource/glsl/4x/vs/passthru_transform_vs4x.glsl" } },
		{ a3shader_vertex,		1, { "../../../../resource/glsl/4x/vs/passColor_transform_vs4x.glsl" } },
	
		{ a3shader_fragment,	1, { "../../../../resource/glsl/4x/fs/drawColorUnif_fs4x.glsl" } },
		{ a3shader_fragment,	1, { "../../../../resource/glsl/4x/fs/drawColorAttrib_fs4x.glsl" } },
	};

	// load unique shaders: 
	//	- load file contents
	//	- create and compile shader object
	//	- release file contents
	for (i = 0; i < numUniqueShaders; ++i)
	{
		a3shaderCreateFromFileList(shaderListPtr + i, shaderDescriptor[i].shaderType,
			shaderDescriptor[i].filePath, shaderDescriptor[i].srcCount);
	}


	// setup programs: 
	//	- create program object
	//	- attach shader objects

	// color attrib program
	currentDemoProg = demoState->prog_drawColor;
	a3shaderProgramCreate(currentDemoProg->program);
	a3shaderProgramAttachShader(currentDemoProg->program, shaderList.passColor_transform_vs);
	a3shaderProgramAttachShader(currentDemoProg->program, shaderList.drawColorAttrib_fs);

	// uniform color program
	currentDemoProg = demoState->prog_drawColorUnif;
	a3shaderProgramCreate(currentDemoProg->program);
	a3shaderProgramAttachShader(currentDemoProg->program, shaderList.passthru_transform_vs);
	a3shaderProgramAttachShader(currentDemoProg->program, shaderList.drawColorUnif_fs);


	// activate a primitive for validation
	// makes sure the specified geometry can draw using programs
	// good idea to activate the drawable with the most attributes
	a3vertexActivateDrawable(demoState->draw_axes);

	// link and validate all programs
	for (i = 0; i < demoStateMaxCount_shaderProgram; ++i)
	{
		currentDemoProg = demoState->shaderProgram + i;
		a3shaderProgramLink(currentDemoProg->program);
		a3shaderProgramValidate(currentDemoProg->program);
	}

	// if linking fails, contingency plan goes here
	// otherwise, release shaders
	for (i = 0; i < numUniqueShaders; ++i)
		a3shaderRelease(shaderListPtr + i);


	// prepare uniforms algorithmically instead of manually for all programs
	for (i = 0; i < demoStateMaxCount_shaderProgram; ++i)
	{
		// activate program
		currentDemoProg = demoState->shaderProgram + i;
		a3shaderProgramActivate(currentDemoProg->program);

		// get uniform locations
		currentUnif = currentDemoProg->uniformLocation;
		for (j = 0; j < demoStateMaxCount_shaderProgramUniform; ++j)
			currentUnif[j] = a3shaderUniformGetLocation(currentDemoProg->program, uniformNames[j]);

		// set default values for all programs that have a uniform that will 
		//	either never change or is consistent for all programs
		if ((uLocation = currentDemoProg->uMVP) >= 0)
			a3shaderUniformSendFloatMat(a3unif_mat4, 0, uLocation, 1, a3identityMat4.mm);
		if ((uLocation = currentDemoProg->uColor) >= 0)
			a3shaderUniformSendFloat(a3unif_vec4, uLocation, 1, defaultColor);
	}

	//done
	a3shaderProgramDeactivate();
	a3vertexDeactivateDrawable();
}


//-----------------------------------------------------------------------------
// release objects
// this is where the union array style comes in handy; don't need a single 
//	release statement for each and every object... just iterate and release!

// utility to unload geometry
void a3demo_unloadGeometry(a3_DemoState *demoState)
{
	a3_BufferObject *currentBuff = demoState->drawDataBuffer,
		*const endBuff = currentBuff + demoStateMaxCount_drawDataBuffer;
	a3_VertexArrayDescriptor *currentVAO = demoState->vertexArray,
		*const endVAO = currentVAO + demoStateMaxCount_vertexArray;
	a3_VertexDrawable *currentDraw = demoState->drawable,
		*const endDraw = currentDraw + demoStateMaxCount_drawable;

	while (currentBuff < endBuff)
		a3bufferRelease(currentBuff++);
	while (currentVAO < endVAO)
		a3vertexArrayReleaseDescriptor(currentVAO++);
	while (currentDraw < endDraw)
		a3vertexReleaseDrawable(currentDraw++);
}


// utility to unload shaders
void a3demo_unloadShaders(a3_DemoState *demoState)
{
	a3_DemoStateShaderProgram *currentProg = demoState->shaderProgram,
		*const endProg = currentProg + demoStateMaxCount_shaderProgram;

	while (currentProg < endProg)
		a3shaderProgramRelease((currentProg++)->program);
}


//-----------------------------------------------------------------------------

// initialize non-asset objects
void a3demo_initScene(a3_DemoState *demoState)
{
	unsigned int i;
	const float cameraAxisPos = 15.0f;

	// all objects
	for (i = 0; i < demoStateMaxCount_sceneObject; ++i)
		a3demo_initSceneObject(demoState->sceneObject + i);

	a3demo_setCameraSceneObject(demoState->sceneCamera, demoState->cameraObject);
	for (i = 0; i < demoStateMaxCount_camera; ++i)
		a3demo_initCamera(demoState->sceneCamera + i);

	// cameras
	demoState->sceneCamera->znear = 1.00f;
	demoState->sceneCamera->zfar = 100.0f;
	demoState->sceneCamera->ctrlMoveSpeed = 10.0f;
	demoState->sceneCamera->ctrlRotateSpeed = 5.0f;
	demoState->sceneCamera->ctrlZoomSpeed = 5.0f;


	// camera's starting orientation depends on "vertical" axis
	// we want the exact same view in either case
	if (demoState->verticalAxis)
	{
		// vertical axis is Y
		demoState->sceneCamera->sceneObject->position.x = +cameraAxisPos;
		demoState->sceneCamera->sceneObject->position.y = +cameraAxisPos;
		demoState->sceneCamera->sceneObject->position.z = +cameraAxisPos;
		demoState->sceneCamera->sceneObject->euler.x = -30.0f;
		demoState->sceneCamera->sceneObject->euler.y = 45.0f;
		demoState->sceneCamera->sceneObject->euler.z = 0.0f;
	}
	else
	{
		// vertical axis is Z
		demoState->sceneCamera->sceneObject->position.x = +cameraAxisPos;
		demoState->sceneCamera->sceneObject->position.y = -cameraAxisPos;
		demoState->sceneCamera->sceneObject->position.z = +cameraAxisPos;
		demoState->sceneCamera->sceneObject->euler.x = 60.0f;
		demoState->sceneCamera->sceneObject->euler.y = 0.0f;
		demoState->sceneCamera->sceneObject->euler.z = 45.0f;
	}

	// same fovy to start
	demoState->sceneCamera->fovy = a3realSixty;

	// demo modes
	demoState->demoMode = 0;
	demoState->demoModeCount = 1;
}


//-----------------------------------------------------------------------------

// the handle release callbacks are no longer valid; since the library was 
//	reloaded, old function pointers are out of scope!
// could reload everything, but that would mean rebuilding GPU data...
//	...or just set new function pointers!
void a3demo_refresh(a3_DemoState *demoState)
{
	a3_BufferObject *currentBuff = demoState->drawDataBuffer,
		*const endBuff = currentBuff + demoStateMaxCount_drawDataBuffer;
	a3_VertexArrayDescriptor *currentVAO = demoState->vertexArray,
		*const endVAO = currentVAO + demoStateMaxCount_vertexArray;
	a3_DemoStateShaderProgram *currentProg = demoState->shaderProgram,
		*const endProg = currentProg + demoStateMaxCount_shaderProgram;

	while (currentBuff < endBuff)
		a3bufferHandleUpdateReleaseCallback(currentBuff++);
	while (currentVAO < endVAO)
		a3vertexArrayHandleUpdateReleaseCallback(currentVAO++);
	while (currentProg < endProg)
		a3shaderProgramHandleUpdateReleaseCallback((currentProg++)->program);
}


// confirm that all graphics objects were unloaded
void a3demo_validateUnload(const a3_DemoState *demoState)
{
	unsigned int handle;
	const a3_BufferObject *currentBuff = demoState->drawDataBuffer,
		*const endBuff = currentBuff + demoStateMaxCount_drawDataBuffer;
	const a3_VertexArrayDescriptor *currentVAO = demoState->vertexArray,
		*const endVAO = currentVAO + demoStateMaxCount_vertexArray;
	const a3_DemoStateShaderProgram *currentProg = demoState->shaderProgram,
		*const endProg = currentProg + demoStateMaxCount_shaderProgram;

	handle = 0;
	currentBuff = demoState->drawDataBuffer;
	while (currentBuff < endBuff)
		handle += (currentBuff++)->handle->handle;
	if (handle)
		printf("\n A3 Warning: One or more draw data buffers not released.");

	handle = 0;
	currentVAO = demoState->vertexArray;
	while (currentVAO < endVAO)
		handle += (currentVAO++)->handle->handle;
	if (handle)
		printf("\n A3 Warning: One or more vertex arrays not released.");

	handle = 0;
	currentProg = demoState->shaderProgram;
	while (currentProg < endProg)
		handle += (currentProg++)->program->handle->handle;
	if (handle)
		printf("\n A3 Warning: One or more shader programs not released.");
}


//-----------------------------------------------------------------------------
// MAIN LOOP

void a3demo_input(a3_DemoState *demoState, double dt)
{
	a3real ctrlRotateSpeed = 1.0f;
	a3real azimuth = 0.0f;
	a3real elevation = 0.0f;
	int rotatingCamera = 0, movingCamera = 0, changingParam = 0;

	// using Xbox controller
	if (a3XboxControlIsConnected(demoState->xcontrol))
	{
		// move and rotate camera using joysticks
		double lJoystick[2], rJoystick[2], lTrigger[1], rTrigger[1];
		a3XboxControlGetJoysticks(demoState->xcontrol, lJoystick, rJoystick);
		a3XboxControlGetTriggers(demoState->xcontrol, lTrigger, rTrigger);

		movingCamera = a3demo_moveSceneObject(demoState->camera->sceneObject, (float)dt * demoState->camera->ctrlMoveSpeed,
			(a3real)(rJoystick[0]),
			(a3real)(*rTrigger - *lTrigger),
			(a3real)(-rJoystick[1])
		);
		// rotate
		{
			ctrlRotateSpeed = 10.0f;
			azimuth = (a3real)(-lJoystick[0]);
			elevation = (a3real)(lJoystick[1]);

			// this really defines which way is "up"
			// mouse's Y motion controls pitch, but X can control yaw or roll
			// controlling yaw makes Y axis seem "up", roll makes Z seem "up"
			rotatingCamera = a3demo_rotateSceneObject(demoState->camera->sceneObject,
				ctrlRotateSpeed * (float)dt * demoState->camera->ctrlRotateSpeed,
				// pitch: vertical tilt
				elevation,
				// yaw/roll depends on "vertical" axis: if y, yaw; if z, roll
				demoState->verticalAxis ? azimuth : a3realZero,
				demoState->verticalAxis ? a3realZero : azimuth);
		}
	}

	// using mouse and keyboard
	else
	{
		// move using WASDEQ
		movingCamera = a3demo_moveSceneObject(demoState->camera->sceneObject, (float)dt * demoState->camera->ctrlMoveSpeed,
			(a3real)a3keyboardGetDifference(demoState->keyboard, a3key_D, a3key_A),
			(a3real)a3keyboardGetDifference(demoState->keyboard, a3key_E, a3key_Q),
			(a3real)a3keyboardGetDifference(demoState->keyboard, a3key_S, a3key_W)
		);
		if (a3mouseIsHeld(demoState->mouse, a3mouse_left))
		{
			azimuth = -(a3real)a3mouseGetDeltaX(demoState->mouse);
			elevation = -(a3real)a3mouseGetDeltaY(demoState->mouse);

			// this really defines which way is "up"
			// mouse's Y motion controls pitch, but X can control yaw or roll
			// controlling yaw makes Y axis seem "up", roll makes Z seem "up"
			rotatingCamera = a3demo_rotateSceneObject(demoState->camera->sceneObject,
				ctrlRotateSpeed * (float)dt * demoState->camera->ctrlRotateSpeed,
				// pitch: vertical tilt
				elevation,
				// yaw/roll depends on "vertical" axis: if y, yaw; if z, roll
				demoState->verticalAxis ? azimuth : a3realZero,
				demoState->verticalAxis ? a3realZero : azimuth);
		}
	}
}

void a3demo_update(a3_DemoState *demoState, double dt)
{
	unsigned int i;

	const float dr = (float)dt * 15.0f;

	// rotate and position objects
	if (demoState->verticalAxis)
	{
		// y-up
		demoState->sphereObject->euler.y = a3trigValid_sind(demoState->sphereObject->euler.y + dr);
		demoState->cylinderObject->euler.y = a3trigValid_sind(demoState->cylinderObject->euler.y + dr);
		demoState->torusObject->euler.y = a3trigValid_sind(demoState->torusObject->euler.y + dr);
		demoState->teapotObject->euler.y = a3trigValid_sind(demoState->teapotObject->euler.y + dr);

		demoState->sphereObject->position.x = +5.0f;
		demoState->cylinderObject->position.z = -5.0f;
		demoState->torusObject->position.x = -5.0f;
		demoState->teapotObject->position.z = +5.0f;

		demoState->groundObject->position.y = -4.0f;
		demoState->groundObject->euler.x = -90.0f;
	}
	else
	{
		// z-up
		demoState->sphereObject->euler.z = a3trigValid_sind(demoState->sphereObject->euler.z + dr);
		demoState->cylinderObject->euler.z = a3trigValid_sind(demoState->cylinderObject->euler.z + dr);
		demoState->torusObject->euler.z = a3trigValid_sind(demoState->torusObject->euler.z + dr);
		demoState->teapotObject->euler.z = a3trigValid_sind(demoState->teapotObject->euler.z + dr);

		demoState->sphereObject->position.x = +5.0f;
		demoState->cylinderObject->position.y = +5.0f;
		demoState->torusObject->position.x = -5.0f;
		demoState->teapotObject->position.y = -5.0f;

		demoState->groundObject->position.z = -4.0f;
	}


	// update scene objects
	for (i = 0; i < demoStateMaxCount_sceneObject; ++i)
		a3demo_updateSceneObject(demoState->sceneObject + i);

	// update cameras
	for (i = 0; i < demoStateMaxCount_camera; ++i)
		a3demo_updateCameraViewProjection(demoState->camera + i);
}

void a3demo_render(const a3_DemoState *demoState)
{
	const a3_VertexDrawable *currentDrawable;
	const a3_DemoStateShaderProgram *currentDemoProgram;

	const int useVerticalY = demoState->verticalAxis;


	// grid lines highlight
	// if Y axis is up, give it a greenish hue
	// if Z axis is up, a bit of blue
	const float gridColor[] = {
		0.15f,
		useVerticalY ? 0.25f : 0.20f,
		useVerticalY ? 0.20f : 0.25f,
		1.0f
	};


	// RGB
	const float rgba4[] = {
		1.0f, 0.0f, 0.0f, 1.0f,	// red
		0.0f, 1.0f, 0.0f, 1.0f,	// green
		0.0f, 0.0f, 1.0f, 1.0f,	// blue
		0.0f, 1.0f, 1.0f, 1.0f,	// cyan
		1.0f, 0.0f, 1.0f, 1.0f,	// magenta
		1.0f, 1.0f, 0.0f, 1.0f,	// yellow
		1.0f, 0.5f, 0.0f, 1.0f,	// orange
		0.0f, 0.5f, 1.0f, 1.0f,	// sky blue
		0.5f, 0.5f, 0.5f, 1.0f,	// solid grey
		0.5f, 0.5f, 0.5f, 0.5f,	// translucent grey
	},
	*const red = rgba4 + 0, *const green = rgba4 + 4, *const blue = rgba4 + 8,
	*const cyan = rgba4 + 12, *const magenta = rgba4 + 16, *const yellow = rgba4 + 20,
	*const orange = rgba4 + 24, *const skyblue = rgba4 + 28,
	*const grey = rgba4 + 32, *const grey_t = rgba4 + 36;


	// model transformations (if needed)
	const a3mat4 convertY2Z = {
		+1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, +1.0f, 0.0f,
		0.0f, -1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f, +1.0f,
	};
	const a3mat4 convertZ2Y = {
		+1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, -1.0f, 0.0f,
		0.0f, +1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f, +1.0f,
	};
	const a3mat4 convertZ2X = {
		0.0f, 0.0f, -1.0f, 0.0f,
		0.0f, +1.0f, 0.0f, 0.0f,
		+1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f, +1.0f,
	};

	// final model matrix and full matrix stack
	a3mat4 modelMat = a3identityMat4, modelMatInv = a3identityMat4, //modelMatOrig = a3identityMat4,
		modelViewProjectionMat = a3identityMat4;

	// current scene object being rendered, for convenience
	const a3_DemoSceneObject *currentSceneObject;


	// reset viewport and clear buffers
	a3framebufferDeactivateSetViewport(a3fbo_depth24, -demoState->frameBorder, -demoState->frameBorder, demoState->frameWidth, demoState->frameHeight);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	// draw grid aligned to world
	currentDemoProgram = demoState->prog_drawColorUnif;
	a3shaderProgramActivate(currentDemoProgram->program);
	currentDrawable = demoState->draw_grid;
	modelViewProjectionMat = demoState->camera->viewProjectionMat;
	if (useVerticalY)
		a3real4x4ConcatL(modelViewProjectionMat.m, convertZ2Y.m);
	a3shaderUniformSendFloatMat(a3unif_mat4, 0, currentDemoProgram->uMVP, 1, modelViewProjectionMat.mm);
	a3shaderUniformSendFloat(a3unif_vec4, currentDemoProgram->uColor, 1, gridColor);
	a3vertexActivateAndRenderDrawable(currentDrawable);


	// draw skybox in uniform color, inverted
	currentDemoProgram = demoState->prog_drawColorUnif;
	a3shaderProgramActivate(currentDemoProgram->program);
	currentDrawable = demoState->draw_skybox;
	modelMatInv = demoState->camera->sceneObject->modelMatInv;
	modelMatInv.v3 = a3wVec4;
	a3real4x4Product(modelViewProjectionMat.m, demoState->camera->projectionMat.m, modelMatInv.m);
	a3shaderUniformSendFloatMat(a3unif_mat4, 0, currentDemoProgram->uMVP, 1, modelViewProjectionMat.mm);
	a3shaderUniformSendFloat(a3unif_vec4, currentDemoProgram->uColor, 1, skyblue);
	glCullFace(GL_FRONT);
	a3vertexActivateAndRenderDrawable(currentDrawable);
	glCullFace(GL_BACK);


	// draw objects: 
	//	- correct "up" axis if needed
	//	- calculate full MVP matrix
	//	- move lighting objects' positions into object space
	//	- send uniforms
	//	- draw

	// draw models
	// ****TO-DO: 
	//	- after visualizing the scene with rotating solid-color objects, 
	//		change to the other shader program and modify the vertex shader 
	//		"passColor_transform_vs4x" to visualize different attributes... 
	//		note the changes!
//	currentDemoProgram = demoState->prog_drawColor;		// uncomment this when ready
	currentDemoProgram = demoState->prog_drawColorUnif;	// comment this when ready
	a3shaderProgramActivate(currentDemoProgram->program);

	// ground
	currentDrawable = demoState->draw_groundPlane;
	currentSceneObject = demoState->groundObject;

	modelMat = currentSceneObject->modelMat;
	a3real4x4TransformInverseIgnoreScale(modelMatInv.m, modelMat.m);
	a3real4x4Product(modelViewProjectionMat.m, demoState->camera->viewProjectionMat.m, modelMat.m);

	a3shaderUniformSendFloatMat(a3unif_mat4, 0, currentDemoProgram->uMVP, 1, modelViewProjectionMat.mm);
	a3shaderUniformSendFloat(a3unif_vec4, currentDemoProgram->uColor, 1, grey_t);
	a3vertexActivateAndRenderDrawable(currentDrawable);

	// ****TO-DO: DRAW GEOMETRY
	// (use the above scene objects for the procedure/steps to draw)


	glDisable(GL_DEPTH_TEST);

	// draw coordinate axes in front of everything
	currentDemoProgram = demoState->prog_drawColor;
	a3shaderProgramActivate(currentDemoProgram->program);
	currentDrawable = demoState->draw_axes;
	a3vertexActivateDrawable(currentDrawable);

	// center of world
	modelViewProjectionMat = demoState->camera->viewProjectionMat;
	a3shaderUniformSendFloatMat(a3unif_mat4, 0, currentDemoProgram->uMVP, 1, modelViewProjectionMat.mm);
	a3vertexRenderActiveDrawable();

	glEnable(GL_DEPTH_TEST);


	// deactivate things
	a3vertexDeactivateDrawable();
	a3shaderProgramDeactivate();


	// HUD
	if (demoState->textInit && demoState->showText)
	{
		// display mode info
		const char *demoModeText[] = {
			"Solid color shapes",
		};


		glDisable(GL_DEPTH_TEST);

		a3textDraw(demoState->text, -0.98f, +0.90f, -1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
			"Demo mode (%u / %u): ", demoState->demoMode + 1, demoState->demoModeCount);
		a3textDraw(demoState->text, -0.98f, +0.80f, -1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
			"    %s", demoModeText[demoState->demoMode]);


		// display controls
		if (a3XboxControlIsConnected(demoState->xcontrol))
		{
			a3textDraw(demoState->text, -0.98f, -0.50f, -1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
				"Xbox controller camera control: ");
			a3textDraw(demoState->text, -0.98f, -0.60f, -1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
				"    Left joystick = rotate | Right joystick, triggers = move");
		}
		else
		{
			a3textDraw(demoState->text, -0.98f, -0.50f, -1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
				"Keyboard/mouse camera control: ");
			a3textDraw(demoState->text, -0.98f, -0.60f, -1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
				"    Left click and drag = rotate | WASDEQ = move");
		}

		a3textDraw(demoState->text, -0.98f, -0.70f, -1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
			"    Toggle demo mode:           ',' prev | next '.' ");
		a3textDraw(demoState->text, -0.98f, -0.80f, -1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
			"    Toggle text display:        't' (toggle) | 'T' (alloc/dealloc) ");
		a3textDraw(demoState->text, -0.98f, -0.90f, -1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
			"    Reload all shader programs: 'P' ****CHECK CONSOLE FOR ERRORS!**** ");

		glEnable(GL_DEPTH_TEST);
	}
}


//-----------------------------------------------------------------------------
