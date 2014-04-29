#version 400

in vec4 vColor;
in vec3 vTEX;
flat in int isBp;

out vec4 out_Color;

void main(void)
{
	 out_Color = vColor;
}