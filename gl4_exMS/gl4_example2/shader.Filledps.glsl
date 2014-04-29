#version 400

in vec4 vColor;
in vec3 vTEX;
flat in int isBp;

out vec4 out_Color;

uniform int displacement;

void main(void)
{
    vec4 theColor = vColor;

	// switch color here based on displacement
	if( displacement != 0)
	{
	    theColor = vec4(0,1,0,0);
	}
	else
	{
	    theColor = vColor;
	}

	out_Color = theColor;
}