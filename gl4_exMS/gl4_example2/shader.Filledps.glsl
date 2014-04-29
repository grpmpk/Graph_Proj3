
#version 400

in vec4 vColor;
in vec3 vTEX;
flat in int isRp;

out vec4 out_Color;

float k;
float l;
float m;

uniform mat4 klmMatrix;


void main(void)
{
    // detKLM();

    out_Color = vColor;
}