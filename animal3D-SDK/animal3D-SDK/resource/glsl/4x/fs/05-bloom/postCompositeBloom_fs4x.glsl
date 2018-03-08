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
	
	postCompositeBloom_fs4x.glsl
	Composites images to produce the HDR bloom effect.
*/

#version 410

// ****TO-DO: 
//	1) declare uniforms for images to composite
//	2) declare uniform for HDR range (if using)
//	3) define and test simple function to composite the images
//		e.g. screen, additive
//	4) define and test HDR function to composite the tone-mapped images
//		(uses range)

in vec2 vPassTexcoord;

uniform sampler2D uTex_dm;

out vec4 rtFragColor;

void main()
{
	vec4 texSample = texture(uTex_dm, vPassTexcoord);
	rtFragColor = texSample;
}
