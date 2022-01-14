#version 330
// vertex shader

in vec3 position;

uniform mat4x4 matrice;

void main() {
    gl_Position = matrice * vec4(position.xyz, 1);
}
