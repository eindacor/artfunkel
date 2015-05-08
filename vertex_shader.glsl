#version 330

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 vertexUV;

uniform mat4 MVP;
uniform mat4 model_matrix;
uniform bool absolute_position;
uniform float aspect_scale;

out vec2 UV;

void main()
{
	if (absolute_position)
	{
		vec4 translated = model_matrix * vec4(position, 1.0f);
		float modified_x = translated.x / aspect_scale; 
		gl_Position = vec4(modified_x, translated.y, translated.z, 1.0f);
	}

	else gl_Position = MVP * vec4(position, 1.0f);	

	UV = vertexUV;
}