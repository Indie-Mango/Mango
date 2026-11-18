#version 450

#define MAX_LIGHTS 64

#define LIGHT_DIRECTIONAL 0
#define LIGHT_POINT       1
#define LIGHT_SPOT        2

layout(location = 0) in vec3 v_WorldPosition;
layout(location = 1) in vec3 v_Normal;
layout(location = 2) in vec2 v_TexCoord;

layout(location = 0) out vec4 o_Color;

// Camera
layout(set = 0, binding = 0) uniform CameraData
{
    mat4 View;
    mat4 Projection;
    vec4 CameraPosition;
} u_Camera;

// Lights
struct Light
{
    vec4 Position;
    vec4 Direction;
    vec4 Color;
    vec4 Params;
    ivec4 Type;
};

layout(set = 0, binding = 2, std140) uniform LightData
{
    Light Lights[MAX_LIGHTS];
    ivec4 LightCount;
} u_Lights;

// Material
layout(set = 1, binding = 0, std140) uniform MaterialData
{
    vec4 BaseColorFactor;
    vec4 EmissiveFactor;

    float MetallicFactor;
    float RoughnessFactor;
    float AlphaCutoff;
    uint Flags;
} u_Material;

layout(set = 1, binding = 1) uniform sampler2D
u_BaseColorTexture;

layout(set = 1, binding = 2) uniform sampler2D
u_MetallicRoughnessTexture;

const uint HAS_BASE_COLOR_TEXTURE = 1u << 0;
const uint HAS_METALLIC_ROUGHNESS_TEXTURE = 1u << 1;
const uint ALPHA_MASK = 1u << 2;

const float PI = 3.14159265359;

// GGX normal distribution
float DistributionGGX(
    vec3 N, vec3 H, float roughness)
{
    float a = roughness * roughness;
    float a2 = a * a;

    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH * NdotH;

    float denominator =
    NdotH2 * (a2 - 1.0) + 1.0;

    return a2 / max(
        PI * denominator * denominator, 0.00001
    );
}

// Smith geometry
float GeometrySchlickGGX(
    float NdotV, float roughness)
{
    float r = roughness + 1.0;
    float k = (r * r) / 8.0;

    return NdotV / max(
        NdotV * (1.0 - k) + k, 0.00001
    );
}

float GeometrySmith(
    vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);

    return
    GeometrySchlickGGX(NdotV, roughness) *
    GeometrySchlickGGX(NdotL, roughness);
}

// Fresnel
vec3 FresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) *
    pow(1.0 - cosTheta, 5.0);
}

// Metallic-roughness BRDF
vec3 EvaluateBRDF(
    vec3 N,
    vec3 V,
    vec3 L,
    vec3 albedo,
    float metallic,
    float roughness)
{
    float NdotL = max(dot(N, L), 0.0);
    float NdotV = max(dot(N, V), 0.0);

    if (NdotL <= 0.0 || NdotV <= 0.0)
    return vec3(0.0);

    vec3 H = normalize(V + L);

    vec3 F0 = mix(
        vec3(0.04), albedo, metallic
    );

    float NDF = DistributionGGX(N, H, roughness);
    float G = GeometrySmith(N, V, L, roughness);

    vec3 F = FresnelSchlick(
        max(dot(H, V), 0.0), F0
    );

    vec3 specular =
    (NDF * G * F) /
    max(4.0 * NdotV * NdotL, 0.0001);

    vec3 kD = (vec3(1.0) - F) *
    (1.0 - metallic);

    vec3 diffuse = kD * albedo / PI;

    return diffuse + specular;
}

vec3 EvaluateLight(
    Light light,
    vec3 worldPos,
    vec3 N,
    vec3 V,
    vec3 albedo,
    float metallic,
    float roughness)
{
    vec3 L;
    float attenuation = 1.0;

    int type = light.Type.x;

    if (type == LIGHT_DIRECTIONAL)
    {
        // Direction is the direction light travels
        L = normalize(-light.Direction.xyz);
    }
    else
    {
        vec3 toLight = light.Position.xyz - worldPos;
        float distanceToLight = length(toLight);

        if (distanceToLight < 0.0001)
        return vec3(0.0);

        L = toLight / distanceToLight;

        float range = light.Params.y;

        // Inverse square attenuation with optional
        // finite-range fade
        attenuation = 1.0 / max(
            distanceToLight * distanceToLight,
            0.01
        );

        if (range > 0.0)
        {
            float normalizedDistance =
            distanceToLight / range;

            float fade = clamp(
                1.0 - pow(normalizedDistance, 4.0),
                0.0, 1.0
            );

            attenuation *= fade * fade;
        }

        if (type == LIGHT_SPOT)
        {
            vec3 spotDirection =
            normalize(light.Direction.xyz);

            // Direction from light toward fragment
            vec3 lightToFragment = -L;

            float cosAngle = dot(
                spotDirection, lightToFragment
            );

            float innerCos = light.Params.z;
            float outerCos = light.Params.w;

            float spotFactor = clamp(
                (cosAngle - outerCos) /
                max(innerCos - outerCos, 0.0001),
                0.0, 1.0
            );

            attenuation *= spotFactor;
        }
    }

    float NdotL = max(dot(N, L), 0.0);

    vec3 radiance =
    light.Color.rgb *
    light.Params.x *
    attenuation;

    return EvaluateBRDF(
        N, V, L, albedo, metallic, roughness
    ) * radiance * NdotL;
}

void main()
{
    vec4 baseColor = u_Material.BaseColorFactor;

    if ((u_Material.Flags &
    HAS_BASE_COLOR_TEXTURE) != 0u)
    {
        baseColor *= texture(
            u_BaseColorTexture, v_TexCoord
        );
    }

    if ((u_Material.Flags & ALPHA_MASK) != 0u &&
    baseColor.a < u_Material.AlphaCutoff)
    {
        discard;
    }

    float metallic = u_Material.MetallicFactor;
    float roughness = u_Material.RoughnessFactor;

    if ((u_Material.Flags &
    HAS_METALLIC_ROUGHNESS_TEXTURE) != 0u)
    {
        vec4 mr = texture(
            u_MetallicRoughnessTexture,
            v_TexCoord
        );

        roughness *= mr.g;
        metallic *= mr.b;
    }

    metallic = clamp(metallic, 0.0, 1.0);
    roughness = clamp(roughness, 0.045, 1.0);

    vec3 N = normalize(v_Normal);

    vec3 V = normalize(
        u_Camera.CameraPosition.xyz -
        v_WorldPosition
    );

    vec3 color = vec3(0.0);

    int count = clamp(
        u_Lights.LightCount.x, 0, MAX_LIGHTS
    );

    for (int i = 0; i < count; ++i)
    {
        color += EvaluateLight(
            u_Lights.Lights[i],
            v_WorldPosition,
            N,
            V,
            baseColor.rgb,
            metallic,
            roughness
        );
    }

    // Simple placeholder ambient
    vec3 ambient = baseColor.rgb * 0.03;

    color += ambient;

    // Emissive factor (texture can be added later)
    color += u_Material.EmissiveFactor.rgb;

    // Linear HDR output
    o_Color = vec4(color, baseColor.a);
}
