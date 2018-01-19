#version 330 core

uniform sampler2D diffuseMap;

out vec4 color;

in vec2 TexCoords;
in vec3 vs_fs_normal;
in vec4 world_space_position;

void main()
{
	color = vec4(texture(diffuseMap, TexCoords).rgb, 1.0f);
}