#version 330

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 vertexUV;

uniform mat4 MVP;
uniform mat4 model_matrix;
uniform bool absolute_position;

out vec2 UV;

void main()
{
	if (absolute_position)
		gl_Position = model_matrix * vec4(position, 1.0f);

	else gl_Position = MVP * vec4(position, 1.0f);	
	UV = vertexUV;
}