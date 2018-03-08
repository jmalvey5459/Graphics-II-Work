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
	
	drawCurveSegment_passColor_gs4x.glsl
	Draw a curve segment between waypoints. Pass solid color.
*/

#version 410

// ****TO-DO: 
//	1) layout qualifiers
//	2) declare input from vertex shader
//	3) declare output data (same as fragment shader)
//	4) declare uniform blocks for input data
//	5) declare other uniforms (matrices, path info, etc.)
//	6) implement interpolation algorithms
//	7) implement path segment sampling and drawing algorithms
//	8) test in main

#define MAX_WAYPOINT 32
#define MAX_VERTICES 128

//(1)
layout(points) in;
layout(line_strip, max_vertices = MAX_VERTICES) out;

//(2)
in int vPassInstanceID[]; 

//(3)
out vec4 vPassColor;

//(4)
uniform ubWaypoint
{
	vec4 waypoints[MAX_WAYPOINT];
};

uniform ubWaypointHandle
{
	vec4 waypointHandles[MAX_WAYPOINT];
};

//(5)
uniform mat4 uMVP;
uniform int uPathMode;
uniform int uPathWaypointCount;

// (6)
//Lerp
vec4 lerp(in vec4 p0, in vec4 p1, const float t)
{
//lerp formula from slide 31
	return vec4((1 -t)*p0 + (t*p1));
}
//BiLerp
/*
vec4 biLerp(in vec4 p0,in vec4 p1,in vec4 p2,in vec4 p3,const float t0, const float t1)
{
//bilerp formula from slide 35
	return vec4((1 -t1) * lerp(p0,p1,t0) + (t1 * lerp(p2,p3,t0), t1));
}*/
// Bezier base
vec4 bezier0(in vec4 p0, const float t)
{
//bezier 0 "formula" from slide 58
	return p0;
}

// Bezier first order (haha star wars) 
vec4 bezier1(in vec4 p0, in vec4 p1, const float t)
{
// bezier 1 formula from slide 60
	return lerp(p0,p1,t);
}

// Bezier 2nd order 
vec4 bezier2(in vec4 p0, in vec4 p1, in vec4 p2, const float t)
{
//bezier 2 forumla from slide 79
	vec4 bez2 = vec4 ((1 -t)*(1 -t)*p0 + 2 *(1 -t)*t*p1 + t*t*p2);

	return bez2;
}

// Bezier 3rd order
vec4 bezier3(in vec4 p0, in vec4 p1, in vec4 p2, in vec4 p3, const float t)
{
//bezier 3 forumla from slide 79
	vec4 bez3 = 
	// (1-t)^3 * p0
	vec4(((1 -t)*(1 -t)*(1 -t))*p0 +
	// 3(1-t)^2 * t * p1
	3 *((1 -t)*(1 -t))*t*p1 +
	//3*(1-t) * t^2 * p2
	3  * (1 -t) * (t*t) *p2 +
	//t^3 * p3
	(t*t*t)*p3);
	return bez3;
}

// Bezier (infinitely scalable)
vec4 bezierN(in vec4 p[MAX_WAYPOINT], const int order, const float t)
{
vec4 passBack;
float t1expo = (1  - t);
float texpo = t;
//bezier pure formula from slide 80 (kind of?)
	for(int i = 0; i < order; i++)
	{
		for(int j = 0; j < order - i; j++)
		{
			t1expo *= (1  - t);
		}
		for(int y = 0; y < i; y++)
		{
			texpo *= t;
		}
		passBack += (t1expo * texpo * p[i]);
	}
	return passBack;
}

// Catmull-Rom
vec4 catmullRom(in vec4 p, in vec4 p0, in vec4 p1, in vec4 p2, const float t)
{
/* 
	matrix  for catmull-rom  from slide 92
	0.0,	2.0,	0.0,	0.0, 
	-1.0,	0.0,	1.0,	0.0,
	2.0,	-5.0,	4.0,	-1.0,
	-1.0,	3.0,	-3.0,	1.0);
*/
// catmull forumla from slide 94
	float t2 = t*t;
	float t3 = t*t*t;
	vec4 cat = 0.5f * ((-t + (2 *t2) - t3)*p + (2 -5 *t2 + 3 *t3)*p0 + (t+4 *t2 - 3  * t3)*p1 + (-t2 + t3)*p2);
	return cat;
}

