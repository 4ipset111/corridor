#ifndef __SHADER_SOURCES_H__
#define __SHADER_SOURCES_H__

const char* VERTEX_SHADER_SOURCE = R"(
#version 460 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 aTexCoord;
layout(location = 2) in vec3 aNormal;

out vec2 TexCoord;
out vec3 Normal;
out vec3 FragPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    FragPos = vec3(model * vec4(aPos, 1.0));
    Normal = mat3(transpose(inverse(model))) * aNormal;
    TexCoord = aTexCoord;
    gl_Position = projection * view * model * vec4(aPos, 1.0);
}
)";

const char* FRAGMENT_SHADER_SOURCE = R"(
#version 460 core
in vec2 TexCoord;
in vec3 Normal;
in vec3 FragPos;

out vec4 FragColor;

uniform sampler2D texture1;
uniform float time;
uniform float pixelSize;
uniform vec3 viewPos;
uniform vec3 viewDir;

float random(vec2 st) {
    return fract(sin(dot(st.xy, vec2(12.9898,78.233))) * 43758.5453123);
}

vec4 applyVHSEffect(vec2 uv, float t) {
    float jitter = random(vec2(t, t)) > 0.98 ? (random(vec2(t)) - 0.5) * 0.02 : 0.0;
    vec2 jitteredUV = uv + vec2(jitter, 0.0);

    vec2 pixelatedUV = floor(jitteredUV / pixelSize) * pixelSize;

    float glitch = sin(t * 10.0 + uv.y * 40.0) * 0.02;
    glitch += (random(vec2(floor(t * 10.0), floor(uv.y * 20.0))) - 0.5) * 0.05;
    if (random(vec2(t)) < 0.8) glitch *= 0.1;

    float r = texture(texture1, pixelatedUV + vec2(glitch + 0.005, 0.0)).r;
    float g = texture(texture1, pixelatedUV).g;
    float b = texture(texture1, pixelatedUV - vec2(glitch + 0.005, 0.0)).b;

    vec4 vhsColor = vec4(r, g, b, 1.0);

    float scanline = sin(uv.y * 400.0 - t * 10.0) * 0.15;
    vhsColor.rgb -= scanline;

    float noise = (random(vec2(uv.x + t, uv.y + t)) - 0.5) * 0.25;
    vhsColor.rgb += noise;

    vhsColor.rgb = mix(vhsColor.rgb, vec3(dot(vhsColor.rgb, vec3(0.299, 0.587, 0.114))), 0.2);

    float scratchX = random(vec2(floor(t * 10.0), 0.3));
    float scratch = 1.0 - smoothstep(0.0, 0.0015, abs(uv.x - scratchX));
    if (random(vec2(t, 0.5)) > 0.98)
        vhsColor.rgb -= scratch * 0.5;

    return vhsColor;
}

void main()
{
    vec3 norm = normalize(Normal);
    vec3 lightToFrag = normalize(FragPos - viewPos);

    float theta = dot(lightToFrag, normalize(viewDir));
    float cutOff = cos(radians(20.0));
    float outerCutOff = cos(radians(30.0));
    float epsilon = cutOff - outerCutOff;
    float intensity = clamp((theta - outerCutOff) / epsilon, 0.0, 1.0);

    float distance = length(viewPos - FragPos);
    float attenuation = 1.0 / (1.0 + 0.05 * distance + 0.01 * (distance * distance));

    float diff = max(dot(norm, -lightToFrag), 0.0);
    float ambient = 0.15;
    float lighting = (ambient + diff * intensity) * attenuation;

    vec4 vhsTexColor = applyVHSEffect(TexCoord, time);
    FragColor = vhsTexColor * vec4(vec3(lighting), 1.0);
}
)";

#endif // __SHADER_SOURCES_H__
