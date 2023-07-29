#version 440 core

struct TesOut
{
    vec3 position_world;
    vec3 normal_world;
    vec3 diffuse_color;
};

in TesOut tes_out;

out vec3 color;

uniform vec3 lightPosition_worldspace;
uniform vec3 eye_worldspace;

void main()
{
    vec3 lightColor = vec3(1, 1, 0.5);
    float lightPower = 600.0f;

    vec3 materialDiffuseColor = tes_out.diffuse_color;
    vec3 materialAmbientColor  = vec3(0.3, 0.3, 0.3) * materialDiffuseColor;
    vec3 materialSpecularColor = vec3(0.5, 0.5, 0.5) * materialDiffuseColor;

    float distance = length(lightPosition_worldspace - tes_out.position_world);

    vec3 n = normalize(tes_out.normal_world);
    vec3 l = normalize(lightPosition_worldspace - tes_out.position_world);
    vec3 e = normalize(eye_worldspace - tes_out.position_world);
    vec3 r = reflect(-l, n);

    float cosTheta = clamp(dot(n, l), 0, 1);
    float cosAlpha = clamp(dot(e, r), 0, 1);

    color = materialAmbientColor
        + materialDiffuseColor * lightColor * lightPower * cosTheta / (distance * distance)
        + materialSpecularColor * lightColor * lightPower * pow(cosAlpha, 5) / (distance * distance);
}
