## PNTrangle implementation by OpenGL

A PN triangle is a cubic triangular Bezier patch. Control points are merely
depending on the vertex positions and normals of a defining base triangle.
Each PN triangle in a mesh is thus independent of neighboring triangles.

Normals of base triangle shall have different values. If not,normal variation cannot work.

### Control point calcualation

![PNTriangle Control point](PNTriangle.drawio.svg)

The diagrams are extracted from paper. Read paper "Curved PN Triangles" for details.

### GLSL TCS code

``` glsl
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
```

### Questions
How to ensure no cracks after tessellation. I feel that it is hard to ensure that.


It is to mplement PN Triangle tessellation by OpenGL.

## Features 
- [x] Implement PN Triangle Tessellation
- [x] Camera supporting rotate, pan, zoom in / out.
- [x] Capture Color Buffer & save to disk in tga format
- [x] Rendering Image sequence
- [x] snap & save to file
- [x] Animation support 

## Plans

- [ ]  

## Control 

- Key 'space' : animation loop  rotate -> zoom in -> zoom out -> random 
- Key 't': toggle tesselation
- Key 'w': toggle wireframe 

## Screen 
- ![screen](./images/wireframe.png)

## Credits
- [glew]()
- [glfw]()
- [glm]()
- [stb image](https://github.com/nothings/stb)
- [tiny obj loader]()
