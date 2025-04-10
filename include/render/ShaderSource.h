// ShaderSource.h
#pragma once

namespace ShaderSource {

inline const char* huskVertex = R"glsl(
#version 330 core

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;

uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProjection;
uniform vec3 uColor;

out vec3 vNormal;
out vec3 vWorldPos;

void main() {
    vec4 worldPos = uModel * vec4(aPosition, 1.0);
    vWorldPos = worldPos.xyz;
    vNormal = mat3(transpose(inverse(uModel))) * aNormal;
    gl_Position = uProjection * uView * worldPos;
}
)glsl";

inline const char* huskFragment = R"glsl(
#version 330 core

in vec3 vNormal;
in vec3 vWorldPos;

uniform vec3 uColor;

out vec4 fragColor;

void main() {
    vec3 lightDir = normalize(vec3(1.0, 1.0, 1.0));
    float diffuse = max(dot(normalize(vNormal), lightDir), 0.4);
    vec3 finalColor = uColor * diffuse;
    fragColor = vec4(finalColor, 1.0);
}
)glsl";

}
