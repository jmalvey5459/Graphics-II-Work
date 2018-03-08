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
	
	a3_DemoShaderProgram.h
	Wrapper for shaders used in demo state.
*/

#ifndef __ANIMAL3D_DEMOSHADERPROGRAM_H
#define __ANIMAL3D_DEMOSHADERPROGRAM_H


//-----------------------------------------------------------------------------
// animal3D framework includes

#include "animal3D/a3graphics/a3_ShaderProgram.h"


//-----------------------------------------------------------------------------

#ifdef __cplusplus
extern "C"
{
#else	// !__cplusplus
	typedef struct a3_DemoStateShader			a3_DemoStateShader;
	typedef struct a3_DemoStateShaderProgram	a3_DemoStateShaderProgram;
#endif	// __cplusplus


//-----------------------------------------------------------------------------

	// maximum number of uniforms in a program
	enum a3_DemoShaderProgramMaxCounts
	{
		demoStateMaxCount_shaderProgramUniform = 48,
		demoStateMaxCount_shaderProgramUniformBlock = 4,
	};


	// structure to help with shader management
	struct a3_DemoStateShader
	{
		a3_Shader shader[1];
		char shaderName[32];

		a3_ShaderType shaderType;
		unsigned int srcCount;
		const char *filePath[8];	// max number of source files per shader
	};


	// structure to help with shader program and uniform management
	struct a3_DemoStateShaderProgram
	{
		a3_ShaderProgram program[1];
		char programName[32];

		// single uniforms
		union {
			int uniformLocation[demoStateMaxCount_shaderProgramUniform];
			struct {
				int
					// common vertex shader uniforms
					uMVP,						// model-view-projection transform
					uMV,						// model-view matrix (object -> view)
					uP,							// projection matrix (view -> clip)
					uP_inv,						// projection matrix inverse (clip -> view)
					uAtlas,						// texture atlas matrix (for UVs)


					uLightPos_obj,				// light position relative to object
					uEyePos_obj,				// eye position relative to object

					// common geometry shader uniforms
					uParticleCount,				// number of particles in list
					uPathMode,					// type of path to display for curve-drawing
					uPathWaypoint0,				// segment start index index for curve-drawing (i)
					uPathWaypoint1,				// segment end index (i + 1)
					uPathWaypoint2,				// segment next index (i + 2)
					uPathWaypointPrev,			// segment prev index (i - 1)
					uPathWaypointCount,			// number of path waypoints
					uPathSegmentTime,			// current segment time
					uPathSegmentParam,			// current segment interpolation parameter

					// common fragment shader uniforms
					uImage0,					// image slot for post-processing
					uImage1,					// image slot for post-processing
					uImage2,					// image slot for post-processing
					uImage3,					// image slot for post-processing
					uImage4,					// image slot for post-processing
					uImage5,					// image slot for post-processing
					uImage6,					// image slot for post-processing
					uImage7,					// image slot for post-processing

					uLightPos_world,			// light position relative to world
					uLightPos_eye,				// light position relative to viewer
					uLightRadius,				// light radius (distance)
					uLightRadiusInvSq,			// light radius inverse squared (attenuation)
					uLightColor,				// light color (RGB)
					uLightCount,				// light count
					uLightIndex,				// light index

					uTex_dm,					// diffuse map/surface texture
					uTex_sm,					// specular map/reflective color
					uPixelSize,					// size of pixel in texture space
					uColor;						// uniform color
			};
		};

		// uniform blocks
		union {
			int uniformBlockLocation[demoStateMaxCount_shaderProgramUniformBlock];
			struct {
				int
					// common uniform blocks
					ubParticle,					// particle block
					ubWaypoint,					// waypoint block
					ubWaypointHandle,			// waypoint handle block
					ubPointLight;				// point light descriptor
			};
		};
	};


//-----------------------------------------------------------------------------


#ifdef __cplusplus
}
#endif	// __cplusplus


#endif	// !__ANIMAL3D_DEMOSHADERPROGRAM_H