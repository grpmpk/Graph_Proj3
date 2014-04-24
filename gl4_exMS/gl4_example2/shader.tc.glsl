// TessControl Shader

#version 400

layout(vertices = 4) out;
in vec4 vPosition[];
in vec4 vColor[];
out vec3 tcPosition[];
out vec4 tcColor[];
uniform float TessLevelInner;
uniform float TessLevelOuter;

#define ID gl_InvocationID

void main()
{
    tcPosition[ID] = vPosition[ID].xyz;
    tcColor[ID] = vColor[ID];
    if (ID == 0) {
        gl_TessLevelInner[0] = TessLevelInner;
        gl_TessLevelOuter[0] = TessLevelOuter;
        gl_TessLevelOuter[1] = TessLevelOuter;
        gl_TessLevelOuter[2] = TessLevelOuter;
    }
}

