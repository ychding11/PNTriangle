#version 440 core

struct TC2E
{
    vec3 position;
    vec3 normal;
    vec4 color;
};

struct TesOut
{
    vec3 position_world;
    vec3 normal_world;
    vec3 diffuse_color;
};

//< specify domain, spacing policy for Tessellator
/*
    ## input layout qualifiers
    - specify options that control the particular form of tessellation
    - equal_spacing            : clamp to [1, max], rounded up to the nearest integer
    - fractional_even_spacing  : clamp to [2, max], rounded up to the nearest even integer
    - fractional_odd_spacing   : clamp to [1, max - 1], rounded up to the nearest odd integer

    - If all of the effective outer levels (as computed above) levels are exactly 1.0, and the effective inner level is also 1.0,
      then nothing is tessellated, and the TES will get 3 vertices and one triangle. 
*/
layout(triangles, equal_spacing, ccw) in;

//< per-vertex inputs from the TCS.
//< They are in array indexed implicitly
in TC2E tcdata[];


out TesOut tes_out;

uniform mat4 M;
uniform mat4 V;
uniform mat4 P;
uniform vec3 mesh_color;

//< per-patch outputs from the TCS as inputs in the TES using the patch keyword
patch in vec3 b300;
patch in vec3 b030;
patch in vec3 b003;
patch in vec3 b210;
patch in vec3 b120;
patch in vec3 b021;
patch in vec3 b012;
patch in vec3 b102;
patch in vec3 b201;
patch in vec3 b111;
patch in vec3 n200;
patch in vec3 n020;
patch in vec3 n002;
patch in vec3 n110;
patch in vec3 n011;
patch in vec3 n101;

vec3 evaluatePosistion(in vec3 tessCoord)
{
    float u =tessCoord.x;
    float v =tessCoord.y;
    float w =tessCoord.z;

    float u3 = u * u * u;
    float v3 = v * v * v;
    float w3 = w * w * w;
    float u2 = u * u;
    float v2 = v * v;
    float w2 = w * w;

    vec3 pos = b300 * w3 + b030 * u3 + b003 * v3
             + b210 * 3. * w2 * u + b120 * 3. * w * u2 + b201 * 3. * w2 * v
             + b021 * 3. * u2 * v + b102 * 3. * w * v2 + b012 * 3. * u * v2
             + b012 * 6. * w * u * v;
    return pos;
}

vec3 evaluateNormal(in vec3 tessCoord)
{
    float u =tessCoord.x;
    float v =tessCoord.y;
    float w =tessCoord.z;

    float u2 = u * u;
    float v2 = v * v;
    float w2 = w * w;

    vec3 normal;
#if 1
    normal = n200 * w2 + n020 * u2 + n002 * v2
           + n110 * w * u + n011 * u * v + n101 * w * v;
#else
    //< Linearly varying normals
    normal = n1 * w + n2 * u + n3 * v;
#endif
    return normal;
}


//<
//< Evaluate surface for given UV coordinate
//< Interpolate attributes of vertex 
//< Maybe displacement here
//<
/*
    - Reference : https://www.khronos.org/opengl/wiki/Tessellation_Evaluation_Shader
    - The inputs for the TES are : per-vertex & per-patch 
    - built-in variables
      - in vec3 gl_TessCoord      : Barycentric coordinates of generated primitive
      - in int gl_PatchVerticesIn : the vertex count for the patch being processed
      - in int gl_PrimitiveID     : the index of the current patch in the series of patches being processed for this draw call

      - buit-in input 
          in gl_PerVertex
            {
              vec4 gl_Position;
              float gl_PointSize;
              float gl_ClipDistance[];
            } gl_in[gl_MaxPatchVertices];

      - buit-in output
          out gl_PerVertex
          {
          vec4 gl_Position;   //< the clip-space output position of the current vertex.
          float gl_PointSize; //< the pixel width/height of the point being rasterized. valid for point primitives
          float gl_ClipDistance[]; //< allows shader to set the distance from the vertex to each User-Defined Clip Plane
          };
*/
void main()
{
    //<
    //< The built-in input variable vec3 gl_TessCoord, which comes from tessellator
    //< It is used to identify the coordinate of generated point in "abstrct patch" or "domain"
    //< It is Barycentric coordinate
    //<
    vec3 pos = evaluatePosistion(gl_TessCoord);
    vec3 normal = evaluateNormal(gl_TessCoord);

    tes_out.diffuse_color  = mesh_color;
    tes_out.position_world = (M * vec4(pos, 1.0)).xyz;
    tes_out.normal_world   = normal;

    gl_Position = P * V * M * vec4(pos, 1.0);
}
