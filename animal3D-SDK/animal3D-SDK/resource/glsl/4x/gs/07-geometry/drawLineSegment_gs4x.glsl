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
	
	drawLineSegment_gs4x.glsl
	Draw a line segment between corresponding points from two sets of points.
*/

#version 410

// ****TO-DO: 
//	1) layout qualifiers
//	2) declare input from vertex shader
//	3) declare uniform blocks for input data
//	4) declare other uniforms (matrices, etc.)
//	5) implement function to draw line between waypoint and handle
//		- additionally, draw the "negative" handle line
//	6) test in main 

#define MAX_WAYPOINT 32
#define MAX_VERTICES 3

//(1)
layout(points) in;
layout(line_strip, max_vertices = MAX_VERTICES) out;

//(2)
in int vPassInstanceID[]; 

//(3)
uniform ubWaypoint
{
	vec4 waypoint[MAX_WAYPOINT];
};

uniform ubWaypointHandle
{
	vec4 waypointHandle[MAX_WAYPOINT];
};

//(3)
uniform mat4 uMVP;


//(4)
void drawLine(vec4 p, vec4 handle)
{
	vec4 point_clip = uMVP * p;
	vec4 handle_clip = uMVP * handle;
	vec4 extention_clip = uMVP * (p - (handle - p));
	gl_Position = point_clip;
	EmitVertex();
	gl_Position = handle_clip;
	EmitVertex();
	gl_Position = extention_clip;
	EmitVertex();

	EndPrimitive();
};


void main()
{
	int i= vPassInstanceID[0];
	drawLine(waypoint[i],waypointHandle[i]);
	
}
