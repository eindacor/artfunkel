#include "hud.h"

hud_element::hud_element(const vec2 &item_centerpoint, float on_screen_width, float on_screen_height, hud_element_type type)
{
	height = on_screen_height;
	width = on_screen_width;
	centerpoint = item_centerpoint;

	float half_height(height / 2.0f);
	float half_width(width / 2.0f);

	upper_left = vec2(centerpoint.x - half_width, centerpoint.y + half_height);
	upper_right = vec2(centerpoint.x + half_width, centerpoint.y + half_height);
	lower_left = vec2(centerpoint.x - half_width, centerpoint.y - half_height);
	lower_right = vec2(centerpoint.x + half_width, centerpoint.y - half_height);

	preDrawSelected = nullptr; 
	postDrawSelected = nullptr;
	preDrawNotSelected = nullptr; 
	postDrawNotSelected = nullptr;

	currently_selected = false;
	currently_hovered = false;

	background_rec = nullptr;
	background_image = nullptr;

	element_type = type;
}

void hud_element::updatePoints()
{
	float half_height(height / 2.0f);
	float half_width(width / 2.0f);

	upper_left = vec2(centerpoint.x - half_width, centerpoint.y + half_height);
	upper_right = vec2(centerpoint.x + half_width, centerpoint.y + half_height);
	lower_left = vec2(centerpoint.x - half_width, centerpoint.y - half_height);
	lower_right = vec2(centerpoint.x + half_width, centerpoint.y - half_height);

	lines.clear();
	vec4 line_color(0.5f, 0.5f, 0.5f, 0.5f);

	shared_ptr<line> top(new line(
		vec4(centerpoint.x - half_width, centerpoint.y + half_height, 0.0f, 1.0f),
		vec4(centerpoint.x + half_width, centerpoint.y + half_height, 0.0f, 1.0f),
		line_color));

	shared_ptr<line> bottom(new line(
		vec4(centerpoint.x - half_width, centerpoint.y - half_height, 0.0f, 1.0f),
		vec4(centerpoint.x + half_width, centerpoint.y - half_height, 0.0f, 1.0f),
		line_color));

	shared_ptr<line> left(new line(
		vec4(centerpoint.x - half_width, centerpoint.y + half_height, 0.0f, 1.0f),
		vec4(centerpoint.x - half_width, centerpoint.y - half_height, 0.0f, 1.0f),
		line_color));

	shared_ptr<line> right(new line(
		vec4(centerpoint.x + half_width, centerpoint.y + half_height, 0.0f, 1.0f),
		vec4(centerpoint.x + half_width, centerpoint.y - half_height, 0.0f, 1.0f),
		line_color));

	shared_ptr<line> diagonal(new line(
		vec4(centerpoint.x - half_width, centerpoint.y + half_height, 0.0f, 1.0f),
		vec4(centerpoint.x + half_width, centerpoint.y - half_height, 0.0f, 1.0f),
		line_color));

	shared_ptr<line> middle(new line(
		vec4(centerpoint.x, centerpoint.y + half_height, 0.0f, 1.0f),
		vec4(centerpoint.x, centerpoint.y - half_height, 0.0f, 1.0f),
		line_color));

	shared_ptr<line> center(new line(
		vec4(centerpoint.x - half_width, centerpoint.y, 0.0f, 1.0f),
		vec4(centerpoint.x + half_width, centerpoint.y, 0.0f, 1.0f),
		line_color));

	lines.push_back(top);
	lines.push_back(bottom);
	lines.push_back(left);
	lines.push_back(right);
	lines.push_back(diagonal);
	lines.push_back(middle);
	lines.push_back(center);
}

bool hud_element::itemSelected(shared_ptr<key_handler> &keys, const vec2 &cursor_position)
{
	if (cursor_position.x < upper_left.x || cursor_position.x > upper_right.x ||
		cursor_position.y < lower_left.y || cursor_position.y > upper_left.y)
		currently_selected = false;

	else currently_selected = true;

	return currently_selected;
}

void hud_element::setBackgroundImage(const shared_ptr<ogl_context> &context, const char* image_path)
{
	background_image = shared_ptr<image>(new image(vec2(0.0f, 0.0f), vec2(width, height), context, image_path));
}

void hud_element::setBackgroundColor(vec4 color)
{
	background_rec = shared_ptr<rectangle>(new rectangle(vec2(0.0f, 0.0f), vec2(width, height), color));
}

void hud_element::drawBackground(const shared_ptr<ogl_context> &context, const shared_ptr<ogl_camera> &camera) const
{
	if (background_rec != nullptr)
		background_rec->draw(context, camera, glm::translate(mat4(1.0f), vec3(centerpoint.x, centerpoint.y, 0.0f)), true);

	if (background_image != nullptr)
		background_image->draw(context, camera, glm::translate(mat4(1.0f), vec3(centerpoint.x, centerpoint.y, 0.0f)), true);
}

