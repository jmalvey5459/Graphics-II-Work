#version 410
layout (triangles) in;
layout (points, max_vertices = 40) out;


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

vec3 dist;
void ProduceVertex( float s, float t )
{
	vec3 v = v0 + s*v1 + t*v2;
	vec3 vel = 3.0 * ( v - CG );
	CG = ( v0 + v1 + v2 ) / 3.0;
	
	v = CG + vel*0.033 + 0.5*vec3(0.0,-9.81f,0.0)* 0.033 * 0.033;
	dist = vel;
	v = normalize(v);
	vec4 ECposition = uMVP * vec4( (v), 1 );
	gl_Position = uP * ECposition;
	EmitVertex();
}


void subDivide()
{	
	
	v1 = ( gl_in[1].gl_Position - gl_in[0].gl_Position ).xyz;
	v2 = ( gl_in[2].gl_Position - gl_in[0].gl_Position ).xyz;
	v0 = gl_in[0].gl_Position.xyz;
	int numLayers = 2;
	float dt = 1.0 / float( numLayers );
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
void explode()
{
	
	float maxDist = 1;
	vec4 endPoint = vec4(dist.x * maxDist, dist.y * maxDist ,dist.z * maxDist,1.0);
	//lerp!!!
	vec4 lerp = vec4((1.0 - 0.2)*vec4(dist,1) + (0.2 * endPoint));
	vec4 p;
	vec4 n = calcFaceNormal() * lerp;
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
	
	
	gl_PointSize = 2.0;
	
	subDivide();

	explode();
	EndPrimitive();
}