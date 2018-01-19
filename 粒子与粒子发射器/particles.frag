#version 330 core

in vec2 TexCoords;
in vec4 ParticleColor;

in vec3 position_out;
in vec3 velocity_out;
out vec4 color;

uniform sampler2D sprite;

void main()
{
    color = texture(sprite, TexCoords) * ParticleColor;
}