mat4 artwork_thumbnail::calcScaleMatrix(const shared_ptr<ogl_context> &context) const
{
	//x dimensionsthroughout are modified for aspect ratio stretching
	float actual_padding_x = thumbnail_padding / context->getAspectRatio();
	float max_dimension_x = (getWidth() - (2.0f * actual_padding_x)) * context->getAspectRatio();
	float max_dimension_y = getHeight() - (2.0f * thumbnail_padding);

	float scale_for_x = max_dimension_x / stored->getOverallDimensions().x;
	float scale_for_y = max_dimension_y / stored->getOverallDimensions().y;

	return (scale_for_x < scale_for_y ?
		glm::scale(mat4(1.0f), vec3(scale_for_x, scale_for_x, 1.0f)) :
		glm::scale(mat4(1.0f), vec3(scale_for_y, scale_for_y, 1.0f))
		);
}

void artwork_thumbnail::draw(const shared_ptr<ogl_context> &context, const shared_ptr<ogl_camera> &camera) const
{
	drawBackground(context, camera);
	if (isCurrentlySelected() && preDrawSelected != nullptr)
		preDrawSelected(context, camera);

	mat4 aspect_matrix = glm::scale(mat4(1.0f), vec3(1.0f / context->getAspectRatio(), 1.0f, 1.0f));
	stored->draw2D(context, camera, getTranslationMatrix() * scale_matrix * aspect_matrix);

	if (isCurrentlySelected() && postDrawSelected != nullptr)
		postDrawSelected(context, camera);
}

bool artwork_thumbnail::isSelected(shared_ptr<key_handler> &keys, const vec2 &cursor_position, shared_ptr<artwork> &selected)
{
	if (itemSelected(keys, cursor_position))
	{
		selected = stored;
		cout << stored->getData()->getTitle() << " selected" << endl;
		return true;
	}

	else return false;
}

bool hud::addElement(string identifier, const shared_ptr<hud_element> &to_add)
{
	if (element_map.find(identifier) == element_map.end())
	{
		element_map.insert(pair<string, shared_ptr<hud_element> >(identifier, to_add));
		return true;
	}

	else return false;
}

shared_ptr<hud_element> hud::getElement(string identifier) const
{
	if (element_map.find(identifier) != element_map.end())
		return element_map.at(identifier);

	else return nullptr;
}

void hud::draw(const shared_ptr<ogl_context> &context, const shared_ptr<ogl_camera> &camera) const
{
	glDisable(GL_DEPTH_TEST);
	for (auto i : element_map)
		i.second->draw(context, camera);

	glEnable(GL_DEPTH_TEST);
}

shared_ptr<hud_element> hud::getSelected(shared_ptr<key_handler> &keys, hud_element_type &type, string &identifier) const
{
	vec2 cursor_position = keys->getCursorPosition();
	for (auto i : element_map)
	{
		if (i.second->itemSelected(keys, cursor_position))
		{
			//TODO adjust to allow for nested arrays
			if (i.second->getType() == ELEMENT_ARRAY)
			{
				shared_ptr<hud_element>within_array;
				within_array = i.second->getSelectedWithinArray(keys, cursor_position, type, identifier);
				return within_array;
			}

			else
			{
				identifier = i.first;
				type = i.second->getType();
				return i.second;
			}
		}
	}

	identifier = "";
	type = NO_TYPE;
	return nullptr;
}

void dynamic_hud_array::setXForLine(vector< shared_ptr<hud_element> > &line_contents, float initial_x_offset)
{
	float individual_x_offset = 0.0f;
	float previous_x_offset = 0.0f;
	for (auto element : line_contents)
	{
		individual_x_offset = (element->getWidth() / 2.0f) + previous_x_offset;
		previous_x_offset = individual_x_offset + (element->getWidth() / 2.0f);
		individual_x_offset += initial_x_offset;
		element->setX(individual_x_offset);
	}
}

void dynamic_hud_array::setYForLine(vector< shared_ptr<hud_element> > &line_contents, float y_offset)
{
	for (auto element : line_contents)
		element->setY(y_offset);
}

void dynamic_hud_array::addElement(const shared_ptr<hud_element> &to_add)
{
	array_elements.push_back(to_add);
	setPageData();
}

void dynamic_hud_array::addElements(const vector< shared_ptr<hud_element> > &element_vec)
{
	array_elements.insert(array_elements.end(), element_vec.begin(), element_vec.end());
	setPageData();
}

