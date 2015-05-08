#version 330 

//uv coordinates from the texture
in vec2 UV;
uniform sampler2D myTextureSampler;
uniform float dim_factor = 1.0f;
uniform bool draw_line = false; 

uniform bool color_override = false;
uniform vec4 override_color = vec4(1.0f, 1.0f, 1.0f, 1.0f);

out vec4 output_color;

void main()
{
	if (!color_override)
	{
		vec3 undimmed_color = texture2D( myTextureSampler, UV).rgb;
		output_color = vec4(undimmed_color.x * dim_factor, undimmed_color.y * dim_factor, undimmed_color.z * dim_factor, 1.0f);
	}

	else output_color = override_color;
}