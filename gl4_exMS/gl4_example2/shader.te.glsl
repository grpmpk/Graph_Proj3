// Tessellation Evaluation Shader
#version 400

//layout(triangles, equal_spacing, cw) in;
layout(isolines) in;

//layout(isolines) in;
in vec3 tcPosition[];
in vec4 tcColor[];

out vec3 tePosition;
out vec4 teColor;

// using decasteljau
//out float k;
//out float l;
//out float m;

uniform mat4 ModelMatrix;
uniform mat4 ViewMatrix;
uniform mat4 ProjectionMatrix;



uniform int displacement;

void main()
{
	float u = gl_TessCoord.x; 
	float v = gl_TessCoord.y; 
	float w = gl_TessCoord.z; 

    vec3 p0 = u * tcPosition[0];
    vec3 p1 = v * tcPosition[1];
    vec3 p2 = w * tcPosition[2];
    
    teColor = u*tcColor[0] + v*tcColor[1] + w*tcColor[2];
    tePosition = p0 + p1 + p2;
    if( displacement != 0)
        tePosition = normalize(tePosition);
	gl_Position = ProjectionMatrix * ViewMatrix * ModelMatrix * vec4(tePosition, 1.0f);  	
}

