#version 400



layout(location=0) in vec4 in_Position;
layout(location=1) in vec4 in_Color;
out vec4 vColor;
out vec4 vPosition;

void main(void)
{
	vPosition = in_Position;  	
	gl_Position = vPosition;
	vColor = in_Color;
}
