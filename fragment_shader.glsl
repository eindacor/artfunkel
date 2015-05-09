#version 330 

//uv coordinates from the texture
in vec2 UV;
uniform sampler2D myTextureSampler;
uniform float dim_factor = 1.0f;
uniform bool draw_line = false; 

uniform bool color_override = false;
uniform vec4 override_color = vec4(1.0f, 1.0f, 1.0f, 1.0f);

uniform bool text = false;
uniform vec4 text_color = vec4(1.0f, 1.0f, 1.0f, 1.0f);
uniform vec4 transparency_color = vec4(1.0f, 1.0f, 1.0f, 1.0f);

out vec4 output_color;

void main()
{
	if (text)
	{
		vec3 texture_color = texture2D( myTextureSampler, UV).rgb;
		//check to see if color pulled from texture map matches that of the the green screen color
		vec3 transparency_delta = abs(transparency_color.rgb - texture_color);
		if (transparency_delta.x + transparency_delta.y + transparency_delta.y < .01)
			output_color = vec4(0.0f, 0.0f, 0.0f, 0.0f);

		else output_color = text_color;
		return;
	}

	if (!color_override)
	{
		vec3 undimmed_color = texture2D( myTextureSampler, UV).rgb;
		output_color = vec4(undimmed_color.x * dim_factor, undimmed_color.y * dim_factor, undimmed_color.z * dim_factor, 1.0f);
	}

	else output_color = override_color;
}