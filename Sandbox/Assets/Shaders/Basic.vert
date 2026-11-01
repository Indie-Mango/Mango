#version 450 core
layout (location = 0) in vec3 a_Pos;
layout (location = 1) in vec2 a_TexCoord;

layout (location = 0) out vec2 v_TexCoord;

layout (set = 0, binding = 0) uniform MVP {
    mat4 Model;
    mat4 View;
    mat4 Proj;
} u_MVP;

void main() {
    gl_Position = u_MVP.Proj * u_MVP.View * u_MVP.Model * vec4(a_Pos, 1.0);
    v_TexCoord = a_TexCoord;
}
