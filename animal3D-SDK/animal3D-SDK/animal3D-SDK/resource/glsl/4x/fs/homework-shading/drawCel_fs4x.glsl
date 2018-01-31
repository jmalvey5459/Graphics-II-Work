
#version 410


uniform sampler2D uTex_dm;
uniform sampler2D uTexToon_dm;
uniform sampler2D uTexToon_sm;


in vec2 vPhongPassTexcoord;
in vec3 vPhongPassNormal;
in vec3 vPhongPassLighting;
in vec3 vPhongPassView;

out vec4 rtFragColor;
void main()
{
	//diffuse
	vec3 L = normalize(vPhongPassLighting);
	vec3 N = normalize(vPhongPassNormal);
	float diffuse = dot(N,L);

	//reflection
	vec3 R = (diffuse + diffuse) * N - L;

	diffuse = max(0.0, diffuse);
	
	vec3 ambient = vec3(0,0,0.1);
	
	vec3 phong = diffuse + ambient;
	vec2 ramp = vec2(0.0f);
		ramp.x = (diffuse*0.5+0.5);

	//all together now!
	vec4 toon = texture(uTexToon_dm,vPhongPassTexcoord);
	vec4 earth = texture(uTex_dm, vPhongPassTexcoord);
	toon *= diffuse;
	rtFragColor = (earth + toon);
//rtFragColor = vec4(0.0, 0.0, 1.0, 1.0);
}