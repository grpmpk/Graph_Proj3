#version 400

in vec4 vColor;
in vec3 vTEX;

out vec4 out_Color;

void main(void)
{
    // TODO: comparison needs to be reversed between concave vs convex (i.e., red vs blue)
	if (vTEX.x*vTEX.x - vTEX.y >= 0)
	{
	    discard;
	}
	else
	{
	    out_Color = vColor;
	}
}
