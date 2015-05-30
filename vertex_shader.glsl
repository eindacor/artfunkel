#version 330

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 vertexUV;
layout (location = 2) in vec3 normal;

uniform mat4 MVP;

out vec2 UV;
out vec3 original_normal_direction;
out vec3 original_vertex_position;

void main()
{
	gl_Position = MVP * vec4(position, 1.0);	

	UV = vertexUV;
	original_vertex_position = position;
	original_normal_direction = normal;
}