#include "hud.h"

hud_element::hud_element(const string &hud_identifier, const vec2 &anchor_point, const justpair &anchor_location,
	const vec2 &on_screen_dimensions, const hud_element_type &type)
{
	height = on_screen_dimensions.y;
	width = on_screen_dimensions.x;
	identifier = hud_identifier;

	float half_height(height / 2.0f);
	float half_width(width / 2.0f);

	float upper, lower, left, right;

	switch (anchor_location.first)
	{
	case H_LEFT: 
		left = anchor_point.x; 
		right = anchor_point.x + on_screen_dimensions.x;
		centerpoint.x = anchor_point.x + half_width;
		break;
	case H_RIGHT: 
		left = anchor_point.x - half_width; 
		right = anchor_point.x + half_width; 
		centerpoint.x = anchor_point.x - half_width;
		break;
	case H_CENTER: 
		left = anchor_point.x - on_screen_dimensions.x; 
		right = anchor_point.x; 
		centerpoint.x = anchor_point.x;
		break;
	}

	switch (anchor_location.second)
	{
	case V_TOP:
		upper = anchor_point.y;
		lower = anchor_point.y - on_screen_dimensions.y;
		centerpoint.y = anchor_point.y - half_height;
		break;
	case V_BOTTOM:
		upper = anchor_point.y - on_screen_dimensions.y;
		lower = anchor_point.y;
		centerpoint.y = anchor_point.y + half_height;
		break;
	case V_MIDDLE:
		upper = anchor_point.y + half_height;
		lower = anchor_point.y - half_height;
		centerpoint.y = anchor_point.y;
		break;
	}

	upper_left = vec2(left, upper);
	upper_right = vec2(right, upper);
	lower_left = vec2(left, lower);
	lower_right = vec2(right, lower);

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

	return currently_selected && selectable;
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
	for (const auto &i : element_map)
		i.second->draw(context, camera);

	glEnable(GL_DEPTH_TEST);
}

shared_ptr<hud_element> hud::getSelected(shared_ptr<key_handler> &keys, hud_element_type &type, string &identifier) const
{
	vec2 cursor_position = keys->getCursorPosition();
	for (const auto &i : element_map)
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
	for (const auto &element : line_contents)
	{
		individual_x_offset = (element->getWidth() / 2.0f) + previous_x_offset;
		previous_x_offset = individual_x_offset + (element->getWidth() / 2.0f);
		individual_x_offset += initial_x_offset;
		element->setX(individual_x_offset);

		if (element->getType() == TEXT_AREA)
		{
			shared_ptr<text_area> text_element = boost::dynamic_pointer_cast<text_area>(element);
			text_element->setVisible(text_element->getCurrentFirstLine());
		}

		else if (element->getType() == ELEMENT_ARRAY)
		{
			shared_ptr<dynamic_hud_array> text_element = boost::dynamic_pointer_cast<dynamic_hud_array>(element);
			text_element->setVisible(0);
		}
	}
}