//this method cycles through all of the array elements and identifiers the "lines" to be displayed and their corresponding heights.
//then the page markers are determined from the height data and the binding height of the elment
void dynamic_hud_array::setPageData()
{
	visible_lines.clear();
	page_map.clear();

	float line_width_max = getWidth();
	float page_height_max = getHeight();

	//vector of pairs containing iterators and floats
	vector< pair<vector< shared_ptr<hud_element> >::iterator, float> >line_heights;
	float current_line_width = 0.0f;
	float current_line_height = 0.0f;
	bool new_line = true;
	vector< shared_ptr<hud_element> >::iterator current_line_first = array_elements.begin();
	for (vector< shared_ptr<hud_element> >::iterator it = array_elements.begin(); it != array_elements.end(); it++)
	{
		if (current_line_width + (*it)->getWidth() > line_width_max)
		{
			line_heights.push_back(pair< vector< shared_ptr<hud_element> >::iterator, float>(current_line_first, current_line_height));
			current_line_height = (*it)->getHeight();
			current_line_width = (*it)->getWidth();
			current_line_first = it;
		}

		else
		{
			current_line_width += (*it)->getWidth();

			if ((*it)->getHeight() > current_line_height)
				current_line_height = (*it)->getHeight();
		}

		if (it == array_elements.end() - 1)
			line_heights.push_back(pair< vector< shared_ptr<hud_element> >::iterator, float>(current_line_first, current_line_height));
	}

	float current_page_height = 0.0f;
	int page_counter = 0;
	for (int i = 0; i < line_heights.size(); i++)
	{
		pair<vector< shared_ptr<hud_element> >::iterator, float> line_data = line_heights.at(i);
		if (current_page_height + line_data.second > page_height_max || i == 0)
		{
			page_map[page_counter] = line_data.first;
			current_page_height = line_data.second;
			page_counter++;
		}

		else current_page_height += line_data.second;
	}

	setVisible(0);
}

bool dynamic_hud_array::setVisible(int page_number)
{
	if (page_number < 0 || page_map.find(page_number) == page_map.end())
		return false;

	current_page = page_number;

	vector< shared_ptr<hud_element> >::iterator start = page_map.at(page_number);
	vector< shared_ptr<hud_element> >::iterator end = (page_map.find(page_number + 1) == page_map.end() ? array_elements.end() : page_map.at(page_number + 1));

	float width_max = getWidth();
	visible_lines.clear();
	//total width and height of individual lines
	map< int, vec2 > line_dimensions;

	vector<shared_ptr<hud_element> > current_line;
	float current_line_width = 0.0f;
	float current_line_height = 0.0f;
	int line_count = 0;
	float total_height = 0.0f;
	for (vector< shared_ptr<hud_element> >::iterator it = start; it != end; it++)
	{
		if (current_line_width + (*it)->getWidth() > width_max)
		{
			//submit current line, check new value
			if (current_line.size() > 0)
			{
				visible_lines.insert(pair<int, vector<shared_ptr<hud_element> > >(line_count, current_line));
				line_dimensions.insert(pair<int, vec2>(line_count, vec2(current_line_width, current_line_height)));
				total_height += current_line_height;
				line_count++;
				current_line.clear();
			}

			current_line_width = (*it)->getWidth();
			current_line_height = (*it)->getHeight();
			current_line.push_back(*it);
		}

		//add element to line
		else
		{
			//add element
			current_line.push_back(*it);
			//adjust width
			current_line_width += (*it)->getWidth();
			//adjust height
			if ((*it)->getHeight() > current_line_height)
				current_line_height = (*it)->getHeight();
		}

		//if it's the last of the array, add current line
		if (it == end - 1)
		{
			visible_lines.insert(pair<int, vector<shared_ptr<hud_element> > >(line_count, current_line));
			line_dimensions.insert(pair<int, vec2>(line_count, vec2(current_line_width, current_line_height)));
			total_height += current_line_height;
		}
	}

	float initial_x_offset = 0.0f;
	float distance_from_top = 0.0f;
	float y_offset = 0.0f;
	for (auto element_vec : visible_lines)
	{
		vector< shared_ptr<hud_element> > line_contents = element_vec.second;
		int line_number = element_vec.first;
		vec2 dimensions = line_dimensions.at(element_vec.first);

		switch (justification.first)
		{
		case H_LEFT:
			initial_x_offset = getCenterpoint().x - (getWidth() / 2.0f);
			setXForLine(line_contents, initial_x_offset);
			break;

		case H_CENTER:
			//dimensions refers to the actual dimensions of the line contents, which is <- getWidth(), the overall binding width of the array
			initial_x_offset = getCenterpoint().x - (dimensions.x / 2.0f);
			setXForLine(line_contents, initial_x_offset);
			break;

		case H_RIGHT:
			//dimensions refers to the actual dimensions of the line contents, which is <- getWidth(), the overall binding width of the array
			initial_x_offset = getCenterpoint().x + (getWidth() / 2.0f) - dimensions.x;
			setXForLine(line_contents, initial_x_offset);
			break;

		case H_STRETCH:
		default:
			break;
		}

		//for each case, find the top-most point, then use distance from top/height of line to come down incrementally
		switch (justification.second)
		{
		case V_TOP:
			y_offset = getCenterpoint().y + (getHeight() / 2.0f) - (distance_from_top + (dimensions.y / 2.0f));
			setYForLine(line_contents, y_offset);
			break;

		case V_MIDDLE:
			y_offset = getCenterpoint().y + (total_height / 2.0f) - (distance_from_top + (dimensions.y / 2.0f));
			setYForLine(line_contents, y_offset);
			break;

		case V_BOTTOM:
			y_offset = getCenterpoint().y - (getHeight() / 2.0f) + total_height - (distance_from_top + (dimensions.y / 2.0f));
			setYForLine(line_contents, y_offset);
			break;

		case V_STRETCH:
		default:
			break;
		}

		//actively keep track of how far the lines are from the top of the total_height value, in order to set the y offset
		distance_from_top += dimensions.y;
	}

	return true;
}


