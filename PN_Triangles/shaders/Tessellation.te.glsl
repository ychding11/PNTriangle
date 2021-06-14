#version 440 core

struct TC2E
{
    vec3 position;
    vec3 normal;
    vec4 color;
};

struct T2F
{
    vec3 position;
    vec3 normal;
    vec4 color;
};

layout(triangles, equal_spacing, ccw) in;
in TC2E tcdata[];
out T2F tedata;
out vec3 position_worldspace;
out vec3 normal_cameraspace;
out vec3 eyeDirection_cameraspace;
out vec3 lightDirection_cameraspace;

uniform mat4 M;
uniform mat4 V;
uniform mat4 P;
uniform vec3 lightPosition_worldspace;

void main() {
    vec3 p1 = tcdata[0].position;
    vec3 p2 = tcdata[1].position;
    vec3 p3 = tcdata[2].position;

    vec3 n1 = tcdata[0].normal;
    vec3 n2 = tcdata[1].normal;
    vec3 n3 = tcdata[2].normal;

    vec3 c1 = tcdata[0].color.rgb;
    vec3 c2 = tcdata[1].color.rgb;
    vec3 c3 = tcdata[2].color.rgb;

    float u = gl_TessCoord.x;
    float v = gl_TessCoord.y;
    float w = gl_TessCoord.z;

    vec3 b300 = p1;
    vec3 b030 = p2;
    vec3 b003 = p3;

    float w12 = dot(p2 - p1, n1);
    float w21 = dot(p1 - p2, n2);
    float w23 = dot(p3 - p2, n2);
    float w32 = dot(p2 - p3, n3);
    float w31 = dot(p1 - p3, n3);
    float w13 = dot(p3 - p1, n1);

    vec3 b210 = (2.*p1 + p2 - w12*n1) / 3.;
    vec3 b120 = (2.*p2 + p1 - w21*n2) / 3.;
    vec3 b021 = (2.*p2 + p3 - w23*n2) / 3.;
    vec3 b012 = (2.*p3 + p2 - w32*n3) / 3.;
    vec3 b102 = (2.*p3 + p1 - w31*n3) / 3.;
    vec3 b201 = (2.*p1 + p3 - w13*n1) / 3.;

    vec3 ee = (b120 + b120 + b021 + b012 + b102 + b210) / 6.;
    vec3 vv = (p1 + p2 + p3) / 3.;
    vec3 b111 = ee + (ee - vv) / 2.;

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

    tedata.position = pos;

    vec3 n200 = n1;
    vec3 n020 = n2;
    vec3 n002 = n3;

    float v12 = (2.*(dot(p2 - p1, n1 + n2) / dot(p2 - p1, p2 - p1)));
    float v23 = (2.*(dot(p3 - p2, n2 + n3) / dot(p3 - p2, p3 - p2)));
    float v31 = (2.*(dot(p1 - p3, n3 + n1) / dot(p1 - p3, p1 - p3)));

    vec3 n110 = normalize(n1 + n2 - v12*(p2 - p1));
    vec3 n011 = normalize(n2 + n3 - v23*(p3 - p2));
    vec3 n101 = normalize(n3 + n1 - v31*(p1 - p3));

    tedata.normal = n200 * w2 + n020 * u2 + n002 * v2
		    + n110 * w * u + n011 * u * v + n101 * w * v;
    //tedata.normal = n1 * w + n2 * u + n3 * v;

    tedata.color = vec4(c1 * w + c2 * u + c3 * v, 1.0);
    //tedata.color = vec4(c1, 1.0);

    gl_Position = P * V * M * vec4(pos, 1.0);

    position_worldspace = (M * vec4(pos, 1.0)).xyz;

    vec3 vertexPosition_cameraspace = (V * M * vec4(pos, 1.0)).xyz;
    eyeDirection_cameraspace = vec3(0.0f, 0.0f, 0.0f) - vertexPosition_cameraspace;

    vec3 lightPosition_cameraspace = (V * vec4(lightPosition_worldspace, 1)).xyz;
    lightDirection_cameraspace = lightPosition_cameraspace + eyeDirection_cameraspace;

    normal_cameraspace = (V * M * vec4(tedata.normal, 0)).xyz;
}
