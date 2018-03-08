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
	
	drawTriangle_exploded_passAttribs_gs4x.glsl
	Receive data from vertex shader, move geometry along face normal to draw 
		"exploded" view. Pass data along to fragment shader.
*/

#version 410

// ****TO-DO: 
//	1) layout qualifiers: input and output primitive type
//	2) declare input and output data (same as vertex and fragment shader)
//	3) declare transformation matrices
//	4) implement function to copy vertex
//	5) implement function to calculate face normal
//	6) implement function to explode triangle
//	7) use function in main



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
uniform mat4 uP;

//(4)
void copyVertex(const int i)
{
	vPassData_out.vPosition_eye = vPassData_in[i].vPosition_eye;
	vPassData_out.vTangent_eye= vPassData_in[i].vTangent_eye;
	vPassData_out.vBitangent_eye= vPassData_in[i].vBitangent_eye;
	vPassData_out.vNormal_eye= vPassData_in[i].vNormal_eye;
	vPassData_out.vTexcoord_atlas= vPassData_in[i].vTexcoord_atlas;
}

//(5)
vec4 calcFaceNormal()
{
	vec3 p0 = vPassData_in[0].vPosition_eye.xyz;
	vec3 p1 = vPassData_in[1].vPosition_eye.xyz;
	vec3 p2 = vPassData_in[2].vPosition_eye.xyz;

	vec3 dp1 = p1 -p0;
	vec3 dp2 = p2 -p0;
	vec3 n = normalize(cross(dp1, dp2));
	return vec4(n,0.0);

}

//(5)
void explodeTriangle()
{
	const float dist =0.2;
	vec4 n = calcFaceNormal() * dist;
	vec4 p;
		
	//vPassData_in.length();
	for(int i=0; i < 3;++i)
	{
		//vPassData_out = vPassData_in[i]; //Only works on some things
		copyVertex(i);
		p = vPassData_out.vPosition_eye +n;
		gl_Position = uP * p;

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
explodeTriangle();

EndPrimitive();
}
