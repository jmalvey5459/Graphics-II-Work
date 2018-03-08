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
	
	drawTangentBasis_passColor_gs4x.glsl
	Convert trianges to tangent bases for each vertex. Pass solid color.
*/

#version 410


// ****TO-DO: 
//	1) layout qualifiers: input and output primitive type
//	2) declare input data (same as vertex shader)
//	3) declare output data (same as fragment shader)
//	4) declare transformation matrices
//	5) implement function to draw triangle edges (wireframe)
//	6) implement general purpose function to draw axes
//	7) implement function to convert vertex to axes and draw
//	8) implement function to convert triangle to axes and draw
//	9) test functions in main

//(1)
layout (triangles) in;
layout (line_strip, max_vertices = 28) out;

//(2)
in vbShadingData
{
	vec4 vPosition_eye;
	vec4 vTangent_eye;
	vec4 vBitangent_eye;
	vec4 vNormal_eye;
	vec4 vTexcoord_atlas;

} vPassData_in[];

//(3)
out vec4 vPassColor;

//(4)
uniform mat4 uP;

//(5)
void drawTriangleEdges()
{
	vPassColor = vec4(0.5);

	gl_Position = gl_in[0].gl_Position;
	EmitVertex();
	gl_Position = gl_in[1].gl_Position;
	EmitVertex();
	gl_Position = gl_in[2].gl_Position;
	EmitVertex();
	gl_Position = gl_in[0].gl_Position;
	EmitVertex();

	EndPrimitive();
}

void main()
{
 drawTriangleEdges();	
}
