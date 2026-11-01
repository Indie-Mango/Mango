#version 450 core
layout (location = 0) out vec4 FragColor;

layout (location = 0) in vec2 v_TexCoord;

layout (set = 1, binding = 0) uniform sampler2D u_Albedo;

void main() {
    FragColor = texture(u_Albedo, v_TexCoord);
}
