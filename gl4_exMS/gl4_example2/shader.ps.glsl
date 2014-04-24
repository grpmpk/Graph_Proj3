#version 400

in vec4 teColor;
in float k;
in float l;
in float m;

out vec4 out_Color;

void main(void)
{
	out_Color = teColor;
//	out_Color = vec4(1,1,1,1);
}
