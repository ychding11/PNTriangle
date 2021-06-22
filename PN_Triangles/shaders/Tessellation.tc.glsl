#version 440 core

struct V2T
{
    vec3 position;
    vec3 normal;
    vec4 color;
};

struct TC2E {
    vec3 position;
    vec3 normal;
    vec4 color;
};

layout (vertices = 3) out;
uniform float tessellationLevelInner;
uniform float tessellationLevelOuter;

in  V2T vdata[];
out TC2E tcdata[];

//< 
//< Output from Hull Shader(Name in D3D11): 
//< 1. Compute tessellate factor( edge tessellation level, inner tessellation level) 
//< 2. Transformed "Control Point", like what VS does.
//< 
void main()
{
    #define ID gl_InvocationID //< once per vertex(CP)
    tcdata[ID].position = vdata[ID].position;
    tcdata[ID].normal = vdata[ID].normal;
    tcdata[ID].color = vdata[ID].color;

    if(ID == 0)
    {
	    gl_TessLevelInner[0] = tessellationLevelInner;
	    gl_TessLevelOuter[0] = tessellationLevelOuter;
	    gl_TessLevelOuter[1] = tessellationLevelOuter;
	    gl_TessLevelOuter[2] = tessellationLevelOuter;
    }
    #undef ID
}

//< Where is "Patch Constant Function" ?
//< which is called per patch to calculate constant values valid for full patch.
