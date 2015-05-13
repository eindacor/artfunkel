#include "hud.h"

bool hud_item::itemSelected(shared_ptr<key_handler> &keys, const shared_ptr<ogl_camera> &camera, const vec2 &cursor_position) const
{
	if (cursor_position.x < upper_left.x || cursor_position.x > upper_right.x ||
		cursor_position.y < lower_left.y || cursor_position.y > upper_left.y)
		return false;

	else return true;
}

void artwork_thumbnail::setStored(const shared_ptr<ogl_context> &context)
{
	mat4 scale_matrix = calcScaleMatrix(context);
	mat4 translation_matrix = glm::translate(mat4(1.0f), vec3(getCenterpoint().x, getCenterpoint().y, 0.0f));
	model_matrix = translation_matrix * scale_matrix;
}

mat4 artwork_thumbnail::calcScaleMatrix(const shared_ptr<ogl_context> &context) const
{
	//x dimensionsthroughout are modified for aspect ratio stretching
	float actual_padding_x = thumbnail_padding * context->getAspectRatio();

	float max_dimension_x = (getWidth() * context->getAspectRatio()) - (2.0f * actual_padding_x);
	float max_dimension_y = getHeight() - (2.0f * thumbnail_padding);

	float scale_for_x = max_dimension_x / stored->getOverallDimensions().x;
	float scale_for_y = max_dimension_y / stored->getOverallDimensions().y;

	return (scale_for_x < scale_for_y ?
		glm::scale(mat4(1.0f), vec3(scale_for_x, scale_for_x, scale_for_x)) :
		glm::scale(mat4(1.0f), vec3(scale_for_y, scale_for_y, scale_for_y))
		);
}