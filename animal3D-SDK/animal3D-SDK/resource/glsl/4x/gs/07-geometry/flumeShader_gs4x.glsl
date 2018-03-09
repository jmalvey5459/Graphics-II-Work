//HUGELY helpful presentation on subdivison and "sandy" effect
//http://web.engr.oregonstate.edu/~mjb/cs519/Handouts/geometry_shaders.1pp.pdf
/*Certificate of Authenticity 
We certify that this work is entirely our own. The assessor of this project may reproduce this project
and provide copies to other academic staff, and/or communicate a copy of
this project to a plagiarism-checking service, which may retain a copy of the
project on its database.*/
#version 410
layout (triangles) in;
layout (points, max_vertices = 42) out;


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


uniform mat4 uP;
uniform mat4 uMVP;
//for sale: time uniform- never used :(
uniform float uTime;

void copyVertex(const int i)
{
	vPassData_out.vPosition_eye = vPassData_in[i].vPosition_eye;
	vPassData_out.vTangent_eye= vPassData_in[i].vTangent_eye;
	vPassData_out.vBitangent_eye= vPassData_in[i].vBitangent_eye;
	vPassData_out.vNormal_eye= vPassData_in[i].vNormal_eye;
	vPassData_out.vTexcoord_atlas= vPassData_in[i].vTexcoord_atlas;
}
vec3 v0, v1, v2;
vec3 CG;
float dt = 0.25 ;
vec3 dist;

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

void ProduceVertex( float s, float t )
{
	vec3 v = v0 + s*v1 + t*v2;
	vec3 vel = 2* ( v - CG );
	CG = ( v0 + v1 + v2 ) / 3.0;
	v = CG + vel*dt + 0.5*vec3(0.0,-9.81f,0.0)* dt *dt;
	dist = v;
	v = normalize(v);
	vec4 position = uMVP * vec4( (4*v), 1 );
	gl_Position = uP * position;
	EmitVertex();
}


void subDivide()
{	
	//get triangle vertices
	v1 = ( gl_in[1].gl_Position - gl_in[0].gl_Position ).xyz;
	v2 = ( gl_in[2].gl_Position - gl_in[0].gl_Position ).xyz;
	v0 = gl_in[0].gl_Position.xyz;
	//decide number of layers, (2^"level")
	int numLayers = 4;
	
	///directly from presentation linked at the top, page 26
	float t_top = 1.0;
	for( int it = 0; it < numLayers; it++ )
	{
		float t_bot = t_top - dt;
		float smax_top = 1. - t_top;
		float smax_bot = 1. - t_bot;
		int nums = it + 1;
		float ds_top = smax_top / float( nums - 1 );
		float ds_bot = smax_bot / float( nums );
		float s_top = 0.;
		float s_bot = 0.;
		for( int is = 0; is < nums; is++ )
		{
			ProduceVertex( s_bot, t_bot );
			ProduceVertex( s_top, t_top );
			s_top += ds_top;
			s_bot += ds_bot;
		}
		ProduceVertex( s_bot, t_bot );
		EndPrimitive();
		t_top = t_bot;
		t_bot -= dt;
	}
}
vec4 bezier2(in vec4 p0, in vec4 p1, in vec4 p2, const float t)
{
	vec4 bez2 = vec4 ((1 -t)*(1 -t)*p0 + 2 *(1 -t)*t*p1 + t*t*p2);
	return bez2;
}
void explode()
{
	//copied over from triangle explode in class, with some minor alterations
	float maxDist = 4;
	vec4 endPoint = vec4(dist.x * maxDist, dist.y * maxDist ,dist.z * maxDist,1.0);

	//slerp
	vec4 bez = bezier2(vec4(CG,1),vec4(dist,1),endPoint,sin(dt));
	vec4 p;
	vec4 n = calcFaceNormal() * bez;
	for(int i=0; i < 3;++i)
	{
		copyVertex(i);
		p = vPassData_out.vPosition_eye + n;
		gl_Position = uP * p;
		EmitVertex();
	}
}
void main()
{
	//cut triangles into smaller triangles and emit the resulting vertices
	subDivide();
	//explode them out from their original location
	explode();
	//done
	EndPrimitive();
}