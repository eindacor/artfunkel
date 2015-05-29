#version 330

//uv coordinates from the texture
in vec2 UV;
in vec3 normal_direction;

uniform sampler2D myTextureSampler;
uniform float dim_factor = 1.0;

uniform bool color_override = false;
uniform vec4 override_color = vec4(1.0, 1.0, 1.0, 1.0);

uniform bool text = false;
uniform vec4 text_color = vec4(1.0, 1.0, 1.0, 1.0);
uniform vec4 transparency_color = vec4(1.0, 1.0, 1.0, 1.0);

//lighting dependencies
uniform mat4 model_matrix;
uniform mat4 view_matrix;
uniform bool use_lighting = false;
vec4 light_direction = vec4(-2.0, -1.0, -6.0, 1.0);

out vec4 output_color;

void main()
{
	vec3 texture_color = texture2D( myTextureSampler, UV).rgb;

	if (text)
	{		
		//check to see if color pulled from texture map matches that of the the green screen color
		vec3 transparency_delta = abs(transparency_color.rgb - texture_color);
		if (transparency_delta.x + transparency_delta.y + transparency_delta.y < .001)
			output_color = vec4(0.0, 0.0, 0.0, 0.0);

		else
		{
			float anti_alias_value = 1.0 - texture_color.y;
			output_color = vec4(text_color.x, text_color.y, text_color.z, anti_alias_value);
		}
		return;
	}

	if (!color_override)
	{
		if(use_lighting)
		{
			vec4 light_color = vec4(1.0, 1.0, 1.0, 1.0);
			vec3 worldspace_normal = vec3(model_matrix * vec4(normal_direction, 1.0));
			worldspace_normal = glm::normalize(worldspace_normal);

			float cosTheta = glm::clamp( glm::dot( worldspace_normal, light_direction ), 0.0, 1.0 );
			vec4 modified_color = light_color * cosTheta;

			output_color = modified_color;

			//vec4 diffuse_color = vec4(texture_color, 1.0);

			//vec3 undimmed_color = texture_color;
			//output_color = vec4(undimmed_color.x * dim_factor, undimmed_color.y * dim_factor, undimmed_color.z * dim_factor, 1.0);
		}

		else
		{
			vec3 undimmed_color = texture_color;
			output_color = vec4(undimmed_color.x * dim_factor, undimmed_color.y * dim_factor, undimmed_color.z * dim_factor, 1.0);
		}
	}

	else output_color = override_color;
}