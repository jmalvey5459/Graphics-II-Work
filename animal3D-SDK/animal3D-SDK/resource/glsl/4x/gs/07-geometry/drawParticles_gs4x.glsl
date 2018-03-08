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
	
	drawParticles_gs4x.glsl
	Draw a list of points as billboard squares.
*/

#version 410

// ****TO-DO: 
//	1) layout qualifiers
//	2) declare uniform block for particles
//	3) declare other uniforms (matrices, counts)
//	4) implement function to draw quad at a point
//	5) test function in main

#define MAX_PARTICLES 32
#define MAX_VERTICES 128

//(1)
layout(points) in;
layout(triangle_strip, max_vertices = MAX_VERTICES) out;

//(2)
uniform ubParticle
{
	vec4 particle[MAX_PARTICLES];
};

//(3)
uniform mat4 uMV;
uniform mat4 uP;
uniform int uParticleCount;

//(4)
void drawQuadAtPoint(vec3 p, const float size)
{
	//draw triangle strip centered at p
	// p2				 p3
	// x-----------------x
	//	\
	//		\
	//			\
	//				\
	//					\
	// x-----------------x
	// p0				 p1

	vec4 p_eye = uMV * vec4(p, 1.0);
	vec4 offset = vec4(0.0);
	float sz_half = size * 0.5;

	offset.x = -sz_half; //p0
	offset.y =-sz_half;
	gl_Position = uP*(p_eye + offset);
	EmitVertex();

	offset.x = sz_half; //p1
	offset.y =-sz_half;
	gl_Position = uP*(p_eye + offset);
	EmitVertex();

	offset.x = -sz_half; //p2
	offset.y =sz_half;
	gl_Position = uP*(p_eye + offset);
	EmitVertex();

	offset.x = sz_half; //p3
	offset.y = sz_half;
	gl_Position = uP*(p_eye + offset);
	EmitVertex();

	EndPrimitive();
};


void main()
{
	for(int i= 0; i < uParticleCount; ++i)
		drawQuadAtPoint(particle[i].xyz, 10.0);
}
