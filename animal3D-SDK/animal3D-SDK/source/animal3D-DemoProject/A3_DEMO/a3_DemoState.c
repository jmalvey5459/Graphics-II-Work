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
// GENERAL UTILITIES

inline a3real4x4r a3demo_setAtlasTransform_internal(a3real4x4p m_out, 
	const unsigned short atlasWidth, const unsigned short atlasHeight,  
	const unsigned short subTexturePosX, const unsigned short subTexturePosY, 
	const unsigned short subTextureWidth, const unsigned short subTextureHeight,
	const unsigned short subTextureBorderPadding, const unsigned short subTextureAdditionalPadding)
{
	a3real4x4SetIdentity(m_out);
	m_out[0][0] = (a3real)(subTextureWidth) / (a3real)(atlasWidth);
	m_out[1][1] = (a3real)(subTextureHeight) / (a3real)(atlasHeight);
	m_out[3][0] = (a3real)(subTexturePosX + subTextureBorderPadding + subTextureAdditionalPadding) / (a3real)(atlasWidth);
	m_out[3][1] = (a3real)(subTexturePosY + subTextureBorderPadding + subTextureAdditionalPadding) / (a3real)(atlasHeight);
	return m_out;
}

inline void a3demo_applyScale_internal(a3_DemoSceneObject *sceneObject, a3real4x4p s)
{
	if (sceneObject->scaleMode)
	{
		if (sceneObject->scaleMode == 1)
		{
			s[0][0] = s[1][1] = s[2][2] = sceneObject->scale.x;
		}
		else
		{
			s[0][0] = sceneObject->scale.x;
			s[1][1] = sceneObject->scale.y;
			s[2][2] = sceneObject->scale.z;
		}
		a3real4x4ConcatL(sceneObject->modelMat.m, s);
	}
}


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

// utility to load framebuffers
void a3demo_loadFramebuffers(a3_DemoState *demoState)
{
	// create framebuffers and change their texture settings if need be
	a3_Framebuffer *fbo;
	unsigned int i, j;

	// storage precision
	const a3_FramebufferColorType colorType_scene = a3fbo_colorRGBA16;
	const a3_FramebufferColorType colorType_post = a3fbo_colorRGBA16;
 
	// initialize framebuffers: 
	//	- scene, with MRT and depth
	//	- compositing, no MRT and no depth
	fbo = demoState->fbo_window_scene;
	a3framebufferCreate(fbo, 3, colorType_scene, a3fbo_depth24, demoState->frameWidth, demoState->frameHeight);

	fbo = demoState->fbo_window_nodepth;
	a3framebufferCreate(fbo, 1, colorType_post, a3fbo_depthDisable, demoState->frameWidth, demoState->frameHeight);


	// change texture settings for all framebuffers
	for (i = 0, fbo = demoState->framebuffer; i < demoStateMaxCount_framebuffer; ++i, ++fbo)
	{
		// color, if applicable
		for (j = 0; j < fbo->color; ++j)
		{
			a3framebufferBindColorTexture(fbo, a3tex_unit00, j);
			a3textureChangeRepeatMode(a3tex_repeatClamp, a3tex_repeatClamp);
			a3textureChangeFilterMode(a3tex_filterLinear);
		}

		// depth, if applicable
		a3framebufferBindDepthTexture(fbo, a3tex_unit00);
		a3textureChangeRepeatMode(a3tex_repeatClamp, a3tex_repeatClamp);
		a3textureChangeFilterMode(a3tex_filterLinear);
	}


	// deactivate texture
	a3textureDeactivate(a3tex_unit00);
}

// utility to load textures
void a3demo_loadTextures(a3_DemoState *demoState)
{
	// pointer to texture
	a3_Texture *tex;
	unsigned int i;

	// list of texture files to load
	const char *texFiles[] = {
		"../../../../resource/tex/sprite/checker.png",
		"../../../../resource/tex/bg/sky_clouds.png",
		"../../../../resource/tex/bg/sky_water.png",
		"../../../../resource/tex/stone/stone_dm.png",
		"../../../../resource/tex/earth/2k/earth_dm_2k.png",
		"../../../../resource/tex/earth/2k/earth_sm_2k.png",

		"../../../../resource/tex/atlas/atlas_scene_dm.png",
		"../../../../resource/tex/atlas/atlas_scene_sm.png",
		"../../../../resource/tex/atlas/atlas_scene_nm.png",
		"../../../../resource/tex/atlas/atlas_scene_hm.png",
	};
	const unsigned int numTextures = sizeof(texFiles) / sizeof(const char *);

	const unsigned short atlasSceneWidth = 2048, atlasSceneHeight = 2048, atlasSceneBorderPad = 8, atlasSceneAdditionalPad = 8;

	for (i = 0; i < numTextures; ++i)
	{
		tex = demoState->texture + i;
		a3textureCreateFromFile(tex, texFiles[i]);
	}

	// change settings on a per-texture basis
	a3textureActivate(demoState->tex_checker, a3tex_unit00);
	a3textureDefaultSettings();	// nearest filtering, repeat on both axes

	a3textureActivate(demoState->tex_sky_clouds, a3tex_unit00);
	a3textureChangeRepeatMode(a3tex_repeatNormal, a3tex_repeatNormal);	// repeat both axes
	a3textureChangeFilterMode(a3tex_filterLinear);	// linear pixel blending

	a3textureActivate(demoState->tex_sky_water, a3tex_unit00);
	a3textureChangeRepeatMode(a3tex_repeatNormal, a3tex_repeatNormal);
	a3textureChangeFilterMode(a3tex_filterLinear);

	a3textureActivate(demoState->tex_stone_dm, a3tex_unit00);
	a3textureChangeRepeatMode(a3tex_repeatNormal, a3tex_repeatNormal);
	a3textureChangeFilterMode(a3tex_filterLinear);

	a3textureActivate(demoState->tex_earth_dm, a3tex_unit00);
	a3textureChangeRepeatMode(a3tex_repeatNormal, a3tex_repeatClamp);	// repeat on U, clamp V
	a3textureChangeFilterMode(a3tex_filterLinear);	// linear pixel blending

	a3textureActivate(demoState->tex_earth_sm, a3tex_unit00);
	a3textureChangeRepeatMode(a3tex_repeatNormal, a3tex_repeatClamp);
	a3textureChangeFilterMode(a3tex_filterLinear);

	a3textureActivate(demoState->tex_atlas_scene_dm, a3tex_unit00);
	a3textureChangeFilterMode(a3tex_filterLinear);

	a3textureActivate(demoState->tex_atlas_scene_sm, a3tex_unit00);
	a3textureChangeFilterMode(a3tex_filterLinear);

	a3textureActivate(demoState->tex_atlas_scene_nm, a3tex_unit00);
	a3textureChangeFilterMode(a3tex_filterLinear);

	a3textureActivate(demoState->tex_atlas_scene_hm, a3tex_unit00);
	a3textureChangeFilterMode(a3tex_filterLinear);


	// set up texture atlas transforms
	a3demo_setAtlasTransform_internal(demoState->atlasEarth.m, atlasSceneWidth, atlasSceneHeight, 
		0, 0, 1024, 512, atlasSceneBorderPad, atlasSceneAdditionalPad);
	a3demo_setAtlasTransform_internal(demoState->atlasMars.m, atlasSceneWidth, atlasSceneHeight,
		0, 544, 1024, 512, atlasSceneBorderPad, atlasSceneAdditionalPad);
	a3demo_setAtlasTransform_internal(demoState->atlasMoon.m, atlasSceneWidth, atlasSceneHeight,
		0, 1088, 1024, 512, atlasSceneBorderPad, atlasSceneAdditionalPad);
	a3demo_setAtlasTransform_internal(demoState->atlasMarble.m, atlasSceneWidth, atlasSceneHeight,
		1056, 0, 512, 512, atlasSceneBorderPad, atlasSceneAdditionalPad);
	a3demo_setAtlasTransform_internal(demoState->atlasCopper.m, atlasSceneWidth, atlasSceneHeight,
		1056, 544, 512, 512, atlasSceneBorderPad, atlasSceneAdditionalPad);
	a3demo_setAtlasTransform_internal(demoState->atlasStone.m, atlasSceneWidth, atlasSceneHeight,
		1600, 0, 256, 256, atlasSceneBorderPad, atlasSceneAdditionalPad);
	a3demo_setAtlasTransform_internal(demoState->atlasChecker.m, atlasSceneWidth, atlasSceneHeight,
		1888, 0, 128, 128, atlasSceneBorderPad, atlasSceneAdditionalPad);


	// done
	a3textureDeactivate(a3tex_unit00);
}

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
	a3_GeometryData sceneShapesData[4] = { 0 };
	a3_GeometryData proceduralShapesData[5] = { 0 };
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
		a3_ProceduralGeometryDescriptor sceneShapes[4] = { a3geomShape_none };
		a3_ProceduralGeometryDescriptor proceduralShapes[5] = { a3geomShape_none };
		a3_ProceduralGeometryDescriptor loadedModelShapes[1] = { a3geomShape_none };

		// static scene procedural objects
		//	(axes, grid, fsq, skybox)
		a3proceduralCreateDescriptorAxes(sceneShapes + 0, a3geomFlag_wireframe, 0.0f, 1);
		a3proceduralCreateDescriptorPlane(sceneShapes + 1, a3geomFlag_wireframe, a3geomAxis_default, 16.0f, 16.0f, 16, 16);
		a3proceduralCreateDescriptorPlane(sceneShapes + 2, a3geomFlag_texcoords, a3geomAxis_default, 2.0f, 2.0f, 1, 1);
		a3proceduralCreateDescriptorBox(sceneShapes + 3, a3geomFlag_texcoords, 100.0f, 100.0f, 100.0f, 1, 1, 1);
		for (i = 0; i < sceneShapesCount; ++i)
		{
			a3proceduralGenerateGeometryData(sceneShapesData + i, sceneShapes + i);
			a3fileStreamWriteObject(fileStream, sceneShapesData + i, (a3_FileStreamWriteFunc)a3geometrySaveDataBinary);
		}

		// other procedurally-generated objects
		a3proceduralCreateDescriptorPlane(proceduralShapes + 0, a3geomFlag_tangents, a3geomAxis_default, 20.0f, 20.0f, 20, 20);
		a3proceduralCreateDescriptorSphere(proceduralShapes + 1, a3geomFlag_tangents, a3geomAxis_default, 1.0f, 32, 24);
		a3proceduralCreateDescriptorCylinder(proceduralShapes + 2, a3geomFlag_tangents, a3geomAxis_default, 1.0f, 1.0f, 32, 1, 1);
		a3proceduralCreateDescriptorTorus(proceduralShapes + 3, a3geomFlag_tangents, a3geomAxis_default, 1.0f, 0.25f, 32, 24);
		a3proceduralCreateDescriptorSphere(proceduralShapes + 4, a3geomFlag_vanilla, a3geomAxis_default, 1.0f, 12, 8);
		for (i = 0; i < proceduralShapesCount; ++i)
		{
			a3proceduralGenerateGeometryData(proceduralShapesData + i, proceduralShapes + i);
			a3fileStreamWriteObject(fileStream, proceduralShapesData + i, (a3_FileStreamWriteFunc)a3geometrySaveDataBinary);
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

	// low-res sphere is also position-only
	currentDrawable = demoState->draw_sphere_lr;
	sharedVertexStorage += a3geometryGenerateDrawable(currentDrawable, proceduralShapesData + 4, vao, vbo_ibo, sceneCommonIndexFormat, 0, 0);

	// fsq and skybox: position and texture coordinates
	vao = demoState->vao_position_texcoord;
	a3geometryGenerateVertexArray(vao, sceneShapesData + 2, vbo_ibo, sharedVertexStorage);
	currentDrawable = demoState->draw_fsq;
	sharedVertexStorage += a3geometryGenerateDrawable(currentDrawable, sceneShapesData + 2, vao, vbo_ibo, sceneCommonIndexFormat, 0, 0);
	currentDrawable = demoState->draw_skybox;
	sharedVertexStorage += a3geometryGenerateDrawable(currentDrawable, sceneShapesData + 3, vao, vbo_ibo, sceneCommonIndexFormat, 0, 0);

	// scene objects: full tangent basis
	vao = demoState->vao_tangent_basis;
	a3geometryGenerateVertexArray(vao, proceduralShapesData + 0, vbo_ibo, sharedVertexStorage);
	currentDrawable = demoState->draw_groundPlane;
	sharedVertexStorage += a3geometryGenerateDrawable(currentDrawable, proceduralShapesData + 0, vao, vbo_ibo, sceneCommonIndexFormat, 0, 0);
	currentDrawable = demoState->draw_sphere;
	sharedVertexStorage += a3geometryGenerateDrawable(currentDrawable, proceduralShapesData + 1, vao, vbo_ibo, sceneCommonIndexFormat, 0, 0);
	currentDrawable = demoState->draw_cylinder;
	sharedVertexStorage += a3geometryGenerateDrawable(currentDrawable, proceduralShapesData + 2, vao, vbo_ibo, sceneCommonIndexFormat, 0, 0);
	currentDrawable = demoState->draw_torus;
	sharedVertexStorage += a3geometryGenerateDrawable(currentDrawable, proceduralShapesData + 3, vao, vbo_ibo, sceneCommonIndexFormat, 0, 0);
	currentDrawable = demoState->draw_teapot;
	sharedVertexStorage += a3geometryGenerateDrawable(currentDrawable, loadedModelsData + 0, vao, vbo_ibo, sceneCommonIndexFormat, 0, 0);
	
	// release data when done
	for (i = 0; i < sceneShapesCount; ++i)
		a3geometryReleaseData(sceneShapesData + i);
	for (i = 0; i < proceduralShapesCount; ++i)
		a3geometryReleaseData(proceduralShapesData + i);
	for (i = 0; i < loadedModelsCount; ++i)
		a3geometryReleaseData(loadedModelsData + i);


	// dummy drawable for point drawing
	// copy any of the existing ones, set vertex count to 1 
	//	and primitive to points
	// DO NOT RELEASE THIS DRAWABLE; it is just a stand-in
	*demoState->dummyDrawable = *demoState->draw_axes;
	demoState->dummyDrawable->count = 1;
	demoState->dummyDrawable->primitive = GL_POINTS;
}


