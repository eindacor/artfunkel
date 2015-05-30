#version 330

//uv coordinates from the texture
in vec2 UV;
in vec3 original_normal_direction;
in vec3 original_vertex_position;

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

//light parameters
vec3 light_direction_worldspace = normalize(vec3(1.0, -7.0, 2.0));
vec3 light_color = vec3(1.0, 1.0, 1.0);
float light_power = 1.0;

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
			//normal calcs
			vec3 translated_normal = original_vertex_position + original_normal_direction;
			translated_normal = vec3(model_matrix * vec4(translated_normal, 1.0));
			vec3 translated_vertex_position = vec3(model_matrix * vec4(original_vertex_position, 1.0));
			vec3 new_normal_direction = translated_vertex_position - translated_normal;

			float cosTheta = clamp( dot(light_direction_worldspace, new_normal_direction), 0, 1 );

			vec3 ambient_light = texture_color * vec3(0.7, 0.7, 0.7);

			vec3 modified_color = (light_color * light_power * cosTheta * texture_color) + ambient_light;

			modified_color.x = clamp(modified_color.x, ambient_light.x, texture_color.x);
			modified_color.y = clamp(modified_color.y, ambient_light.y, texture_color.y);
			modified_color.z = clamp(modified_color.z, ambient_light.z, texture_color.z);

			output_color = vec4(modified_color, 1.0);
		}

		else
		{
			vec3 undimmed_color = texture_color;
			output_color = vec4(undimmed_color.x * dim_factor, undimmed_color.y * dim_factor, undimmed_color.z * dim_factor, 1.0);
		}
	}

	else output_color = override_color;
}