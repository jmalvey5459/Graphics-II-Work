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
	
	a3_DemoState.h
	Demo state interface and programmer function declarations.

	********************************************
	*** THIS IS YOUR DEMO'S MAIN HEADER FILE ***
	********************************************
*/

#ifndef __ANIMAL3D_DEMOSTATE_H
#define __ANIMAL3D_DEMOSTATE_H


//-----------------------------------------------------------------------------
// animal3D framework includes

#include "animal3D/animal3D.h"


//-----------------------------------------------------------------------------
// other demo includes

#include "_utilities/a3_DemoSceneObject.h"
#include "_utilities/a3_DemoShaderProgram.h"


//-----------------------------------------------------------------------------

#ifdef __cplusplus
extern "C"
{
#else	// !__cplusplus
	typedef struct a3_DemoState					a3_DemoState;
#endif	// __cplusplus


//-----------------------------------------------------------------------------

	// object maximum counts for easy array storage
	// good idea to make these numbers greater than what you actually need 
	//	and if you end up needing more just increase the count... there's 
	//	more than enough memory to hold extra objects
	enum a3_DemoStateObjectMaxCounts
	{
		demoStateMaxCount_sceneObject = 8,
		demoStateMaxCount_camera = 2,
		demoStateMaxCount_light = 4,
		demoStateMaxCount_waypoint = 32,
		demoStateMaxCount_timer = 1,
		demoStateMaxCount_framebuffer = 2,
		demoStateMaxCount_texture = 16,
		demoStateMaxCount_drawDataBuffer = 1,
		demoStateMaxCount_vertexArray = 4,
		demoStateMaxCount_drawable = 16,
		demoStateMaxCount_uniformBuffer = 4,
		demoStateMaxCount_shaderProgram = 16,
	};

	// additional counters for demo modes
	enum a3_DemoStateModeCounts
	{
		demoStateMaxPipelineModes = 2,
		demoStateMaxPassModes = 4,
		demoStateMaxOutputModes = 4,
	};


//-----------------------------------------------------------------------------

	// persistent demo state data structure
	struct a3_DemoState
	{
		//---------------------------------------------------------------------
		// general variables pertinent to the state

		// terminate key pressed
		int exitFlag;

		// global vertical axis: Z = 0, Y = 1
		int verticalAxis;

		// asset streaming between loads enabled (careful!)
		int streaming;

		// window and full-frame dimensions
		unsigned int windowWidth, windowHeight;
		unsigned int frameWidth, frameHeight;
		int frameBorder;


		//---------------------------------------------------------------------
		// objects that have known or fixed instance count in the whole demo

		// text renderer
		int textInit, showText;
		a3_TextRenderer text[1];

		// input
		a3_MouseInput mouse[1];
		a3_KeyboardInput keyboard[1];
		a3_XboxControllerInput xcontrol[4];

		// pointer to fast trig table
		float trigTable[4096 * 4];


		//---------------------------------------------------------------------
		// scene variables and objects

		// demo mode array: 
		//	- pipeline (2): which pipeline is being viewed
		//	- pass/mode (3,4): which pass/sub-mode in the pipeline is being viewed
		//	- output (4,4,4; 1,1,1,1): which output from the pass/sub-mode is being viewed
		unsigned int demoPipelineMode, demoPassMode[demoStateMaxPipelineModes], demoOutputMode[demoStateMaxPipelineModes][demoStateMaxPassModes];
		unsigned int demoPipelineCount, demoPassCount[demoStateMaxPipelineModes], demoOutputCount[demoStateMaxPipelineModes][demoStateMaxPassModes];

		// toggle grid in scene and axes superimposed
		int displayGrid, displayAxes, displaySkybox;

		// grid properties
		a3mat4 gridTransform;
		a3vec4 gridColor;


		// texture atlas transforms: transform to sub-texture within atlas
		a3mat4 atlasEarth, atlasMars, atlasMoon, atlasMarble, atlasCopper, atlasStone, atlasChecker;


		// lights
		a3_DemoPointLight pointLight[demoStateMaxCount_light];
		a3mat4 pointLightMVP[demoStateMaxCount_light];
		unsigned int lightCount;


		// waypoints and handles
		a3vec4 waypoint[demoStateMaxCount_waypoint];
		a3vec4 waypointHandle[demoStateMaxCount_waypoint];
		unsigned int waypointCount;

		// path following
		a3vec4 pathFollowerPosition;
		a3real pathFollowerSize;
		a3real pathSegmentDuration, pathSegmentTime, pathSegmentParam;
		unsigned int pathWaypoint0, pathWaypoint1, pathWaypoint2, pathWaypointPrev;

		// dummy drawable for rendering path and point objects
		a3_VertexDrawable dummyDrawable[1];


		// scene viewing modes
		int displayTangentBasis;


		//---------------------------------------------------------------------
		// object arrays: organized as anonymous unions for two reasons: 
		//	1. easy to manage entire sets of the same type of object using the 
		//		array component
		//	2. at the same time, variables are named pointers

		// scene objects
		union {
			a3_DemoSceneObject sceneObject[demoStateMaxCount_sceneObject];
			struct {
				a3_DemoSceneObject
					cameraObject[2],					// transform for cameras
					skyboxObject[1],					// transform for skybox (based on camera)
					groundObject[1],					// ground
					sphereObject[1],					// sphere
					cylinderObject[1],					// cylinder
					torusObject[1],						// torus
					teapotObject[1];					// teapot
			};
		};

		// cameras
		//	- any object can have a camera "component"
		union {
			a3_DemoCamera camera[demoStateMaxCount_camera];
			struct {
				a3_DemoCamera
					sceneCamera[1],						// scene viewing camera
					editCamera[1];						// top-down editing camera
			};
		};

		// timers
		union {
			a3_Timer timer[demoStateMaxCount_timer];
			struct {
				a3_Timer
					renderTimer[1];						// render FPS timer
			};
		};


		// framebuffers
		union {
			a3_Framebuffer framebuffer[demoStateMaxCount_framebuffer];
			struct {
				a3_Framebuffer
					fbo_window_scene[1],				// framebuffer for scene
					fbo_window_nodepth[1];				// framebuffer for compositing
			};
		};


		// textures
		union {
			a3_Texture texture[demoStateMaxCount_texture];
			struct {
				a3_Texture
					tex_checker[1],						// checkered texture
					tex_sky_clouds[1],					// skybox with clouds
					tex_sky_water[1],					// skybox with water
					tex_stone_dm[1],					// stone diffuse texture
					tex_earth_dm[1],					// earth diffuse texture
					tex_earth_sm[1],					// earth specular texture

					// atlases
					tex_atlas_scene_dm[1],				// diffuse map atlas for scene objects
					tex_atlas_scene_sm[1],				// specular map atlas for scene objects
					tex_atlas_scene_nm[1],				// normal map atlas for scene objects
					tex_atlas_scene_hm[1];				// height map atlas for scene objects
			};
		};


		// draw data buffers
		union {
			a3_VertexBuffer drawDataBuffer[demoStateMaxCount_drawDataBuffer];
			struct {
				a3_VertexBuffer
					vbo_staticSceneObjectDrawBuffer[1];			// buffer to hold all data for static scene objects (e.g. grid)
			};
		};

		// vertex array objects
		union {
			a3_VertexArrayDescriptor vertexArray[demoStateMaxCount_vertexArray];
			struct {
				a3_VertexArrayDescriptor
					vao_position[1],							// VAO for vertex format with only position
					vao_position_color[1],						// VAO for vertex format with position and color
					vao_position_texcoord[1],					// VAO for vertex format with position and UVs
					vao_tangent_basis[1];						// VAO for vertex format with full tangent basis
			};
		};

		// drawables
		union {
			a3_VertexDrawable drawable[demoStateMaxCount_drawable];
			struct {
				a3_VertexDrawable
					draw_grid[1],								// wireframe ground plane to emphasize scaling
					draw_axes[1],								// coordinate axes at the center of the world
					draw_fsq[1],								// full-screen quad
					draw_skybox[1],								// skybox cube mesh
					draw_groundPlane[1],						// tessellated ground plane mesh
					draw_sphere[1],								// high-res sphere mesh
					draw_cylinder[1],							// high-res cylinder mesh
					draw_torus[1],								// high-res torus mesh
					draw_sphere_lr[1],							// low-res sphere mesh
					draw_teapot[1];								// can't not have a Utah teapot
			};
		};


		// uniform buffers
		union {
			a3_UniformBuffer uniformBuffer[demoStateMaxCount_uniformBuffer];
			struct {
				a3_UniformBuffer
					ubo_waypointBuffer[1],						// buffer to send waypoints for curve drawing
					ubo_waypointHandleBuffer[1],				// buffer to send waypoint handles for curve drawing
					ubo_lightUniformBuffer[1];					// buffer to send lighting data to shaders
			};
		};

		// shader programs and uniforms
		union {
			a3_DemoStateShaderProgram shaderProgram[demoStateMaxCount_shaderProgram];
			struct {
				a3_DemoStateShaderProgram
					prog_drawCurveSegment[1],					// draw curve segment using GS, pass color
					prog_drawLineSegment[1],					// draw line segment using GS, uniform color
					prog_drawParticles[1],						// draw particle list using GS, uniform color
					prog_drawTangentBasis[1],					// draw tangent basis for each vertex using GS, pass color
					prog_drawPhong_multilight_mrt_exploded[1],	// draw phong with multiple lights, exploded view using GS
					prog_drawPhong_multilight_mrt_inverted[1],	// draw Phong with multiple lights, invert faces using GS

					prog_drawPhong_multilight_mrt[1],			// draw Phong with multiple lights
					prog_drawTexture[1],						// draw sample from texture

					prog_drawColor[1],							// draw color attribute
					prog_drawColorUnif[1];						// draw uniform color

			};
		};


		//---------------------------------------------------------------------
	};

	
//-----------------------------------------------------------------------------

	// demo-related functions

	// other utilities
	void a3demo_setDefaultGraphicsState();

	// loading and unloading
	void a3demo_loadFramebuffers(a3_DemoState *demoState);
	void a3demo_loadTextures(a3_DemoState *demoState);
	void a3demo_loadGeometry(a3_DemoState *demoState);
	void a3demo_loadShaders(a3_DemoState *demoState);

	void a3demo_unloadFramebuffers(a3_DemoState *demoState);
	void a3demo_unloadTextures(a3_DemoState *demoState);
	void a3demo_unloadGeometry(a3_DemoState *demoState);
	void a3demo_unloadShaders(a3_DemoState *demoState);

	void a3demo_initScene(a3_DemoState *demoState);

	void a3demo_refresh(a3_DemoState *demoState);

	void a3demo_validateUnload(const a3_DemoState *demoState);

	// main loop
	void a3demo_input(a3_DemoState *demoState, double dt);
	void a3demo_update(a3_DemoState *demoState, double dt);
	void a3demo_render(const a3_DemoState *demoState);


//-----------------------------------------------------------------------------


#ifdef __cplusplus
}
#endif	// __cplusplus


#endif	// !__ANIMAL3D_DEMOSTATE_H