// utility to load shaders
void a3demo_loadShaders(a3_DemoState *demoState)
{
	// direct to demo programs
	a3_DemoStateShaderProgram *currentDemoProg;
	int *currentUnif, uLocation, flag;
	unsigned int i, j;

	// maximum uniform buffer size
	const unsigned int uBlockSzMax = a3shaderUniformBlockMaxSize();

	// list of uniform names: align with uniform list in demo struct!
	const char *uniformNames[demoStateMaxCount_shaderProgramUniform] = {
		// common vertex
		"uMVP",
		"uMV",
		"uP",
		"uP_inv",
		"uAtlas",


		"uLightPos_obj",
		"uEyePos_obj",

		// common geometry
	

		"uParticleCount",
		"uPathMode",
		"uPathWaypoint0",
		"uPathWaypoint1",
		"uPathWaypoint2",
		"uPathWaypointPrev",
		"uPathWaypointCount",
		"uPathSegmentTime",
		"uPathSegmentParam",

		// common fragment
		"uImage0",
		"uImage1",
		"uImage2",
		"uImage3",
		"uImage4",
		"uImage5",
		"uImage6",
		"uImage7",

		"uLightPos_world",
		"uLightPos_eye",
		"uLightRadius",
		"uLightRadiusInvSq",
		"uLightColor",
		"uLightCount",
		"uLightIndex",

		"uTex_dm",
		"uTex_sm",
		"uPixelSize",
		"uColor",


	};
	const char *uniformBlockNames[demoStateMaxCount_shaderProgramUniformBlock] = {
		"ubParticle",
		"ubWaypoint",
		"ubWaypointHandle",
		"ubPointLight",
	};


	// some default uniform values
	const float defaultColor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
	const int defaultTexUnits[] = { 0, 1, 2, 3, 4, 5, 6, 7 };
	const float defaultFloat[] = { 0.0f };
	const int defaultInt[] = { 0 };


	// list of all unique shaders
	// this is a good idea to avoid multi-loading 
	//	those that are shared between programs
	union {
		struct {
			// vertex shaders
			// 07
			a3_DemoStateShader passInstanceID_vs[1];
			a3_DemoStateShader passFlume_vs[1];

			// 06
			a3_DemoStateShader passAttribsView_atlas_transform_vs[1];
			// 02
			a3_DemoStateShader passTexcoord_transform_vs[1];
			// base
			a3_DemoStateShader passthru_transform_vs[1];
			a3_DemoStateShader passColor_transform_vs[1];

			// geometry shaders
			// 07
			a3_DemoStateShader drawCurveSegment_passColor_gs[1];
			a3_DemoStateShader drawLineSegment_gs[1];
			a3_DemoStateShader drawParticles_gs[1];
			a3_DemoStateShader drawTangentBasis_passColor_gs[1];
			a3_DemoStateShader drawTriangleExploded_passAttribs_gs[1];
			a3_DemoStateShader drawTriangleInverted_passAttribs_gs[1];
			a3_DemoStateShader drawFlumeShader_gs[1];
			// fragment shaders

			a3_DemoStateShader drawFlumeColor_fs[1];
			// 06
			a3_DemoStateShader drawPhong_multilight_mrt_fs[1];
			// 02
			a3_DemoStateShader drawTexture_fs[1];


			// base
			a3_DemoStateShader drawColorUnif_fs[1];
			a3_DemoStateShader drawColorAttrib_fs[1];

		};
	} shaderList = {
		{
			// ****REMINDER: 'Encoded' shaders are available for proof-of-concept
			//	testing ONLY!  Insert /e before file names.
			// DO NOT SUBMIT WORK USING ENCODED SHADERS OR YOU WILL GET ZERO!!!

			// vs
			// 07
			{ { { 0 },	"Pass instance ID VS",	a3shader_vertex  ,	1,{ "../../../../resource/glsl/4x/vs/07-geometry/passInstanceID_vs4x.glsl" } } },
			{ { { 0 },	"Draw Flume VS",	a3shader_geometry,	1,{ "../../../../resource/glsl/4x/vs/07-geometry/flumeShader_vs4x.glsl" } } },

			// 06
			{ { { 0 },	"Pass g-buff attr VS",	a3shader_vertex  ,	1,{ "../../../../resource/glsl/4x/vs/06-deferred/e/passAttribsView_atlas_transform_vs4x.glsl" } } },
			// 02
			{ { { 0 },	"Texture VS",			a3shader_vertex  ,	1,{ "../../../../resource/glsl/4x/vs/02-shading/e/passTexcoord_transform_vs4x.glsl" } } },
			// base
			{ { { 0 },	"Pass-thru VS",			a3shader_vertex  ,	1,{ "../../../../resource/glsl/4x/vs/e/passthru_transform_vs4x.glsl" } } },
			{ { { 0 },	"Pass color VS",		a3shader_vertex  ,	1,{ "../../../../resource/glsl/4x/vs/e/passColor_transform_vs4x.glsl" } } },

			// gs
			// 07
			{ { { 0 },	"Draw curve seg GS",	a3shader_geometry,	1,{ "../../../../resource/glsl/4x/gs/07-geometry/drawCurveSegment_passColor_gs4x.glsl" } } },
			{ { { 0 },	"Draw line seg GS",		a3shader_geometry,	1,{ "../../../../resource/glsl/4x/gs/07-geometry/drawLineSegment_gs4x.glsl" } } },
			{ { { 0 },	"Draw particles GS",	a3shader_geometry,	1,{ "../../../../resource/glsl/4x/gs/07-geometry/drawParticles_gs4x.glsl" } } },
			{ { { 0 },	"Draw TBN basis GS",	a3shader_geometry,	1,{ "../../../../resource/glsl/4x/gs/07-geometry/drawTangentBasis_passColor_gs4x.glsl" } } },
			{ { { 0 },	"Draw tri exploded GS",	a3shader_geometry,	1,{ "../../../../resource/glsl/4x/gs/07-geometry/drawTriangle_exploded_passAttribs_gs4x.glsl" } } },
			{ { { 0 },	"Draw tri inverted GS",	a3shader_geometry,	1,{ "../../../../resource/glsl/4x/gs/07-geometry/drawTriangle_inverted_passAttribs_gs4x.glsl" } } },
			{ { { 0 },	"Draw Flume GS",	a3shader_geometry,	1,{ "../../../../resource/glsl/4x/gs/07-geometry/flumeShader_gs4x.glsl" } } },

			// fs
			//07
			{ { { 0 },	"Draw Flume FS",	a3shader_geometry,	1,{ "../../../../resource/glsl/4x/fs/07-geometry/flumeShader_fs4x.glsl" } }},
			// 06
			{ { { 0 },	"Phong multi-light FS",	a3shader_fragment,	1,{ "../../../../resource/glsl/4x/fs/06-deferred/e/drawPhong_multilight_mrt_fs4x.glsl" } } },
			// 02
			{ { { 0 },	"Texture FS",			a3shader_fragment,	1,{ "../../../../resource/glsl/4x/fs/02-shading/e/drawTexture_fs4x.glsl" } } },
			// base
			{ { { 0 },	"Draw unif color FS",	a3shader_fragment,	1,{ "../../../../resource/glsl/4x/fs/e/drawColorUnif_fs4x.glsl" } } },
			{ { { 0 },	"Draw attrib color FS",	a3shader_fragment,	1,{ "../../../../resource/glsl/4x/fs/drawColorAttrib_fs4x.glsl" } } },
			
		}
	};
	a3_DemoStateShader *const shaderListPtr = (a3_DemoStateShader *)(&shaderList), *shaderPtr;
	const unsigned int numUniqueShaders = sizeof(shaderList) / sizeof(a3_DemoStateShader);


	printf("\n\n---------------- LOAD SHADERS STARTED  ---------------- \n");


	// load unique shaders: 
	//	- load file contents
	//	- create and compile shader object
	//	- release file contents
	for (i = 0; i < numUniqueShaders; ++i)
	{
		shaderPtr = shaderListPtr + i;
		flag = a3shaderCreateFromFileList(shaderPtr->shader, shaderPtr->shaderType,
			shaderPtr->filePath, shaderPtr->srcCount);
		if (flag == 0)
			printf("\n ^^^^ SHADER %u '%s' FAILED TO COMPILE \n\n", i, shaderPtr->shaderName);
	//	else if (flag < 0)
	//		printf("\n CANNOT COMPILE SHADER %u '%s' \n\n", i, shaderPtr->shaderName);
	}


	// setup programs: 
	//	- create program object
	//	- attach shader objects

	// 07 programs
	// draw curve segment
	currentDemoProg = demoState->prog_drawCurveSegment;
	strcpy(currentDemoProg->programName, "Draw curve segment prog");
	a3shaderProgramCreate(currentDemoProg->program);
	a3shaderProgramAttachShader(currentDemoProg->program, shaderList.passInstanceID_vs->shader);
	a3shaderProgramAttachShader(currentDemoProg->program, shaderList.drawColorAttrib_fs->shader);
	a3shaderProgramAttachShader(currentDemoProg->program, shaderList.drawCurveSegment_passColor_gs->shader);

	// draw line segment
	currentDemoProg = demoState->prog_drawLineSegment;
	strcpy(currentDemoProg->programName, "Draw line segment prog");
	a3shaderProgramCreate(currentDemoProg->program);
	a3shaderProgramAttachShader(currentDemoProg->program, shaderList.passInstanceID_vs->shader);
	a3shaderProgramAttachShader(currentDemoProg->program, shaderList.drawColorUnif_fs->shader);
	a3shaderProgramAttachShader(currentDemoProg->program, shaderList.drawLineSegment_gs->shader);

	// draw particles
	currentDemoProg = demoState->prog_drawParticles;
	strcpy(currentDemoProg->programName, "Draw particle list prog");
	a3shaderProgramCreate(currentDemoProg->program);
	a3shaderProgramAttachShader(currentDemoProg->program, shaderList.passInstanceID_vs->shader);
	a3shaderProgramAttachShader(currentDemoProg->program, shaderList.drawColorUnif_fs->shader);
	a3shaderProgramAttachShader(currentDemoProg->program, shaderList.drawParticles_gs->shader);

	// draw tangent basis
	currentDemoProg = demoState->prog_drawTangentBasis;
	strcpy(currentDemoProg->programName, "Draw tangent basis prog");
	a3shaderProgramCreate(currentDemoProg->program);
	a3shaderProgramAttachShader(currentDemoProg->program, shaderList.passAttribsView_atlas_transform_vs->shader);
	a3shaderProgramAttachShader(currentDemoProg->program, shaderList.drawColorAttrib_fs->shader);
	a3shaderProgramAttachShader(currentDemoProg->program, shaderList.drawTangentBasis_passColor_gs->shader);

	// Phong exploded triangle
	currentDemoProg = demoState->prog_drawPhong_multilight_mrt_exploded;
	strcpy(currentDemoProg->programName, "Phong multi-light exploded prog");
	a3shaderProgramCreate(currentDemoProg->program);
	a3shaderProgramAttachShader(currentDemoProg->program, shaderList.passAttribsView_atlas_transform_vs->shader);
	a3shaderProgramAttachShader(currentDemoProg->program, shaderList.drawPhong_multilight_mrt_fs->shader);
	a3shaderProgramAttachShader(currentDemoProg->program, shaderList.drawTriangleExploded_passAttribs_gs->shader);

	// Phong inverted triangle
	currentDemoProg = demoState->prog_drawPhong_multilight_mrt_inverted;
	strcpy(currentDemoProg->programName, "Phong multi-light inverted prog");
	a3shaderProgramCreate(currentDemoProg->program);
	a3shaderProgramAttachShader(currentDemoProg->program, shaderList.passAttribsView_atlas_transform_vs->shader);
	a3shaderProgramAttachShader(currentDemoProg->program, shaderList.drawPhong_multilight_mrt_fs->shader);
	a3shaderProgramAttachShader(currentDemoProg->program, shaderList.drawTriangleInverted_passAttribs_gs->shader);

	//Flume Shader
	currentDemoProg = demoState->prog_drawFlume;
	strcpy(currentDemoProg->programName, "draw Flume prog");
	a3shaderProgramCreate(currentDemoProg->program);
	a3shaderProgramAttachShader(currentDemoProg->program, shaderList.passAttribsView_atlas_transform_vs->shader);
	a3shaderProgramAttachShader(currentDemoProg->program, shaderList.drawPhong_multilight_mrt_fs->shader);
	a3shaderProgramAttachShader(currentDemoProg->program, shaderList.drawFlumeShader_gs->shader);


	// 06 programs
	// multi-light Phong
	currentDemoProg = demoState->prog_drawPhong_multilight_mrt;
	strcpy(currentDemoProg->programName, "Phong multi-light program");
	a3shaderProgramCreate(currentDemoProg->program);
	a3shaderProgramAttachShader(currentDemoProg->program, shaderList.passAttribsView_atlas_transform_vs->shader);
	a3shaderProgramAttachShader(currentDemoProg->program, shaderList.drawPhong_multilight_mrt_fs->shader);


	// 02 programs
	// texturing
	currentDemoProg = demoState->prog_drawTexture;
	strcpy(currentDemoProg->programName, "Texture program");
	a3shaderProgramCreate(currentDemoProg->program);
	a3shaderProgramAttachShader(currentDemoProg->program, shaderList.passTexcoord_transform_vs->shader);
	a3shaderProgramAttachShader(currentDemoProg->program, shaderList.drawTexture_fs->shader);


	// base programs
	// color attrib program
	currentDemoProg = demoState->prog_drawColor;
	strcpy(currentDemoProg->programName, "Draw attrib color program");
	a3shaderProgramCreate(currentDemoProg->program);
	a3shaderProgramAttachShader(currentDemoProg->program, shaderList.passColor_transform_vs->shader);
	a3shaderProgramAttachShader(currentDemoProg->program, shaderList.drawColorAttrib_fs->shader);

	// uniform color program
	currentDemoProg = demoState->prog_drawColorUnif;
	strcpy(currentDemoProg->programName, "Draw unif color program");
	a3shaderProgramCreate(currentDemoProg->program);
	a3shaderProgramAttachShader(currentDemoProg->program, shaderList.passthru_transform_vs->shader);
	a3shaderProgramAttachShader(currentDemoProg->program, shaderList.drawColorUnif_fs->shader);


	// activate a primitive for validation
	// makes sure the specified geometry can draw using programs
	// good idea to activate the drawable with the most attributes
	a3vertexActivateDrawable(demoState->draw_axes);

	// link and validate all programs
	for (i = 0; i < demoStateMaxCount_shaderProgram; ++i)
	{
		currentDemoProg = demoState->shaderProgram + i;
		flag = a3shaderProgramLink(currentDemoProg->program);
		if (flag == 0)
			printf("\n ^^^^ PROGRAM %u '%s' FAILED TO LINK \n\n", i, currentDemoProg->programName);
	//	else if (flag < 0)
	//		printf("\n CANNOT LINK PROGRAM %u '%s' \n\n", i, currentDemoProg->programName);

		flag = a3shaderProgramValidate(currentDemoProg->program);
		if (flag == 0)
			printf("\n ^^^^ PROGRAM %u '%s' FAILED TO VALIDATE \n\n", i, currentDemoProg->programName);
	//	else if (flag < 0)
	//		printf("\n CANNOT VALIDATE PROGRAM %u '%s' \n\n", i, currentDemoProg->programName);
	}

	// if linking fails, contingency plan goes here
	// otherwise, release shaders
	for (i = 0; i < numUniqueShaders; ++i)
	{
		shaderPtr = shaderListPtr + i;
		a3shaderRelease(shaderPtr->shader);
	}


	// prepare uniforms algorithmically instead of manually for all programs
	for (i = 0; i < demoStateMaxCount_shaderProgram; ++i)
	{
		// activate program
		currentDemoProg = demoState->shaderProgram + i;
		a3shaderProgramActivate(currentDemoProg->program);

		// get uniform and uniform block locations
		currentUnif = currentDemoProg->uniformLocation;
		for (j = 0; j < demoStateMaxCount_shaderProgramUniform; ++j)
			currentUnif[j] = a3shaderUniformGetLocation(currentDemoProg->program, uniformNames[j]);

		currentUnif = currentDemoProg->uniformBlockLocation;
		for (j = 0; j < demoStateMaxCount_shaderProgramUniformBlock; ++j)
			currentUnif[j] = a3shaderUniformBlockGetLocation(currentDemoProg->program, uniformBlockNames[j]);


		// set default values for all programs that have a uniform that will 
		//	either never change or is consistent for all programs

		// common VS
		if ((uLocation = currentDemoProg->uMVP) >= 0)
			a3shaderUniformSendFloatMat(a3unif_mat4, 0, uLocation, 1, a3identityMat4.mm);
		if ((uLocation = currentDemoProg->uMV) >= 0)
			a3shaderUniformSendFloatMat(a3unif_mat4, 0, uLocation, 1, a3identityMat4.mm);
		if ((uLocation = currentDemoProg->uP) >= 0)
			a3shaderUniformSendFloatMat(a3unif_mat4, 0, uLocation, 1, a3identityMat4.mm);
		if ((uLocation = currentDemoProg->uP_inv) >= 0)
			a3shaderUniformSendFloatMat(a3unif_mat4, 0, uLocation, 1, a3identityMat4.mm);
		if ((uLocation = currentDemoProg->uAtlas) >= 0)
			a3shaderUniformSendFloatMat(a3unif_mat4, 0, uLocation, 1, a3identityMat4.mm);

		if ((uLocation = currentDemoProg->uLightPos_obj) >= 0)
			a3shaderUniformSendFloat(a3unif_vec4, uLocation, 1, a3wVec4.v);
		if ((uLocation = currentDemoProg->uEyePos_obj) >= 0)
			a3shaderUniformSendFloat(a3unif_vec4, uLocation, 1, a3wVec4.v);

		// common GS

		if ((uLocation = currentDemoProg->uParticleCount) >= 0)
			a3shaderUniformSendInt(a3unif_single, uLocation, 1, defaultInt);
		if ((uLocation = currentDemoProg->uPathMode) >= 0)
			a3shaderUniformSendInt(a3unif_single, uLocation, 1, defaultInt);
		if ((uLocation = currentDemoProg->uPathWaypoint0) >= 0)
			a3shaderUniformSendInt(a3unif_single, uLocation, 1, defaultInt);
		if ((uLocation = currentDemoProg->uPathWaypoint1) >= 0)
			a3shaderUniformSendInt(a3unif_single, uLocation, 1, defaultInt);
		if ((uLocation = currentDemoProg->uPathWaypoint2) >= 0)
			a3shaderUniformSendInt(a3unif_single, uLocation, 1, defaultInt);
		if ((uLocation = currentDemoProg->uPathWaypointPrev) >= 0)
			a3shaderUniformSendInt(a3unif_single, uLocation, 1, defaultInt);
		if ((uLocation = currentDemoProg->uPathWaypointCount) >= 0)
			a3shaderUniformSendInt(a3unif_single, uLocation, 1, defaultInt);
		if ((uLocation = currentDemoProg->uPathSegmentTime) >= 0)
			a3shaderUniformSendFloat(a3unif_single, uLocation, 1, defaultFloat);
		if ((uLocation = currentDemoProg->uPathSegmentParam) >= 0)
			a3shaderUniformSendFloat(a3unif_single, uLocation, 1, defaultFloat);

		// common FS
		if ((uLocation = currentDemoProg->uImage0) >= 0)
			a3shaderUniformSendInt(a3unif_single, uLocation, 1, defaultTexUnits + 0);
		if ((uLocation = currentDemoProg->uImage1) >= 0)
			a3shaderUniformSendInt(a3unif_single, uLocation, 1, defaultTexUnits + 1);
		if ((uLocation = currentDemoProg->uImage2) >= 0)
			a3shaderUniformSendInt(a3unif_single, uLocation, 1, defaultTexUnits + 2);
		if ((uLocation = currentDemoProg->uImage3) >= 0)
			a3shaderUniformSendInt(a3unif_single, uLocation, 1, defaultTexUnits + 3);
		if ((uLocation = currentDemoProg->uImage4) >= 0)
			a3shaderUniformSendInt(a3unif_single, uLocation, 1, defaultTexUnits + 4);
		if ((uLocation = currentDemoProg->uImage5) >= 0)
			a3shaderUniformSendInt(a3unif_single, uLocation, 1, defaultTexUnits + 5);
		if ((uLocation = currentDemoProg->uImage6) >= 0)
			a3shaderUniformSendInt(a3unif_single, uLocation, 1, defaultTexUnits + 6);
		if ((uLocation = currentDemoProg->uImage7) >= 0)
			a3shaderUniformSendInt(a3unif_single, uLocation, 1, defaultTexUnits + 7);

		if ((uLocation = currentDemoProg->uLightPos_world) >= 0)
			a3shaderUniformSendFloat(a3unif_vec4, uLocation, 1, a3wVec4.v);
		if ((uLocation = currentDemoProg->uLightPos_eye) >= 0)
			a3shaderUniformSendFloat(a3unif_vec4, uLocation, 1, a3wVec4.v);
		if ((uLocation = currentDemoProg->uLightRadius) >= 0)
			a3shaderUniformSendFloat(a3unif_single, uLocation, 1, defaultFloat);
		if ((uLocation = currentDemoProg->uLightRadiusInvSq) >= 0)
			a3shaderUniformSendFloat(a3unif_single, uLocation, 1, defaultFloat);
		if ((uLocation = currentDemoProg->uLightColor) >= 0)
			a3shaderUniformSendFloat(a3unif_vec4, uLocation, 1, defaultColor);
		if ((uLocation = currentDemoProg->uLightCount) >= 0)
			a3shaderUniformSendInt(a3unif_single, uLocation, 1, defaultInt);
		if ((uLocation = currentDemoProg->uLightIndex) >= 0)
			a3shaderUniformSendInt(a3unif_single, uLocation, 1, defaultInt);

		if ((uLocation = currentDemoProg->uTex_dm) >= 0)
			a3shaderUniformSendInt(a3unif_single, uLocation, 1, defaultTexUnits + 0);
		if ((uLocation = currentDemoProg->uTex_sm) >= 0)
			a3shaderUniformSendInt(a3unif_single, uLocation, 1, defaultTexUnits + 1);
		if ((uLocation = currentDemoProg->uPixelSize) >= 0)
			a3shaderUniformSendFloat(a3unif_vec2, uLocation, 1, a3zeroVec2.v);
		if ((uLocation = currentDemoProg->uColor) >= 0)
			a3shaderUniformSendFloat(a3unif_vec4, uLocation, 1, defaultColor);

		// default block bindings
		if ((uLocation = currentDemoProg->ubParticle) >= 0)
			a3shaderUniformBlockBind(currentDemoProg->program, 0, currentDemoProg->ubParticle);
		if ((uLocation = currentDemoProg->ubWaypoint) >= 0)
			a3shaderUniformBlockBind(currentDemoProg->program, 1, currentDemoProg->ubWaypoint);
		if ((uLocation = currentDemoProg->ubWaypointHandle) >= 0)
			a3shaderUniformBlockBind(currentDemoProg->program, 2, currentDemoProg->ubWaypointHandle);
		if ((uLocation = currentDemoProg->ubPointLight) >= 0)
			a3shaderUniformBlockBind(currentDemoProg->program, 3, currentDemoProg->ubPointLight);
	}


	// set up uniform buffers
	a3bufferCreate(demoState->ubo_waypointBuffer, a3buffer_uniform, uBlockSzMax, 0);
	a3bufferCreate(demoState->ubo_waypointHandleBuffer, a3buffer_uniform, uBlockSzMax, 0);
	a3bufferCreate(demoState->ubo_lightUniformBuffer, a3buffer_uniform, uBlockSzMax, 0);


	printf("\n\n---------------- LOAD SHADERS FINISHED ---------------- \n");

	//done
	a3shaderProgramDeactivate();
	a3vertexDeactivateDrawable();
}


