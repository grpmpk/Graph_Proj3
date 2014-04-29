#version 400

layout(location=0) in vec4 in_Position;
layout(location=1) in vec4 in_Color;
layout(location=2) in vec3 in_TEX;  // TODO: calculate this for KLM
layout(location=3) in int isRv;

out vec4 vColor;
out vec3 vTEX;    // TODO: calculate this for KLM
flat out int isRp;

uniform mat4 ModelMatrix;
uniform mat4 ViewMatrix;
uniform mat4 ProjectionMatrix;

void main(void)
{
	vColor = in_Color;
	isRp = isRv;
	vTEX = in_TEX;  // TODO: calculate this for KLM
	gl_Position = ProjectionMatrix * ViewMatrix * ModelMatrix * in_Position;
}
