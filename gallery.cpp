#include "gallery.h"
#include "artwork.h"
#include "utility_funcs.h"

bool display_wall::validPlacement(const shared_ptr<artwork> &placed, const vec2 &position)
{
	vec3 dimensions(placed->getOverallDimensions());
	float width(dimensions.x);
	float height(dimensions.y);

	float half_width(width / 2.0f);
	float half_height(height / 2.0f);

	vector<vec2> placed_points_to_check{
		vec2(half_width * -1.0f, half_height) + position,			//upper left
		vec2(half_width, half_height) + position, 					//upper right
		vec2(half_width * -1.0f, half_height * -1.0f) + position,	//lower left	
		vec2(half_width, half_height * -1.0f) + position			//lower right
	};

	for (auto i : placed_points_to_check)
	{
		//verifies paintings is within bounds of wall
		if (!jep::pointInPolygon(wall_points, i))
			return false;

		//verifies painting does not collide with other paintings
		for (auto p : wall_contents)
		{
			vec3 other_dimensions = p.second->getOverallDimensions();
			float other_half_width = other_dimensions.x / 2.0f;
			float other_half_height = other_dimensions.y / 2.0f;

			vector<vec2> other_bounding_points{
				vec2(other_half_width * -1.0f, other_half_height) + p.first,			//upper left
				vec2(other_half_width, other_half_height) + p.first, 					//upper right
				vec2(other_half_width * -1.0f, other_half_height * -1.0f) + p.first,	//lower left	
				vec2(other_half_width, other_half_height * -1.0f) + p.first				//lower right
			};

			if (jep::pointInPolygon(other_bounding_points, i))
				return false;
		}
	}

	return true;
}

/*
vec2 display_wall::getCursorLocationOnWall(shared_ptr<key_handler> &keys, const shared_ptr<ogl_camera> &camera) const
{
	pair<vec3, vec3> ray(getRayFromCursorPosition(keys, camera));

	mat4 inverse_wall_model_matrix = glm::inverse(wall_model_matrix);

	ray.first = vec3(inverse_wall_model_matrix * vec4(ray.first, 1.0f));
	ray.second = vec3(inverse_wall_model_matrix * vec4(ray.second, 1.0f));

	vec3 origin = ray.first;
	vec3 direction = ray.second - ray.first;

	vector< vector<vec3> > select_surfaces = art->getSelectSurfaces();

	//cycle through each surface, testing ray intersection
	for (auto i : select_surfaces)
	{
		if (i.size() != 3)
		{
			cout << "surface tested is missing vertices" << endl;
			return false;
		}

		vec3 first_point(i.at(1));
		vec3 second_point(i.at(0));
		vec3 third_point(i.at(2));
		vec3 result;

		if (glm::intersectRayTriangle(origin, direction, first_point, second_point, third_point, result))
		{

		}
	}
}
*/

void gallery::addPainting(int index, const shared_ptr<artwork> &work)
{
	//TODO check to verify painting position is not occupied
	//creates a copy of the instance passed, so the translation matrix applied doesn't affect the original
	works_displayed[index] = shared_ptr<artwork>(new artwork(*work));

	//center on eye level, unless painting is within .5 of floor
	float eye_level = 1.65f;
	float y_offset = 0.0f;
	float min_distance_from_floor = .5f;
	if ((works_displayed[index]->getData()->getHeight() * .0067f) + min_distance_from_floor > eye_level)
		y_offset = (works_displayed[index]->getData()->getHeight() / 200.0f) + min_distance_from_floor;

	else y_offset = eye_level - (works_displayed[index]->getData()->getHeight() / 600.0f);

	works_displayed[index]->moveRelative(glm::translate(mat4(1.0f), vec3(0.0f, y_offset, 0.0f)));
	works_displayed[index]->moveRelative(work_positions[index]);
}

void gallery::renderGallery(const shared_ptr<ogl_context> &ogl_con, const shared_ptr<ogl_camera> &ogl_cam) const {

	float eye_level = 1.65f;

	for (auto i : works_displayed)
		i.second->draw(ogl_con, ogl_cam);
}