void dynamic_hud_array::setYForLine(vector< shared_ptr<hud_element> > &line_contents, float y_offset)
{
	for (const auto &element : line_contents)
	{
		element->setY(y_offset);
		if (element->getType() == TEXT_AREA)
		{
			shared_ptr<text_area> text_element = boost::dynamic_pointer_cast<text_area>(element);
			text_element->setVisible(text_element->getCurrentFirstLine());
		}

		else if (element->getType() == ELEMENT_ARRAY)
		{
			shared_ptr<dynamic_hud_array> text_element = boost::dynamic_pointer_cast<dynamic_hud_array>(element);
			text_element->setVisible(0);
		}
	}
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

	float width_max = getWidth() - (array_padding.x * 2.0f);
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
	for (const auto &element_vec : visible_lines)
	{
		vector< shared_ptr<hud_element> > line_contents = element_vec.second;
		int line_number = element_vec.first;
		vec2 dimensions = line_dimensions.at(element_vec.first);

		switch (justification.first)
		{
		case H_LEFT:
			initial_x_offset = getCenterpoint().x - ((getWidth() / 2.0f) - array_padding.x);
			setXForLine(line_contents, initial_x_offset);
			break;

		case H_CENTER:
			//dimensions refers to the actual dimensions of the line contents, which is <- getWidth(), the overall binding width of the array
			initial_x_offset = getCenterpoint().x - (dimensions.x / 2.0f);
			setXForLine(line_contents, initial_x_offset);
			break;

		case H_RIGHT:
			//dimensions refers to the actual dimensions of the line contents, which is <- getWidth(), the overall binding width of the array
			initial_x_offset = getCenterpoint().x + ((getWidth() / 2.0f) - array_padding.x) - dimensions.x;
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
			y_offset = getCenterpoint().y + ((getHeight() / 2.0f) - array_padding.y) - (distance_from_top + (dimensions.y / 2.0f));
			setYForLine(line_contents, y_offset);
			break;

		case V_MIDDLE:
			y_offset = getCenterpoint().y + (total_height / 2.0f) - (distance_from_top + (dimensions.y / 2.0f));
			setYForLine(line_contents, y_offset);
			break;

		case V_BOTTOM:
			y_offset = getCenterpoint().y - ((getHeight() / 2.0f) - array_padding.y) + total_height - (distance_from_top + (dimensions.y / 2.0f));
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

void text_area::setPageData()
{
	float line_width_max = getWidth() - (element_padding.x * 2.0f);
	int max_characters = line_width_max / (character_dimensions.x * xy_spacing_scale.x);

	float page_height_max = getHeight() - (element_padding.y * 2.0f);
	
	float line_height_max = getHeight() - (element_padding.y * 2.0f);
	lines_per_page = line_height_max / (character_dimensions.y * xy_spacing_scale.y);

	//TODO explain below with comments
	line_map.clear();
	vector<char> char_vec;
	for (int c = 0; c < raw_text.size(); c++)
		char_vec.push_back(raw_text.at(c));

	for (vector<char>::iterator first_of_line = char_vec.begin(); first_of_line != char_vec.end(); first_of_line++)
	{
		bool endline = false;

		if (*first_of_line == '\n')
		{
			line_map.insert(pair<int, string>(line_map.size(), "\n"));
			continue;
		}
			
		if (*first_of_line == ' ')
			continue;

		vector<char>::iterator last_of_line = first_of_line;
		for (int i = 0; i < max_characters; i++)
		{
			last_of_line++;
			if (last_of_line == char_vec.end() || *last_of_line == '\n')
			{
				string extracted_line;
				extracted_line.insert(extracted_line.end(), first_of_line, last_of_line);
				line_map.insert(pair<int, string>(line_map.size(), extracted_line));
				
				endline = true;
				//first_of_line is set to last_of_line, because when loop continues, it will increment it to the location right after last_of_line
				first_of_line = last_of_line;
				break;
			}
		}
		
		if (first_of_line == char_vec.end())
			break;

		if (endline)
			continue;

		//if there are no breaks before line max, extract line
		if (std::find(first_of_line, last_of_line, ' ') == last_of_line)
		{
			string extracted_line;
			extracted_line.insert(extracted_line.end(), first_of_line, last_of_line);
			line_map.insert(pair<int, string>(line_map.size(), extracted_line));

			//first_of_line is set to last_of_line, because when loop continues, it will increment it to the location right after last_of_line
			first_of_line = last_of_line;
			continue;
		}

		else while (*last_of_line != ' ')
			last_of_line--;

		string extracted_line;
		extracted_line.insert(extracted_line.end(), first_of_line, last_of_line);
		line_map.insert(pair<int, string>(line_map.size(), extracted_line));

		//first_of_line is set to last_of_line, because when loop continues, it will increment it to the location right after last_of_line
		first_of_line = last_of_line;
	}

	setVisible(0);
}

bool text_area::setVisible(int first_line_index)
{
	if (first_line_index < 0)
		return setVisible(0);

	if (first_line_index >= line_map.size())
		return false;

	current_first_line = first_line_index;

	int lines_to_format = (line_map.size() - current_first_line < lines_per_page ? line_map.size() - current_first_line : lines_per_page);
	float total_height = (float)lines_to_format * (character_dimensions.y * xy_spacing_scale.y);

	visible_characters.clear();
	for (int i = current_first_line; i < current_first_line + lines_per_page && i < line_map.size(); i++)
	{
		int local_index = i - current_first_line;
		string line_to_format = line_map.at(i);
		float line_width = line_to_format.size() * (character_dimensions.x * xy_spacing_scale.x);

		vec2 starting_position;

		switch (justification.first)
		{
		case H_LEFT: starting_position.x = getCenterpoint().x - (getWidth() / 2.0f); break;
		case H_CENTER: starting_position.x = getCenterpoint().x - (line_width / 2.0f); break;
		case H_RIGHT: starting_position.x = getCenterpoint().x + (getWidth() / 2.0f) - line_width; break;
		default: break;
		}

		float distance_from_top_line = (float)local_index * (character_dimensions.y * xy_spacing_scale.y);
		//for each case, find the top-most point, then use distance from top/height of line to come down incrementally
		switch (justification.second)
		{
		case V_TOP: starting_position.y = getCenterpoint().y + (getHeight() / 2.0f) - 
			(distance_from_top_line /*+ (character_dimensions.y / 2.0f)*/); 
			break;
		case V_MIDDLE: starting_position.y = getCenterpoint().y + (total_height / 2.0f) - 
			(distance_from_top_line /*+ (character_dimensions.y / 2.0f)*/);
			break;
		case V_BOTTOM: starting_position.y = getCenterpoint().y - (getHeight() / 2.0f) + total_height - 
			(distance_from_top_line /*+ (character_dimensions.y / 2.0f)*/);
			break;
		default: break;
		}

		vec2 cursor_position = starting_position;

		for (const auto &c : line_to_format)
		{
			shared_ptr<text_character> toAdd(new text_character(c, text, cursor_position, UL, character_dimensions, ital));
			visible_characters.push_back(toAdd);

			cursor_position += vec2(character_dimensions.x * xy_spacing_scale.x, 0.0f);
			//cursor_position = toAdd->getUpperRight() - vec2((character_dimensions.x / 2.0f), 0.0f);
		}		
	}

	return true;
}

void text_area::draw(const shared_ptr<ogl_context> &context, const shared_ptr<ogl_camera> &camera) const
{
	glDisable(GL_DEPTH_TEST);
	drawBackground(context, camera);
	//drawLines(context, camera);

	glUniform1i(context->getShaderGLint(text_enable_shader_ID), true);

	//set text color
	glUniform4f(context->getShaderGLint(text_color_shader_ID),
		text_color.x, text_color.y, text_color.z, text_color.w);

	for (const auto &c : visible_characters)
		c->draw(context, camera);

	glUniform1i(context->getShaderGLint(text_enable_shader_ID), false);

	glEnable(GL_DEPTH_TEST);
}

shared_ptr<hud_element> dynamic_hud_array::getSelectedWithinArray(
	shared_ptr<key_handler> &keys, const vec2 &cursor_position, hud_element_type &type, string &identifier) const
{
	shared_ptr<hud_element> found = nullptr;
	for (const auto &i : visible_lines) {
		for (const auto &j : i.second) {
			j->clearBackgroundColor();
			if (j->itemSelected(keys, cursor_position))
			{
				if (j->getType() == ELEMENT_ARRAY && !j->isSelectable())
				{
					shared_ptr<dynamic_hud_array> nested_array = boost::dynamic_pointer_cast<dynamic_hud_array>(j);
					found = nested_array->getSelectedWithinArray(keys, cursor_position, type, identifier);
				}

				else if (j->isSelectable())
					found = j;
			}
		}
	}

	if (found == nullptr)
	{
		type = NO_TYPE;
		identifier = "";
	}

	else
	{
		type = found->getType();
		found->setBackgroundColor(vec4(1.0f, 1.0f, 1.0f, 0.5f));
		identifier = found->getIdentifier();
	}

	return found;
}

void dynamic_hud_array::deselectAllWithin()
{
	for (const auto &i : visible_lines) {
		for (const auto &j : i.second) {
			j->clearBackgroundColor();
			if (j->getType() == ELEMENT_ARRAY)
			{
				shared_ptr<dynamic_hud_array> nested_array = boost::dynamic_pointer_cast<dynamic_hud_array>(j);
				nested_array->deselectAllWithin();
				nested_array->deselect();
			}

			else j->deselect();
		}
	}
}

void dynamic_hud_array::draw(const shared_ptr<ogl_context> &context, const shared_ptr<ogl_camera> &camera) const
{
	glDisable(GL_DEPTH_TEST);
	drawBackground(context, camera);
	//drawLines(context, camera);
	for (const auto &i : visible_lines)
	{
		for (const auto &j : i.second)
		{
			j->draw(context, camera);
			//j->drawLines(context, camera);
		}
	}

	for (const auto &i : lines)
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