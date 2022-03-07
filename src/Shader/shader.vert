#version 460

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aColor;
layout(location = 2) in vec2 aTex;

layout(location = 3) out vec3 ourColor;
layout(location = 4) out vec2 ourTex;

void main() {
    gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);
    ourColor = aColor;
    ourTex = aTex;
}
