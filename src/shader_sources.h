#ifndef __SHADER_SOURCES_H__
#define __SHADER_SOURCES_H__

const char* VERTEX_SHADER_SOURCE = R"(
#version 460 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 aTexCoord;
layout(location = 2) in vec3 aNormal;
layout(location = 3) in float aTexID;

out vec2 TexCoord;
out vec3 Normal;
out vec3 FragPos;
out flat float TexID;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform float doorShake;

void main()
{
    vec3 pos = aPos;
    if (aTexID > 1.5) pos.x += doorShake;
    FragPos = vec3(model * vec4(pos, 1.0));
    Normal = mat3(transpose(inverse(model))) * aNormal;
    TexCoord = aTexCoord;
    TexID = aTexID;
    gl_Position = projection * view * model * vec4(pos, 1.0);
}
)";

const char* FRAGMENT_SHADER_SOURCE = R"(
#version 460 core
in vec2 TexCoord;
in vec3 Normal;
in vec3 FragPos;

out vec4 FragColor;
in flat float TexID;

uniform sampler2DArray textureArray;
uniform float time;
uniform float pixelSize;
uniform vec3 viewPos;
uniform float flashlightOn;
uniform vec3 viewDir;
uniform float pixelationOn;
uniform float vhsOn;

float random(vec2 st) {
    return fract(sin(dot(st.xy, vec2(12.9898,78.233))) * 43758.5453123);
}

vec4 applyVHSEffect(vec2 uv, float t) {
    float jitter = (vhsOn > 0.5 && random(vec2(t, t)) > 0.98) ? (random(vec2(t)) - 0.5) * 0.02 : 0.0;
    vec2 jitteredUV = uv + vec2(jitter, 0.0);

    vec2 pixelatedUV = (pixelationOn > 0.5) ? floor(jitteredUV / pixelSize) * pixelSize : jitteredUV;

    float glitch = 0.0;
    if (vhsOn > 0.5) {
        glitch = sin(t * 10.0 + uv.y * 40.0) * 0.02;
        glitch += (random(vec2(floor(t * 10.0), floor(uv.y * 20.0))) - 0.5) * 0.05;
        if (random(vec2(t)) < 0.8) glitch *= 0.1;
    }

    vec3 uvLayer = vec3(pixelatedUV, TexID);
    float r = texture(textureArray, uvLayer + vec3(glitch + 0.005, 0.0, 0.0)).r;
    float g = texture(textureArray, uvLayer).g;
    float b = texture(textureArray, uvLayer - vec3(glitch + 0.005, 0.0, 0.0)).b;

    vec4 vhsColor = vec4(r, g, b, 1.0);

    if (vhsOn > 0.5) {
        float scanline = sin(uv.y * 400.0 - t * 10.0) * 0.15;
        vhsColor.rgb -= scanline;

        float noise = (random(vec2(uv.x + t, uv.y + t)) - 0.5) * 0.25;
        vhsColor.rgb += noise;

        vhsColor.rgb = mix(vhsColor.rgb, vec3(dot(vhsColor.rgb, vec3(0.299, 0.587, 0.114))), 0.2);

        float scratchX = random(vec2(floor(t * 10.0), 0.3));
        float scratch = 1.0 - smoothstep(0.0, 0.0015, abs(uv.x - scratchX));
        if (random(vec2(t, 0.5)) > 0.98)
            vhsColor.rgb -= scratch * 0.5;
    }

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
    float lighting = (ambient + diff * intensity * flashlightOn) * attenuation;

    vec4 vhsTexColor = applyVHSEffect(TexCoord, time);
    FragColor = vhsTexColor * vec4(vec3(lighting), 1.0);
}
)";

const char* TEXT_VERTEX_SOURCE = R"(
#version 460 core
layout (location = 0) in vec4 vertex; // <vec2 pos, vec2 tex>
out vec2 TexCoords;
uniform mat4 projection;
void main() {
    gl_Position = projection * vec4(vertex.xy, 0.0, 1.0);
    TexCoords = vertex.zw;
}
)";

const char* TEXT_FRAGMENT_SOURCE = R"(
#version 460 core
in vec2 TexCoords;
out vec4 color;
uniform sampler2D text;
uniform vec3 textColor;
void main() {
    vec4 sampled = vec4(1.0, 1.0, 1.0, texture(text, TexCoords).r);
    color = vec4(textColor, 1.0) * sampled;
}
)";

#endif // __SHADER_SOURCES_H__
