#version 400

in vec4 teColor;
//in float k;
//in float l;
//in float m;

float k;
float l;
float m;

uniform mat4 klmMatrix;

out vec4 out_Color;

float myDeCasteljau(float v0, float v1, float v2, float v3){
	float v10 = (1/2 * v0) + (1/2 * v1);
	float v11 = (1/2 * v1) + (1/2 * v2);
	float v12 = (1/2 * v2) + (1/2 * v3);


	float v20 = (1/2 * v10) + (1/2 * v11);
	float v21 = (1/2 * v11) + (1/2 * v12);

	return ((1/2 * v20) + (1/2 * v21));
}

void detKLM(){
	k = myDeCasteljau(klmMatrix[0].x, klmMatrix[1].x, klmMatrix[2].x, klmMatrix[3].x);
	l = myDeCasteljau(klmMatrix[0].y, klmMatrix[1].y, klmMatrix[2].y, klmMatrix[3].y);
	m = myDeCasteljau(klmMatrix[0].z, klmMatrix[1].z, klmMatrix[2].z, klmMatrix[3].z);
}

void main(void)
{
	detKLM();
	out_Color = teColor;
//	out_Color = vec4(1,1,1,1);
}