//-----------------------------------------------------------------------------
// release objects
// this is where the union array style comes in handy; don't need a single 
//	release statement for each and every object... just iterate and release!

// utility to unload framebuffers
void a3demo_unloadFramebuffers(a3_DemoState *demoState)
{
	a3_Framebuffer *currentFBO = demoState->framebuffer,
		*const endFBO = currentFBO + demoStateMaxCount_framebuffer;

	while (currentFBO < endFBO)
		a3framebufferRelease(currentFBO++);
}
 
// utility to unload textures
void a3demo_unloadTextures(a3_DemoState *demoState)
{
	a3_Texture *currentTex = demoState->texture,
		*const endTex = currentTex + demoStateMaxCount_texture;

	while (currentTex < endTex)
		a3textureRelease(currentTex++);
}

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
	a3_UniformBuffer *currentUBO = demoState->uniformBuffer,
		*const endUBO = currentUBO + demoStateMaxCount_uniformBuffer;
	a3_DemoStateShaderProgram *currentProg = demoState->shaderProgram,
		*const endProg = currentProg + demoStateMaxCount_shaderProgram;

	while (currentUBO < endUBO)
		a3bufferRelease(currentUBO++);
	while (currentProg < endProg)
		a3shaderProgramRelease((currentProg++)->program);
}


