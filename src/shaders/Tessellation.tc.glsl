#version 440 core

struct V2T
{
    vec3 position;
    vec3 normal;
    vec4 color;
};

struct TC2E
{
    vec3 position;
    vec3 normal;
    vec4 color;
};

//< output patch size is the number of vertices in the output patch
layout (vertices = 3) out;
uniform float tessellationLevelInner;
uniform vec3  tessellationLevelOuter;

in  V2T vdata[];

//< output variables are passed directly to the TES, without any form of interpolation.
//< array size is implicit
out TC2E tcdata[];

patch out vec3 b300;
patch out vec3 b030;
patch out vec3 b003;
patch out vec3 b210;
patch out vec3 b120;
patch out vec3 b021;
patch out vec3 b012;
patch out vec3 b102;
patch out vec3 b201;
patch out vec3 b111;

void evaluateControlPoints(in vec3 p1, in vec3 p2, in vec3 p3,in vec3 n1, in vec3 n2, in vec3 n3)
{
    //< Calculate Vertex control points
    b300 = p1;
    b030 = p2;
    b003 = p3;

    //< Weight
    float w12 = dot(p2 - p1, n1);
    float w21 = dot(p1 - p2, n2);
    float w23 = dot(p3 - p2, n2);
    float w32 = dot(p2 - p3, n3);
    float w31 = dot(p1 - p3, n3);
    float w13 = dot(p3 - p1, n1);

    //< Tangent control points
    b210 = (2.f * p1 + p2 - w12 * n1) / 3.f;
    b120 = (2.f * p2 + p1 - w21 * n2) / 3.f;
    b021 = (2.f * p2 + p3 - w23 * n2) / 3.f;
    b012 = (2.f * p3 + p2 - w32 * n3) / 3.f;
    b102 = (2.f * p3 + p1 - w31 * n3) / 3.f;
    b201 = (2.f * p1 + p3 - w13 * n1) / 3.f;

    //< Center Control point 
    vec3 ee = (b120 + b120 + b021 + b012 + b102 + b210) * 0.25f;
    vec3 vv = (p1 + p2 + p3) * 0.166666667f;
    b111 = ee - vv;
}

patch out vec3 n200;
patch out vec3 n020;
patch out vec3 n002;
patch out vec3 n110;
patch out vec3 n011;
patch out vec3 n101;

void evaluateNormalControlPoints(in vec3 p1, in vec3 p2, in vec3 p3,in vec3 n1, in vec3 n2, in vec3 n3)
{
    n200 = n1;
    n020 = n2;
    n002 = n3;

    float v12 = (2. * (dot(p2 - p1, n1 + n2) / dot(p2 - p1, p2 - p1)));
    float v23 = (2. * (dot(p3 - p2, n2 + n3) / dot(p3 - p2, p3 - p2)));
    float v31 = (2. * (dot(p1 - p3, n3 + n1) / dot(p1 - p3, p1 - p3)));

    n110 = normalize(n1 + n2 - v12 * (p2 - p1));
    n011 = normalize(n2 + n3 - v23 * (p3 - p2));
    n101 = normalize(n3 + n1 - v31 * (p1 - p3));
}

void evaluateTesLevel()
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

    gl_TessLevelOuter[0] = tessellationLevelOuter.x;
    gl_TessLevelOuter[1] = tessellationLevelOuter.y;
    gl_TessLevelOuter[2] = tessellationLevelOuter.z;
}

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
//< gl_InvocationID    : the index of the TCS invocation within this patch.
//<                      A TCS invocation writes to per-vertex output variables by using this to index them.
//<
//< built-in variables from output of vertex shader
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
    #define ID gl_InvocationID //< once per vertex(Control Point)
    tcdata[ID].position = vdata[ID].position;
    tcdata[ID].normal   = vdata[ID].normal;
    tcdata[ID].color    = vdata[ID].color;

    if(ID == 0)
    {
        vec3 p1 = vdata[0].position;
        vec3 p2 = vdata[1].position;
        vec3 p3 = vdata[2].position;

        vec3 n1 = vdata[0].normal;
        vec3 n2 = vdata[1].normal;
        vec3 n3 = vdata[2].normal;

        evaluateControlPoints(p1, p2, p3, n1, n2, n3);
        evaluateNormalControlPoints(p1, p2, p3, n1, n2, n3);
        evaluateTesLevel();
    }
    #undef ID
}

//< Where is "Patch Constant Function" ?
//< It is called per patch to calculate constant values valid for full patch.
