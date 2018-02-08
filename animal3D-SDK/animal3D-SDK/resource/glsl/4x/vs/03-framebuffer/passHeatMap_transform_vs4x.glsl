#version 410

layout (location = 0) in vec4 aPosition;
layout (location = 8) in vec2 aTexcoord;

uniform mat4 uMVP;

out vec2 vTexcoord;

void main()
{
	gl_Position = uMVP * aPosition;
	vTexcoord = aTexcoord;
}