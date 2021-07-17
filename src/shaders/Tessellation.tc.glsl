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
        /*
          - The interpretation of tessellation level depends on the abstract patch type, but the general idea is this:
          - In common cases, each tessellation level defines how many segments an edge is tessellated into( 4 means  one edge ==> 4 edges (2 vertices become 5)).
          - The outer tessellation levels define the tessellation for the outer edges of the patch.
          - The inner tessellation levels are for the number of tessellations within the patch.
          - Triangles only uses 1 inner level and 3 outer levels. The rest are ignored.
          - The patch can be discarded if any outer tessellation level <= 0, it gives chance to TCS to cull patches by passing 0 for a relevant outer tessellation level 
          - The effective tessellation levels = function{ value here, spacing policy }

          */
	    gl_TessLevelInner[0] = tessellationLevelInner;
	    gl_TessLevelOuter[0] = tessellationLevelOuter;
	    gl_TessLevelOuter[1] = tessellationLevelOuter;
	    gl_TessLevelOuter[2] = tessellationLevelOuter;
    }
    #undef ID
}

//< Where is "Patch Constant Function" ?
//< which is called per patch to calculate constant values valid for full patch.
