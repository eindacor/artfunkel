#include "gallery.h"
#include "artwork.h"

bool display_wall::validPlacement(const shared_ptr<artwork> &placed, const vec2 &position)
{
	vec3 dimensions(placed->getOverallDimensions());
	float width(dimensions.x);
	float height(dimensions.y);

	float half_width(width / 2.0f);
	float half_height(height / 2.0f);

	vector<vec2> points_to_check{
		vec2(half_width * -1.0f, half_height) + position,			//upper left
		vec2(half_width, half_height) + position, 					//upper right
		vec2(half_width * -1.0f, half_height * -1.0f) + position,	//lower left	
		vec2(half_width, half_height * -1.0f) + position			//lower right
	};

	for (vector<vec2>::const_iterator it = wall_points.cbegin(); it != wall_points.cend(); it++)
	{
		vector<vec2>::const_iterator next;

		if (it + 1 == wall_points.end())
			next = wall_points.cbegin();

		else next = it + 1;


	}
}

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