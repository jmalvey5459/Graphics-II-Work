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
	
	drawTriangle_inverted_passAttribs_gs4x.glsl
	Receive inputs from vertex shader, draw in reverse order to invert faces. 
		Pass respective data along to fragment shader.
*/

#version 410

// ****TO-DO: 
//	1) layout qualifiers: input and output primitive type
//	2) declare input and output data (same as vertex and fragment shader)
//	3) implement function to copy vertex
//	4) implement function to copy triangle
//	5) implement function to invert triangle
//	6) implement function to invert triangle with proper lighting data
//	7) test functions in main

//(1)
layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

//(2)
in vbShadingData
{
	vec4 vPosition_eye;
	vec4 vTangent_eye;
	vec4 vBitangent_eye;
	vec4 vNormal_eye;
	vec4 vTexcoord_atlas;

} vPassData_in[];

out vbShadingData
{
	vec4 vPosition_eye;
	vec4 vTangent_eye;
	vec4 vBitangent_eye;
	vec4 vNormal_eye;
	vec4 vTexcoord_atlas;

} vPassData_out;

//(3)
void copyVertex(const int i)
{
	vPassData_out.vPosition_eye = vPassData_in[i].vPosition_eye;
	vPassData_out.vTangent_eye= vPassData_in[i].vTangent_eye;
	vPassData_out.vBitangent_eye= vPassData_in[i].vBitangent_eye;
	vPassData_out.vNormal_eye= vPassData_in[i].vNormal_eye;
	vPassData_out.vTexcoord_atlas= vPassData_in[i].vTexcoord_atlas;
}

//(4)
void copyTriangle()
{
	//vPassData_in.length();
	for(int i=0; i < 3;++i)
	{
		//vPassData_out = vPassData_in[i]; //Only works on some things
		copyVertex(i);
		gl_Position = gl_in[i].gl_Position;

		EmitVertex();
	}
}

//(5)
void invertTriangle()
{
	for(int i=2; i >= 0;--i)
	{
		//vPassData_out = vPassData_in[i]; //Only works on some things
		copyVertex(i);
		gl_Position = gl_in[i].gl_Position;

		EmitVertex();
	}
}

void invertTriangleLighting()
{
	for(int i=2; i >= 0;--i)
	{
		//vPassData_out = vPassData_in[i]; //Only works on some things
		copyVertex(i);
		vPassData_out.vNormal_eye *= -1.0;
		gl_Position = gl_in[i].gl_Position;

		EmitVertex();
	}
}


void main()
{
invertTriangleLighting();

EndPrimitive();
}