shared_ptr<hud_element> dynamic_hud_array::getSelectedWithinArray(
	shared_ptr<key_handler> &keys, const vec2 &cursor_position, hud_element_type &type, string &identifier) const
{
	shared_ptr<hud_element> found = nullptr;
	for (auto i : visible_lines) {
		for (auto j : i.second) {
			j->clearBackgroundColor();
			if (j->itemSelected(keys, cursor_position))
			{
				if (j->getType() == ELEMENT_ARRAY)
					j->getSelectedWithinArray(keys, cursor_position, type, identifier);

				else
				{
					type = j->getType();
					found = j;
					found->setBackgroundColor(vec4(1.0f, 1.0f, 1.0f, 0.5f));
				}
			}
		}
	}

	identifier = "";
	if (found == nullptr)
		type = NO_TYPE;

	return found;
}

void dynamic_hud_array::draw(const shared_ptr<ogl_context> &context, const shared_ptr<ogl_camera> &camera) const
{
	glDisable(GL_DEPTH_TEST);
	drawBackground(context, camera);
	for (auto i : visible_lines)
	{
		for (auto j : i.second)
		{
			j->draw(context, camera);
			//j->drawLines(context, camera);
		}
	}

	for (auto i : lines)
		i->draw(context, camera, true);

	glEnable(GL_DEPTH_TEST);
}

void dynamic_hud_array::setLines()
{
	lines.clear();

	float half_height(getHeight() / 2.0f);
	float half_width(getHeight() / 2.0f);

	vec4 line_color(0.5f, 0.5f, 0.5f, 1.0f);

	shared_ptr<line> top(new line(
		vec4(-1.0f, getCenterpoint().y + half_height, 0.0f, 1.0f),
		vec4(getCenterpoint().x + half_width, getCenterpoint().y + half_height, 0.0f, 1.0f),
		line_color));

	shared_ptr<line> bottom(new line(
		vec4(getCenterpoint().x - half_width, getCenterpoint().y - half_height, 0.0f, 1.0f),
		vec4(getCenterpoint().x + half_width, getCenterpoint().y - half_height, 0.0f, 1.0f),
		line_color));

	shared_ptr<line> left(new line(
		vec4(getCenterpoint().x - half_width, getCenterpoint().y + half_height, 0.0f, 1.0f),
		vec4(getCenterpoint().x - half_width, getCenterpoint().y - half_height, 0.0f, 1.0f),
		line_color));

	shared_ptr<line> right(new line(
		vec4(getCenterpoint().x + half_width, getCenterpoint().y + half_height, 0.0f, 1.0f),
		vec4(getCenterpoint().x + half_width, getCenterpoint().y - half_height, 0.0f, 1.0f),
		line_color));

	shared_ptr<line> diagonal(new line(
		vec4(getCenterpoint().x - half_width, getCenterpoint().y + half_height, 0.0f, 1.0f),
		vec4(getCenterpoint().x + half_width, getCenterpoint().y - half_height, 0.0f, 1.0f),
		line_color));

	shared_ptr<line> middle(new line(
		vec4(getCenterpoint().x, getCenterpoint().y + half_height, 0.0f, 1.0f),
		vec4(getCenterpoint().x, getCenterpoint().y - half_height, 0.0f, 1.0f),
		line_color));

	shared_ptr<line> center(new line(
		vec4(getCenterpoint().x - half_width, getCenterpoint().y, 0.0f, 1.0f),
		vec4(getCenterpoint().x + half_width, getCenterpoint().y, 0.0f, 1.0f),
		line_color));

	lines.push_back(top);
	lines.push_back(bottom);
	lines.push_back(left);
	lines.push_back(right);
	lines.push_back(diagonal);
	lines.push_back(middle);
	lines.push_back(center);
}