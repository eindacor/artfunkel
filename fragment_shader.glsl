#version 330 

//uv coordinates from the texture
in vec2 UV;
uniform sampler2D myTextureSampler;

out vec3 output_color;

void main()
{
	output_color = texture2D( myTextureSampler, UV).rgb;
}