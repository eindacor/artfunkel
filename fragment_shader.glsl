#version 330 

//uv coordinates from the texture
in vec2 UV;
uniform sampler2D myTextureSampler;
uniform float dim_factor = 1.0f;
uniform bool draw_line = false; 

out vec4 output_color;

void main()
{
	vec3 undimmed_color = texture2D( myTextureSampler, UV).rgb;
	output_color = vec4(undimmed_color.x * dim_factor, undimmed_color.y * dim_factor, undimmed_color.z * dim_factor, 1.0f);

	if (draw_line)
		output_color = vec4(1.0f, 1.0f, 1.0f, 1.0f);
}