//-----------------------------------------------------------------------------

// initialize non-asset objects
void a3demo_initScene(a3_DemoState *demoState)
{
	unsigned int i;
	const a3real cameraAxisPos = 15.0f;

	a3_DemoPointLight *pointLight;


	// all objects
	for (i = 0; i < demoStateMaxCount_sceneObject; ++i)
		a3demo_initSceneObject(demoState->sceneObject + i);

	a3demo_setCameraSceneObject(demoState->sceneCamera, demoState->cameraObject + 0);
	a3demo_setCameraSceneObject(demoState->editCamera, demoState->cameraObject + 1);
	for (i = 0; i < demoStateMaxCount_camera; ++i)
		a3demo_initCamera(demoState->camera + i);

	// cameras
	demoState->sceneCamera->znear = 1.00f;
	demoState->sceneCamera->zfar = 100.0f;
	demoState->sceneCamera->ctrlMoveSpeed = 10.0f;
	demoState->sceneCamera->ctrlRotateSpeed = 5.0f;
	demoState->sceneCamera->ctrlZoomSpeed = 5.0f;

	demoState->editCamera->znear = -100.0f;
	demoState->editCamera->zfar = +100.0f;


	// camera's starting orientation depends on "vertical" axis
	// we want the exact same view in either case
	if (demoState->verticalAxis)
	{
		// vertical axis is Y
		// camera
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
		// camera
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
	demoState->demoPipelineCount = 2;

	// pipelines: 
	// A: forward lighting
	//	- scene
	//		- lighting
	//		- diffuse shading
	//		- specular shading
	//		- depth
	//	- inverted (same outputs)
	//	- exploded (same outputs)
	demoState->demoPassCount[0] = 4;
	demoState->demoOutputCount[0][0] = 4;
	demoState->demoOutputCount[0][1] = 4;
	demoState->demoOutputCount[0][2] = 4;
	
	// B: edit mode
	//	- linear path
	//	- Catmull-Rom path
	//	- cubic Bezier path
	//	- cubic Hermite path
	demoState->demoPassCount[1] = 4;
	demoState->demoOutputCount[1][0] = 1;
	demoState->demoOutputCount[1][1] = 1;
	demoState->demoOutputCount[1][2] = 1;
	demoState->demoOutputCount[1][3] = 1;


	// initialize other objects
	demoState->displayGrid = 1;
	demoState->displayAxes = 1;
	demoState->displaySkybox = 1;


	// scene objects
	demoState->sphereObject->scale.x = 2.0f;
	demoState->sphereObject->scaleMode = 1;	// unif

	demoState->cylinderObject->scale.x = 1.0f;
	demoState->cylinderObject->scale.y = 1.0f;
	demoState->cylinderObject->scale.z = 4.0f;
	demoState->cylinderObject->scaleMode = 2;	// non-unif

	demoState->torusObject->scale.x = 2.0f;
	demoState->torusObject->scaleMode = 1;	// unif


	// lights
	demoState->lightCount = 4;

	// first light is hard-coded
	pointLight = demoState->pointLight;
	pointLight->worldPos = a3wVec4;
	if (demoState->verticalAxis)
		pointLight->worldPos.y = 10.0f;
	else
		pointLight->worldPos.z = 10.0f;
	pointLight->radius = 50.0f;
	pointLight->radiusInvSq = a3recip(pointLight->radius * pointLight->radius);
	pointLight->color = a3oneVec4;

	// all other lights are random
	a3randomSetSeed(0);
	for (i = 1, pointLight = demoState->pointLight + i;
		i < demoStateMaxCount_light;
		++i, ++pointLight)
	{
		// set to zero vector
		pointLight->worldPos = a3wVec4;

		// random positions
		pointLight->worldPos.x = a3randomRange(-10.0f, +10.0f);
		if (demoState->verticalAxis)
		{
			pointLight->worldPos.z = -a3randomRange(-10.0f, +10.0f);
			pointLight->worldPos.y = -a3randomRange(-2.0f, +8.0f);
		}
		else
		{
			pointLight->worldPos.y = a3randomRange(-10.0f, +10.0f);
			pointLight->worldPos.z = a3randomRange(-2.0f, +8.0f);
		}

		// random colors
		pointLight->color.r = a3randomNormalized();
		pointLight->color.g = a3randomNormalized();
		pointLight->color.b = a3randomNormalized();
		pointLight->color.a = a3realOne;

		// random radius
		pointLight->radius = a3randomRange(10.0f, 50.0f);
		pointLight->radiusInvSq = a3recip(pointLight->radius * pointLight->radius);
	}


	// paths
	demoState->pathFollowerPosition = a3wVec4;
	demoState->pathFollowerPosition.x = 640.0f;
	demoState->pathFollowerPosition.y = 360.0f;
	demoState->pathFollowerSize = 10.0f;
	demoState->pathSegmentDuration = 4.0f;
}


//-----------------------------------------------------------------------------

// the handle release callbacks are no longer valid; since the library was 
//	reloaded, old function pointers are out of scope!
// could reload everything, but that would mean rebuilding GPU data...
//	...or just set new function pointers!
void a3demo_refresh(a3_DemoState *demoState)
{
	a3_Framebuffer *currentFBO = demoState->framebuffer,
		*const endFBO = currentFBO + demoStateMaxCount_framebuffer;
	a3_Texture *currentTex = demoState->texture,
		*const endTex = currentTex + demoStateMaxCount_texture;
	a3_BufferObject *currentBuff = demoState->drawDataBuffer,
		*const endBuff = currentBuff + demoStateMaxCount_drawDataBuffer;
	a3_VertexArrayDescriptor *currentVAO = demoState->vertexArray,
		*const endVAO = currentVAO + demoStateMaxCount_vertexArray;
	a3_DemoStateShaderProgram *currentProg = demoState->shaderProgram,
		*const endProg = currentProg + demoStateMaxCount_shaderProgram;
	a3_UniformBuffer *currentUBO = demoState->uniformBuffer,
		*const endUBO = currentUBO + demoStateMaxCount_uniformBuffer;

	while (currentFBO < endFBO)
		a3framebufferHandleUpdateReleaseCallback(currentFBO++);
	while (currentTex < endTex)
		a3textureHandleUpdateReleaseCallback(currentTex++);
	while (currentBuff < endBuff)
		a3bufferHandleUpdateReleaseCallback(currentBuff++);
	while (currentVAO < endVAO)
		a3vertexArrayHandleUpdateReleaseCallback(currentVAO++);
	while (currentUBO < endUBO)
		a3bufferHandleUpdateReleaseCallback(currentUBO++);
	while (currentProg < endProg)
		a3shaderProgramHandleUpdateReleaseCallback((currentProg++)->program);
}


// confirm that all graphics objects were unloaded
void a3demo_validateUnload(const a3_DemoState *demoState)
{
	unsigned int handle;
	const a3_Framebuffer *currentFBO = demoState->framebuffer,
		*const endFBO = currentFBO + demoStateMaxCount_framebuffer;
	const a3_Texture *currentTex = demoState->texture,
		*const endTex = currentTex + demoStateMaxCount_texture;
	const a3_BufferObject *currentBuff = demoState->drawDataBuffer,
		*const endBuff = currentBuff + demoStateMaxCount_drawDataBuffer;
	const a3_VertexArrayDescriptor *currentVAO = demoState->vertexArray,
		*const endVAO = currentVAO + demoStateMaxCount_vertexArray;
	const a3_DemoStateShaderProgram *currentProg = demoState->shaderProgram,
		*const endProg = currentProg + demoStateMaxCount_shaderProgram;
	const a3_UniformBuffer *currentUBO = demoState->uniformBuffer,
		*const endUBO = currentUBO + demoStateMaxCount_uniformBuffer;

	handle = 0;
	currentFBO = demoState->framebuffer;
	while (currentFBO < endFBO)
		handle += (currentFBO++)->handle->handle;
	if (handle)
		printf("\n A3 Warning: One or more framebuffers not released.");

	handle = 0;
	currentTex = demoState->texture;
	while (currentTex < endTex)
		handle += (currentTex++)->handle->handle;
	if (handle)
		printf("\n A3 Warning: One or more textures not released.");

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
	currentUBO = demoState->uniformBuffer;
	while (currentUBO < endUBO)
		handle += (currentUBO++)->handle->handle;
	if (handle)
		printf("\n A3 Warning: One or more uniform buffers not released.");

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


	// do nothing if not in scene mode
	if (demoState->demoPipelineMode != 0)
		return;


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

	const int useVerticalY = demoState->verticalAxis;

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

	// tmp matrix for scale
	a3mat4 scaleMat = a3identityMat4;

	// light pointers
	a3_DemoPointLight *pointLight;
	a3mat4 *lightMVP;


	// rotate and position objects
	if (useVerticalY)
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


	// apply corrections if required
	// grid
	demoState->gridTransform = useVerticalY ? convertZ2Y : a3identityMat4;

	// skybox position
	demoState->skyboxObject->modelMat.v3 = demoState->cameraObject->modelMat.v3;

	if (useVerticalY)
	{
		// sphere's axis is Z
		a3real4x4ConcatL(demoState->sphereObject->modelMat.m, convertZ2Y.m);
	}
	else
	{
		// need to rotate skybox if Z-up
		a3real4x4ConcatL(demoState->skyboxObject->modelMat.m, convertY2Z.m);

		// teapot's axis is Y
		a3real4x4ConcatL(demoState->teapotObject->modelMat.m, convertY2Z.m);
	}

	// always rotate cylinder and torus
	a3real4x4ConcatL(demoState->cylinderObject->modelMat.m, convertZ2X.m);
	a3real4x4ConcatL(demoState->torusObject->modelMat.m, convertZ2X.m);


	// apply scales
	a3demo_applyScale_internal(demoState->sphereObject, scaleMat.m);
	a3demo_applyScale_internal(demoState->cylinderObject, scaleMat.m);
	a3demo_applyScale_internal(demoState->torusObject, scaleMat.m);


	// grid lines highlight
	// if Y axis is up, give it a greenish hue
	// if Z axis is up, a bit of blue
	demoState->gridColor = a3wVec4;
	if (useVerticalY)
		demoState->gridColor.y = 0.25f;
	else
		demoState->gridColor.z = 0.25f;


	// update lights
	for (i = 0, pointLight = demoState->pointLight + i, lightMVP = demoState->pointLightMVP + i; 
		i < demoState->lightCount; 
		++i, ++pointLight, ++lightMVP)
	{
		// set light scale and world position
		*lightMVP = a3identityMat4;
		lightMVP->v3 = pointLight->worldPos;

		// convert to view space and retrieve view position
		a3real4x4ConcatR(demoState->cameraObject->modelMatInv.m, lightMVP->m);
		pointLight->viewPos = lightMVP->v3;

		// complete by converting to clip space
		a3real4x4ConcatR(demoState->sceneCamera->projectionMat.m, lightMVP->m);
	}


	// update path follower
	// simple keyframe controller
	if (demoState->demoPipelineMode == 1 && demoState->waypointCount > 1)
	{
		// waypoint indices: current (i), current end (i + 1), next (i + 2), and previous (i - 1)
		unsigned int w0, w1, w2, wp;

		// update time
		demoState->pathSegmentTime += (a3real)dt;

		// if segment time exceeds duration, go to next segment
		if (demoState->pathSegmentTime >= demoState->pathSegmentDuration)
		{
			demoState->pathSegmentTime -= demoState->pathSegmentDuration;
			demoState->pathWaypoint0 = (demoState->pathWaypoint0 + 1) % (demoState->waypointCount - 1);
		}

		// update param
		demoState->pathSegmentParam = demoState->pathSegmentTime / demoState->pathSegmentDuration;

		// waypoint indices
		w0 = demoState->pathWaypoint0;
		w1 = demoState->pathWaypoint1 = w0 < demoState->waypointCount - 1 ? w0 + 1 : w0;
		w2 = demoState->pathWaypoint2 = w1 < demoState->waypointCount - 1 ? w1 + 1 : w1;
		wp = demoState->pathWaypointPrev = w0 > 0 ? w0 - 1 : w0;

		// update path follower position based on algorithm and param
		switch (demoState->demoPassMode[demoState->demoPipelineMode])
		{
			// linear interpolation
		case 0: 
			a3real3Lerp(demoState->pathFollowerPosition.v, 
				demoState->waypoint[w0].v, demoState->waypoint[w1].v, 
				demoState->pathSegmentParam);
			break;

			// Catmull-Rom interpolation
		case 1:
			a3real3CatmullRom(demoState->pathFollowerPosition.v,
				demoState->waypoint[wp].v, demoState->waypoint[w0].v, demoState->waypoint[w1].v, demoState->waypoint[w2].v,
				demoState->pathSegmentParam);
			break;

			// cubic Bezier interpolation
		case 2:
			a3real3Bezier3(demoState->pathFollowerPosition.v,
				demoState->waypoint[w0].v, demoState->waypointHandle[w0].v, demoState->waypointHandle[w1].v, demoState->waypoint[w1].v,
				demoState->pathSegmentParam);
			break;

			// cubic Hermite interpolation
		case 3:
			a3real3HermiteControl(demoState->pathFollowerPosition.v,
				demoState->waypoint[w0].v, demoState->waypoint[w1].v, demoState->waypointHandle[w0].v, demoState->waypointHandle[w1].v,
				demoState->pathSegmentParam);
			break;
		}
	}


	// send uniform data
	// first reset "used" counter so it can actually fit the data
	*(demoState->ubo_waypointBuffer->used) = 0;
	a3bufferFill(demoState->ubo_waypointBuffer, 0, sizeof(a3vec4) * demoState->waypointCount, demoState->waypoint, 0);

	*(demoState->ubo_waypointHandleBuffer->used) = 0;
	a3bufferFill(demoState->ubo_waypointHandleBuffer, 0, sizeof(a3vec4) * demoState->waypointCount, demoState->waypointHandle, 0);

	*(demoState->ubo_lightUniformBuffer->used) = 0;
	a3bufferFill(demoState->ubo_lightUniformBuffer, 0, sizeof(a3_DemoPointLight) * demoState->lightCount, demoState->pointLight, 0);
}

void a3demo_render(const a3_DemoState *demoState)
{
	const a3_VertexDrawable *currentDrawable;
	const a3_DemoStateShaderProgram *currentDemoProgram;

	unsigned int i, j;

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


	// bias matrix
	const a3mat4 bias = {
		0.5f, 0.0f, 0.0f, 0.0f,
		0.0f, 0.5f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.5f, 0.0f,
		0.5f, 0.5f, 0.5f, 1.0f,
	};

	// final model matrix and full matrix stack
	a3mat4 modelViewProjectionMat = a3identityMat4, modelViewMat = a3identityMat4, modelMat = a3identityMat4;
		//viewProjectionBiasMat = a3identityMat4, modelViewProjectionBiasMat = a3identityMat4;

	// current scene object being rendered, for convenience
	const a3_DemoSceneObject *currentSceneObject;

	// framebuffers to use for rendering and display
	const a3_Framebuffer *currentFBO, *displayFBO;

	// display mode for current pipeline
	// ensures we don't go through the whole pipeline if not needed
	const unsigned int demoPipeline = demoState->demoPipelineMode, demoPipelineCount = demoState->demoPipelineCount;
	const unsigned int demoPass = demoState->demoPassMode[demoPipeline], demoPassCount = demoState->demoPassCount[demoPipeline];
	const unsigned int demoOutput = demoState->demoOutputMode[demoPipeline][demoPass], demoOutputCount = demoState->demoOutputCount[demoPipeline][demoPass];


	//-------------------------------------------------------------------------
	// 1) SCENE PASS: render scene with desired shader
	//	- render shapes using appropriate shaders
	//	- capture color and depth

	// activate FBO and clear buffers
	currentFBO = displayFBO = demoState->fbo_window_scene;
	a3framebufferActivate(currentFBO);
	

	// full clear
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	if (demoState->displayGrid && demoPipeline == 0)
	{
		// draw grid aligned to world
		currentDemoProgram = demoState->prog_drawColorUnif;
		a3shaderProgramActivate(currentDemoProgram->program);
		currentDrawable = demoState->draw_grid;
		modelViewProjectionMat = demoState->sceneCamera->viewProjectionMat;
		a3real4x4ConcatL(modelViewProjectionMat.m, demoState->gridTransform.m);
		a3shaderUniformSendFloatMat(a3unif_mat4, 0, currentDemoProgram->uMVP, 1, modelViewProjectionMat.mm);
		a3shaderUniformSendFloat(a3unif_vec4, currentDemoProgram->uColor, 1, demoState->gridColor.v);
		a3vertexActivateAndRenderDrawable(currentDrawable);
	}


	// calculate VP-B matrix
	//	(gives us screen-space from some viewer's POV)
//	a3real4x4Product(viewProjectionBiasMat.m, bias.m, demoState->sceneCamera->viewProjectionMat.m);

	// how to use: multiply by a different object's model matrix 
	//	and store in MVP-B matrix: 
//	a3real4x4Product(modelViewProjectionBiasMat.m, viewProjectionBiasMat.m, currentSceneObject->modelMat.m);


	// draw objects: 
	//	- correct "up" axis if needed
	//	- calculate full MVP matrix
	//	- move lighting objects' positions into object space
	//	- send uniforms
	//	- draw
	
	// activate atlases to their respective texture slots
	// these are shared textures; send atlas transforms to use different parts!
	a3textureActivate(demoState->tex_atlas_scene_dm, a3tex_unit00);
	a3textureActivate(demoState->tex_atlas_scene_sm, a3tex_unit01);

	// multiple geometry passes
	for (i = 0, j = demoState->displayTangentBasis + 1; i < j && demoPipeline == 0; ++i)
	{
		// select forward algorithm
		if (i == 0)
		{
			switch (demoPass)
			{
				// Phong
			case 0:
				currentDemoProgram = demoState->prog_drawPhong_multilight_mrt;
				break;

				// Phong inverted
			case 1:
				currentDemoProgram = demoState->prog_drawPhong_multilight_mrt_inverted;
				break;

				// Phong exploded
			case 2:
				currentDemoProgram = demoState->prog_drawPhong_multilight_mrt_exploded;
				break;
				//flume 
			case 3:
				currentDemoProgram = demoState->prog_drawFlume;
				

				break;
			
			}
			a3shaderProgramActivate(currentDemoProgram->program);

			a3shaderUniformBufferActivate(demoState->ubo_lightUniformBuffer, 3);
			a3shaderUniformSendInt(a3unif_single, currentDemoProgram->uLightCount, 1, &demoState->lightCount);
		}

		// draw tangent basis
		else
		{
			currentDemoProgram = demoState->prog_drawTangentBasis;
			a3shaderProgramActivate(currentDemoProgram->program);
		}

		// send other shared data
		// projection matrix
		a3shaderUniformSendFloatMat(a3unif_mat4, 0, currentDemoProgram->uP, 1, demoState->sceneCamera->projectionMat.mm);

		// ground
		currentDrawable = demoState->draw_groundPlane;
		currentSceneObject = demoState->groundObject;
	//	a3real4x4Product(modelViewProjectionMat.m, demoState->sceneCamera->viewProjectionMat.m, currentSceneObject->modelMat.m);
	//	a3shaderUniformSendFloatMat(a3unif_mat4, 0, currentDemoProgram->uMVP, 1, modelViewProjectionMat.mm);
		a3real4x4Product(modelViewMat.m, demoState->cameraObject->modelMatInv.m, currentSceneObject->modelMat.m);
		a3shaderUniformSendFloatMat(a3unif_mat4, 0, currentDemoProgram->uMV, 1, modelViewMat.mm);
		a3shaderUniformSendFloatMat(a3unif_mat4, 0, currentDemoProgram->uAtlas, 1, demoState->atlasStone.mm);
		a3vertexActivateAndRenderDrawable(currentDrawable);

		// sphere
		currentDrawable = demoState->draw_sphere;
		currentSceneObject = demoState->sphereObject;
	//	a3real4x4Product(modelViewProjectionMat.m, demoState->sceneCamera->viewProjectionMat.m, currentSceneObject->modelMat.m);
	//	a3shaderUniformSendFloatMat(a3unif_mat4, 0, currentDemoProgram->uMVP, 1, modelViewProjectionMat.mm);
		a3real4x4Product(modelViewMat.m, demoState->cameraObject->modelMatInv.m, currentSceneObject->modelMat.m);
		a3shaderUniformSendFloatMat(a3unif_mat4, 0, currentDemoProgram->uMV, 1, modelViewMat.mm);
		a3shaderUniformSendFloatMat(a3unif_mat4, 0, currentDemoProgram->uAtlas, 1, demoState->atlasEarth.mm);
		a3vertexActivateAndRenderDrawable(currentDrawable);

		// cylinder
		currentDrawable = demoState->draw_cylinder;
		currentSceneObject = demoState->cylinderObject;
	//	a3real4x4Product(modelViewProjectionMat.m, demoState->sceneCamera->viewProjectionMat.m, currentSceneObject->modelMat.m);
	//	a3shaderUniformSendFloatMat(a3unif_mat4, 0, currentDemoProgram->uMVP, 1, modelViewProjectionMat.mm);
		a3real4x4Product(modelViewMat.m, demoState->cameraObject->modelMatInv.m, currentSceneObject->modelMat.m);
		a3shaderUniformSendFloatMat(a3unif_mat4, 0, currentDemoProgram->uMV, 1, modelViewMat.mm);
		a3shaderUniformSendFloatMat(a3unif_mat4, 0, currentDemoProgram->uAtlas, 1, demoState->atlasMarble.mm);
		a3vertexActivateAndRenderDrawable(currentDrawable);

		// torus
		currentDrawable = demoState->draw_torus;
		currentSceneObject = demoState->torusObject;
	//	a3real4x4Product(modelViewProjectionMat.m, demoState->sceneCamera->viewProjectionMat.m, currentSceneObject->modelMat.m);
	//	a3shaderUniformSendFloatMat(a3unif_mat4, 0, currentDemoProgram->uMVP, 1, modelViewProjectionMat.mm);
		a3real4x4Product(modelViewMat.m, demoState->cameraObject->modelMatInv.m, currentSceneObject->modelMat.m);
		a3shaderUniformSendFloatMat(a3unif_mat4, 0, currentDemoProgram->uMV, 1, modelViewMat.mm);
		a3shaderUniformSendFloatMat(a3unif_mat4, 0, currentDemoProgram->uAtlas, 1, demoState->atlasMars.mm);
		a3vertexActivateAndRenderDrawable(currentDrawable);
	
		// teapot
		currentDrawable = demoState->draw_teapot;
		currentSceneObject = demoState->teapotObject;
	//	a3real4x4Product(modelViewProjectionMat.m, demoState->sceneCamera->viewProjectionMat.m, currentSceneObject->modelMat.m);
	//	a3shaderUniformSendFloatMat(a3unif_mat4, 0, currentDemoProgram->uMVP, 1, modelViewProjectionMat.mm);
		a3real4x4Product(modelViewMat.m, demoState->cameraObject->modelMatInv.m, currentSceneObject->modelMat.m);
		a3shaderUniformSendFloatMat(a3unif_mat4, 0, currentDemoProgram->uMV, 1, modelViewMat.mm);
		a3shaderUniformSendFloatMat(a3unif_mat4, 0, currentDemoProgram->uAtlas, 1, demoState->atlasChecker.mm);
		a3vertexActivateAndRenderDrawable(currentDrawable);
	}


	//-------------------------------------------------------------------------
	// 2) CURVES
	//	- render curve objects

	if (demoPipeline == 1)
	{
		if (demoState->waypointCount)
		{
			// use dummy drawable so that only one vertex is drawn a number of times
			// we are not rendering the actual geometry, just need VS to trigger
			currentDrawable = demoState->dummyDrawable;
			a3vertexActivateDrawable(currentDrawable);


			// draw all waypoints and handles
			currentDemoProgram = demoState->prog_drawParticles;
			a3shaderProgramActivate(currentDemoProgram->program);

			// send common uniforms
			a3real4x4Product(modelViewMat.m, demoState->editCamera->sceneObject->modelMatInv.m, a3identityMat4.m);
			a3shaderUniformSendFloatMat(a3unif_mat4, 0, currentDemoProgram->uP, 1, demoState->editCamera->projectionMat.mm);
			a3shaderUniformSendFloatMat(a3unif_mat4, 0, currentDemoProgram->uMV, 1, modelViewMat.mm);
			a3shaderUniformSendInt(a3unif_single, currentDemoProgram->uParticleCount, 1, &demoState->waypointCount);

			// bind to particle uniform block
			a3shaderUniformBufferActivate(demoState->ubo_waypointBuffer, 0);
			a3shaderUniformSendFloat(a3unif_vec4, currentDemoProgram->uColor, 1, red);

			// render a single point to trigger VS then GS
			a3vertexRenderActiveDrawable();

			// now do handles
			a3shaderUniformBufferActivate(demoState->ubo_waypointHandleBuffer, 0);
			a3shaderUniformSendFloat(a3unif_vec4, currentDemoProgram->uColor, 1, green);

			// render
			a3vertexRenderActiveDrawable();


			// now draw lines from waypoints to handles
			currentDemoProgram = demoState->prog_drawLineSegment;
			a3shaderProgramActivate(currentDemoProgram->program);

			modelViewProjectionMat = demoState->editCamera->viewProjectionMat;
			a3shaderUniformSendFloatMat(a3unif_mat4, 0, currentDemoProgram->uMVP, 1, modelViewProjectionMat.mm);
			a3shaderUniformSendFloat(a3unif_vec4, currentDemoProgram->uColor, 1, blue);
			a3shaderUniformBufferActivate(demoState->ubo_waypointBuffer, 1);
			a3shaderUniformBufferActivate(demoState->ubo_waypointHandleBuffer, 2);

			// render instanced to draw multiple times
			a3vertexRenderActiveDrawableInstanced(demoState->waypointCount);


			// finally, draw curves
			currentDemoProgram = demoState->prog_drawCurveSegment;
			a3shaderProgramActivate(currentDemoProgram->program);

			modelViewProjectionMat = demoState->editCamera->viewProjectionMat;
			a3shaderUniformSendFloatMat(a3unif_mat4, 0, currentDemoProgram->uMVP, 1, modelViewProjectionMat.mm);
			a3shaderUniformSendInt(a3unif_single, currentDemoProgram->uPathMode, 1, &demoPass);
			a3shaderUniformSendInt(a3unif_single, currentDemoProgram->uPathWaypoint0, 1, &demoState->pathWaypoint0);
			a3shaderUniformSendInt(a3unif_single, currentDemoProgram->uPathWaypoint1, 1, &demoState->pathWaypoint1);
			a3shaderUniformSendInt(a3unif_single, currentDemoProgram->uPathWaypoint2, 1, &demoState->pathWaypoint2);
			a3shaderUniformSendInt(a3unif_single, currentDemoProgram->uPathWaypointPrev, 1, &demoState->pathWaypointPrev);
			a3shaderUniformSendInt(a3unif_single, currentDemoProgram->uPathWaypointCount, 1, &demoState->waypointCount);
			a3shaderUniformSendFloat(a3unif_single, currentDemoProgram->uPathSegmentTime, 1, &demoState->pathSegmentTime);
			a3shaderUniformSendFloat(a3unif_single, currentDemoProgram->uPathSegmentParam, 1, &demoState->pathSegmentParam);
			a3shaderUniformBufferActivate(demoState->ubo_waypointBuffer, 1);
			a3shaderUniformBufferActivate(demoState->ubo_waypointHandleBuffer, 2);

			// render instanced to draw multiple times
			if (demoState->waypointCount > 1)
				a3vertexRenderActiveDrawableInstanced(demoState->waypointCount - 1);
		}


		// draw object at path follower position
		a3real4x4SetScale(modelMat.m, demoState->pathFollowerSize);
		modelMat.v3 = demoState->pathFollowerPosition;
		a3real4x4Product(modelViewProjectionMat.m, demoState->editCamera->viewProjectionMat.m, modelMat.m);

		currentDemoProgram = demoState->prog_drawColorUnif;
		a3shaderProgramActivate(currentDemoProgram->program);
		a3shaderUniformSendFloatMat(a3unif_mat4, 0, currentDemoProgram->uMVP, 1, modelViewProjectionMat.mm);
		a3shaderUniformSendFloat(a3unif_vec4, currentDemoProgram->uColor, 1, orange);
		currentDrawable = demoState->draw_teapot;
		a3vertexActivateAndRenderDrawable(currentDrawable);

		// axes
		currentDemoProgram = demoState->prog_drawColor;
		a3shaderProgramActivate(currentDemoProgram->program);
		a3shaderUniformSendFloatMat(a3unif_mat4, 0, currentDemoProgram->uMVP, 1, modelViewProjectionMat.mm);
		glDisable(GL_DEPTH_TEST);
		currentDrawable = demoState->draw_axes;
		a3vertexActivateAndRenderDrawable(currentDrawable);
		glEnable(GL_DEPTH_TEST);
	}


	//-------------------------------------------------------------------------
	// 3) DISPLAY TO FBO
	//	- render skybox
	//	- render FSQ with display texture

	currentFBO = demoState->fbo_window_nodepth;
	a3framebufferActivate(currentFBO);

	currentDemoProgram = demoState->prog_drawTexture;
	a3shaderProgramActivate(currentDemoProgram->program);

	// display skybox or clear
	if (demoState->displaySkybox && demoPipeline == 0)
	{
		// draw skybox with texture, inverted
		currentDrawable = demoState->draw_skybox;
		currentSceneObject = demoState->skyboxObject;
		a3real4x4Product(modelViewProjectionMat.m, demoState->sceneCamera->viewProjectionMat.m, currentSceneObject->modelMat.m);
		a3shaderUniformSendFloatMat(a3unif_mat4, 0, currentDemoProgram->uMVP, 1, modelViewProjectionMat.mm);
		a3textureActivate(demoState->tex_sky_clouds, a3tex_unit00);

		glDepthFunc(GL_ALWAYS);
		glCullFace(GL_FRONT);
		a3vertexActivateAndRenderDrawable(currentDrawable);
		glCullFace(GL_BACK);
		glDepthFunc(GL_LEQUAL);
	}
	else
	{
		// clearing is expensive!
		// instead, draw skybox and force depth to farthest possible value in scene
		// we could call this a "skybox clear" because it serves both purposes
		glClear(GL_COLOR_BUFFER_BIT);
	}

	// bind scene image to FSQ
	if (demoOutput < demoOutputCount - 1 || demoOutputCount < 2)
		a3framebufferBindColorTexture(displayFBO, a3tex_unit00, demoOutput);
	else
		a3framebufferBindDepthTexture(displayFBO, a3tex_unit00);

	// switch display FBO for later
	displayFBO = currentFBO;

	// draw FSQ with blending
	a3shaderUniformSendFloatMat(a3unif_mat4, 0, currentDemoProgram->uMVP, 1, a3identityMat4.mm);
	currentDrawable = demoState->draw_fsq;
	a3vertexActivateAndRenderDrawable(currentDrawable);


	//-------------------------------------------------------------------------
	// DISPLAY PASS: if doing post-processing, select output texture and render FSQ

	a3framebufferDeactivateSetViewport(a3fbo_depthDisable, -demoState->frameBorder, -demoState->frameBorder, demoState->frameWidth, demoState->frameHeight);

	a3framebufferBindColorTexture(displayFBO, a3tex_unit00, 0);

	// draw without blend to avoid transparent background and needing to clear
	glDisable(GL_BLEND);
	a3vertexRenderActiveDrawable();
	glEnable(GL_BLEND);


	//-------------------------------------------------------------------------
	// superimpose axes
	if (demoState->displayAxes && demoPipeline == 0)
	{
		glDisable(GL_DEPTH_TEST);

		// draw coordinate axes in front of everything
		currentDemoProgram = demoState->prog_drawColor;
		a3shaderProgramActivate(currentDemoProgram->program);
		currentDrawable = demoState->draw_axes;
		a3vertexActivateDrawable(currentDrawable);

		// center of world from current viewer
		// also draw other viewer/viewer-like object in scene
		modelViewProjectionMat = demoState->sceneCamera->viewProjectionMat;
		a3shaderUniformSendFloatMat(a3unif_mat4, 0, currentDemoProgram->uMVP, 1, modelViewProjectionMat.mm);
		a3vertexRenderActiveDrawable();

		glEnable(GL_DEPTH_TEST);
	}


	// deactivate things
	a3vertexDeactivateDrawable();
	a3shaderProgramDeactivate();
	a3textureDeactivate(a3tex_unit07);
	a3textureDeactivate(a3tex_unit06);
	a3textureDeactivate(a3tex_unit05);
	a3textureDeactivate(a3tex_unit04);
	a3textureDeactivate(a3tex_unit03);
	a3textureDeactivate(a3tex_unit02);
	a3textureDeactivate(a3tex_unit01);
	a3textureDeactivate(a3tex_unit00);


	// HUD
	if (demoState->textInit && demoState->showText)
	{
		// display mode info
		const char *pipelineText[demoStateMaxPipelineModes] = {
			"Scene viewer",
			"Curve editor",
		};
		const char *passText[demoStateMaxPipelineModes][demoStateMaxPassModes] = {
			{
				"Scene with lighting",
				"Scene with lighting (inverted)",
				"Scene with lighting (exploded)",
				"Tennis Court Flume",
			},
			{
				"Linear path",
				"Catmull-Rom path",
				"Cubic Bezier path",
				"Cubic Hermite path",
			},
		};
	

		// text color
		const a3vec3 col = { a3realZero, a3realQuarter, a3realHalf };

		// set viewport to window size
		glViewport(0, 0, demoState->windowWidth, demoState->windowHeight);
		glDisable(GL_DEPTH_TEST);

		a3textDraw(demoState->text, -0.98f, +0.90f, -1.0f, col.r, col.g, col.b, 1.0f,
			"Demo pipeline (%u / %u) (',' prev | next '.'): %s", demoPipeline + 1, demoPipelineCount, pipelineText[demoPipeline]);
		a3textDraw(demoState->text, -0.98f, +0.80f, -1.0f, col.r, col.g, col.b, 1.0f,
			"    Pipeline pass (%u / %u) ('<' | '>'): %s", demoPass + 1, demoPassCount, passText[demoPipeline][demoPass]);
	//	a3textDraw(demoState->text, -0.98f, +0.70f, -1.0f, col.r, col.g, col.b, 1.0f,
	//		"        Pass output (%u / %u) ('{' | '}'): %s", demoOutput + 1, demoOutputCount, outputText[demoPipeline][demoPass][demoOutput]);

		if (demoPipeline == 0)
		{
			a3textDraw(demoState->text, -0.98f, +0.60f, -1.0f, col.r, col.g, col.b, 1.0f,
				"GRID in scene (toggle 'g') %d | AXES overlay ('x') %d | SKYBOX backdrop ('b') %d", demoState->displayGrid, demoState->displayAxes, demoState->displaySkybox);

			a3textDraw(demoState->text, -0.98f, +0.50f, -1.0f, col.r, col.g, col.b, 1.0f,
				"Tangent basis overlay ('n') %d", demoState->displayTangentBasis);

			// display controls
			if (a3XboxControlIsConnected(demoState->xcontrol))
			{
				a3textDraw(demoState->text, -0.98f, -0.60f, -1.0f, col.r, col.g, col.b, 1.0f,
					"Xbox controller camera control: ");
				a3textDraw(demoState->text, -0.98f, -0.70f, -1.0f, col.r, col.g, col.b, 1.0f,
					"    Left joystick = rotate | Right joystick, triggers = move");
			}
			else
			{
				a3textDraw(demoState->text, -0.98f, -0.60f, -1.0f, col.r, col.g, col.b, 1.0f,
					"Keyboard/mouse camera control: ");
				a3textDraw(demoState->text, -0.98f, -0.70f, -1.0f, col.r, col.g, col.b, 1.0f,
					"    Left click & drag = rotate | WASDEQ = move | wheel = zoom");
			}
		}
		else
		{
			a3textDraw(demoState->text, -0.98f, +0.60f, -1.0f, col.r, col.g, col.b, 1.0f,
				"Waypoint count: %u", demoState->waypointCount);

			a3textDraw(demoState->text, -0.98f, +0.50f, -1.0f, col.r, col.g, col.b, 1.0f,
				"Path segment %u | time %.3f / duration %.3f = param %.3f", demoState->pathWaypoint0, 
				demoState->pathSegmentTime, demoState->pathSegmentDuration, demoState->pathSegmentParam);

			// controls
			a3textDraw(demoState->text, -0.98f, -0.60f, -1.0f, col.r, col.g, col.b, 1.0f,
				"Left click = place waypoint, drag to adjust handle");
			a3textDraw(demoState->text, -0.98f, -0.70f, -1.0f, col.r, col.g, col.b, 1.0f,
				"Right click = delete previous waypoint and handle");
		}

		// global controls
		a3textDraw(demoState->text, -0.98f, -0.80f, -1.0f, col.r, col.g, col.b, 1.0f,
			"Toggle text display:        't' (toggle) | 'T' (alloc/dealloc) ");
		a3textDraw(demoState->text, -0.98f, -0.90f, -1.0f, col.r, col.g, col.b, 1.0f,
			"Reload all shader programs: 'P' ****CHECK CONSOLE FOR ERRORS!**** ");

		glEnable(GL_DEPTH_TEST);
	}
}


//-----------------------------------------------------------------------------