// Cubic Hermite 
vec4 cubicHermite(in vec4 p0, in vec4 m0, in vec4 p1, in vec4 m1, const float t)
{
//cspline forumlas from slide 112
	float h00 = 1  - 3 *(t*t) + 2 *(t*t*t);
	float h10 = t - 2 * (t*t) + (t*t*t);
	float h01 = 3  * (t*t) - 2  * (t*t*t);
	float h11 = -1  * (t*t) + (t*t*t);

	vec4 cspline = (h00 * p0 + h10 * m0 + h01 * p1 + h11 * m1);
	return cspline;
}

// (7)
void drawLerp(in vec4 p0, in vec4 p1, const int n, const float dt)
{
	vec4 l;
	float t;
	int i;
	for(i=0,t=0.0; i<n; ++i, t+=dt)
	{
		l = lerp(p0,p1,t);
		gl_Position = uMVP *l;
		EmitVertex();
	}
	
	EndPrimitive();
}/*
void drawBiLerp(in vec4 p0,in vec4 p1,in vec4 p2,in vec4 p3,const float t0, const float t1)
{
	vec4 biLerp;
	float t;
	int i;
	for(i=0,t=0.0; i<n; ++i, t+=dt)
	{
		biLerp = biLerp(p0,p1,p2,p3,t0,t1);
		gl_Position = uMVP *biLerp;
		EmitVertex();
	}
	
	EndPrimitive();
}*/
void drawBezier3(in vec4 p0, in vec4 p1, in vec4 p2, in vec4 p3, const int n, const float dt)
{
	vec4 bez;
	float t;
	int i;
	for(i=0,t=0.0; i<n; ++i, t+=dt)
	{
		bez = bezier3(p0,p1,p2,p3,t);
		gl_Position = uMVP *bez;
		EmitVertex();
	}
	
	EndPrimitive();
}

void drawCatmullRom(in vec4 p, in vec4 p0, in vec4 p1, in vec4 p2, const int n, const float dt)
{
	float t;
	int i;
	vec4 cat;
	for(i=0,t=0.0; i<n; ++i, t+=dt)
	{
		cat = catmullRom(p,p0,p1,p2,t);
		gl_Position = uMVP *cat;
		EmitVertex();

	}

	EndPrimitive();
}

void drawCubicHermite(in vec4 p0, in vec4 m0,  in vec4 p1, in vec4 m1, const int n, const float dt)
{
	vec4 H;	
	float t;
	int i;
	for(i=0,t=0.0; i<n; ++i, t+=dt)
	{
		H = cubicHermite(p0,m0,p1,m1,t);
		gl_Position = uMVP * H; 
		EmitVertex();
	}
	EndPrimitive();
}

void main()
{
	int w0 = vPassInstanceID[0];
	int w1 = w0 < uPathWaypointCount ? w0+1 : w0;

	int n = MAX_VERTICES;
	float dt = 1.0/float(n);
	vPassColor = vec4(0.0, 0.5, 1.0, 1.0);

	// (8) 
	switch (uPathMode)
	{
		case 0:
			drawLerp(waypoints[w0], waypoints[w1], n, dt);
			break;
		case 1:
			drawCatmullRom(waypoints[w0>0 ? w0-1:w0],waypoints[w0],waypoints[w0 < uPathWaypointCount-1?w0+1:w0],waypoints[w1 < uPathWaypointCount - 1?w1+1:w1],n,dt);
			break;
		case 2:
			drawBezier3(waypoints[w0],waypointHandles[w0],waypointHandles[w1],waypoints[w1],n,dt);
			break;
		case 3: 
			vec4 m0 = waypointHandles[w0] - waypoints[w0];
			vec4 m1 = waypointHandles[w1] -  waypoints[w1];
			drawCubicHermite(waypoints[w0],m0,waypoints[w1],m1,n,dt);
			break;
	};
}
