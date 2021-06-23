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

//< output patch size is the number of vertices in the output patch
layout (vertices = 3) out;
uniform float tessellationLevelInner;
uniform float tessellationLevelOuter;

in  V2T vdata[];

//< utput variables are passed directly to the Tessellation Evaluation Shader, without any form of interpolation.
//< array size is implicit
out TC2E tcdata[];

//< 
//< Output from Hull Shader(Name in D3D11): 
//< 1. Compute tessellate factor( edge tessellation level, inner tessellation level) 
//< 2. Transformed "Control Point", like what VS does.
//<
//< patch out float gl_TessLevelOuter[4];
//< patch out float gl_TessLevelInner[2];
//<
//< built-in input variables:
//< gl_PatchVerticesIn : the number of vertices in the input patch.
//< gl_PrimitiveID     : the index of the current patch within this rendering command.
//< gl_InvocationID    : the index of the TCS invocation within this patch. A TCS invocation writes to per-vertex output variables by using this to index them.
//<
//< uilt-in variables from output of vertex shader
/*
    in gl_PerVertex
    {
      vec4 gl_Position;
      float gl_PointSize;
      float gl_ClipDistance[];
    } gl_in[gl_MaxPatchVertices];
*/
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
