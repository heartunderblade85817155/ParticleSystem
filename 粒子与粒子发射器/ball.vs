#version 330 core

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texcoords;

out vec4 world_space_position;
out vec3 vs_fs_normal;
out vec2 TexCoords;

void main()
{
	world_space_position = model * vec4(position, 1.0f);
	vs_fs_normal = transpose(inverse(mat3(view * model))) * normal;
	TexCoords = texcoords;
	gl_Position = projection * view * model * vec4(position, 1.0f);
}