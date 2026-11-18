#version 450

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Normal;
layout(location = 2) in vec2 a_TexCoord;

layout(set = 0, binding = 0) uniform CameraData
{
    mat4 View;
    mat4 Projection;
    vec4 CameraPosition;
} u_Camera;

layout(set = 0, binding = 1) uniform ObjectData
{
    mat4 Model;
} u_Object;

layout(location = 0) out vec3 v_WorldPosition;
layout(location = 1) out vec3 v_Normal;
layout(location = 2) out vec2 v_TexCoord;

void main()
{
    vec4 worldPosition =
    u_Object.Model * vec4(a_Position, 1.0);

    v_WorldPosition = worldPosition.xyz;

    mat3 normalMatrix = transpose(
        inverse(mat3(u_Object.Model))
    );

    v_Normal = normalize(normalMatrix * a_Normal);
    v_TexCoord = a_TexCoord;

    gl_Position =
    u_Camera.Projection *
    u_Camera.View *
    worldPosition;
}
