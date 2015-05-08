#include "utility_funcs.h"
#include "artwork.h"

string getDateString(const date &d, bool include_day)
{
	string date_string;

	bool day_known = d.getDayOfWeek() != UNKNOWN_DAY;

	if (day_known && include_day)
		date_string += d.getDayString() + ", ";

	if (d.getMonth() != UNKNOWN_MONTH)
		date_string += d.getMonthString() + " ";

	if (day_known)
		date_string += std::to_string(d.getDayOfMonth()) + ", ";

	date_string += std::to_string(d.getYear());

	return date_string;
}

bignum lookupValue(rarity work_rarity, bignum work_rarity_scale)
{
	//placeholder function
	bignum min, max;
	switch (work_rarity)
	{
	case COMMON:
		min = bignum("5000");
		max = bignum("15000");
		break;
	case UNCOMMON:
		min = bignum("15000");
		max = bignum("45000");
		break;
	case RARE:
		min = bignum("45000");
		max = bignum("165000");
		break;
	case LEGENDARY:
		min = bignum("645000");
		max = bignum("2565000");
		break;
	case MASTERPIECE:
		min = bignum("2565000");
		max = bignum("10245000");
		break;
	}

	bignum value = ((max - min) * work_rarity_scale) + min;

	return value;
}

void printDate(date d, bool include_day)
{
	cout << getDateString(d, include_day) << endl;
}

genre genreFromString(string s)
{
	if (s == "OTHER") return OTHER;
	if (s == "DADAISM") return DADAISM;
	if (s == "SURREALISM") return SURREALISM;
	if (s == "IMPRESSIONISM") return IMPRESSIONISM;
	if (s == "POST-IMPRESSIONISM") return POST_IMPRESSIONISM;
	if (s == "PORTRAIT") return PORTRAIT;
	if (s == "ABSTRACT") return ABSTRACT;
	if (s == "EXPRESSIONISM") return EXPRESSIONISM;
	if (s == "CONTEMPORARY") return CONTEMPORARY;
	if (s == "SCENE") return SCENE;
	if (s == "MODERNISM") return MODERNISM;
	if (s == "POST-MODERNISM") return POST_MODERNISM;
	if (s == "POINTISM") return POINTISM;
	if (s == "SKETCH") return SKETCH;
	if (s == "POP ART") return POP_ART;
	if (s == "HISTORIC") return HISTORIC;
	if (s == "LANDSCAPE") return LANDSCAPE;
	if (s == "FOLK ART") return FOLK_ART;
	if (s == "FAUVISM") return FAUVISM;
	if (s == "CUBISM") return CUBISM;
	if (s == "PHOTOGRAPH") return PHOTOGRAPH;

	return UNKNOWN_GENRE;
}

string stringFromGenre(genre g)
{
	switch (g)
	{
	case OTHER: return "Other";
	case DADAISM: return "Dadaism";
	case SURREALISM: return "Surrealism";
	case IMPRESSIONISM: return "Impressionism";
	case POST_IMPRESSIONISM: return "Post-Impressionism";
	case PORTRAIT: return "Portrait";
	case ABSTRACT: return "Abstract";
	case EXPRESSIONISM: return "Expressionsim";
	case CONTEMPORARY: return "Contemporary";
	case SCENE: return "Scene";
	case MODERNISM: return "Modernism";
	case POST_MODERNISM: return "Post-Modernism";
	case POINTISM: return "Pointism";
	case SKETCH: return "Sketch";
	case POP_ART: return "Pop Art";
	case HISTORIC: return "Historic";
	case LANDSCAPE: return "Landscape";
	case FOLK_ART: return "Folk Art";
	case FAUVISM: return "Fauvism";
	case CUBISM: return "Cubism";
	case PHOTOGRAPH: return "Photograph";
	default: cout << "genre not found: " << g << endl;
		return "Unknown Genre";
	}
	
}

rarity rarityFromString(string s)
{
	if (s == "COMMON")
		return COMMON;

	if (s == "UNCOMMON")
		return UNCOMMON;

	if (s == "RARE")
		return RARE;

	if (s == "LEGENDARY")
		return LEGENDARY;

	if (s == "MASTERPIECE")
		return MASTERPIECE;

	return UNKNOWN_RARITY;
}

string stringFromRarity(rarity r)
{
	switch (r)
	{
	case COMMON: return "Common";
	case UNCOMMON: return "Uncommon";
	case RARE: return "Rare";
	case LEGENDARY: return "Legendary";
	case MASTERPIECE: return "Masterpiece";
	default: return "Unknown Rarity";
	}
}

float getDelta(vec3 first, vec3 second, char axis)
{
	switch (axis)
	{
	case 'x': return second.x - first.x;
	case 'y': return second.y - first.y;
	case 'z': return second.z - first.z;
	default: return 0.0f;
	}
}

vector<float> generateInterleavedVertices(vec3 bottom_left, vec3 top_left, vec3 top_right, vec3 bottom_right, 
		float uv_map_dimension, char u_axis, char v_axis)
{
	vector<float> geometry_data = {
		bottom_left.x, bottom_left.y, bottom_left.z,	//vert data
		0.0f, 0.0f,																//uv data
		top_left.x, top_left.y, top_left.z,
		getDelta(bottom_left, top_left, u_axis) / uv_map_dimension, getDelta(bottom_left, top_left, v_axis) / uv_map_dimension,
		top_right.x, top_right.y, top_right.z,
		getDelta(bottom_left, top_right, u_axis) / uv_map_dimension, getDelta(bottom_left, top_right, v_axis) / uv_map_dimension,
		bottom_left.x, bottom_left.y, bottom_left.z,
		0.0f, 0.0f,
		top_right.x, top_right.y, top_right.z,
		getDelta(bottom_left, top_right, u_axis) / uv_map_dimension, getDelta(bottom_left, top_right, v_axis) / uv_map_dimension,
		bottom_right.x, bottom_right.y, bottom_right.z,
		getDelta(bottom_left, bottom_right, u_axis) / uv_map_dimension, getDelta(bottom_left, bottom_right, v_axis) / uv_map_dimension
	};

	return geometry_data;
}

//TODO templatize function
void offsetArtworks(vector<pair<int, shared_ptr<artwork> > > &art_vec, float space_between, float eye_level, float starting_z, bool x_only)
{
	float x_offset = 0.0f;
	float previous_width = 0.0f;
	int display_count = 0;

	for (auto i : art_vec)
	{
		//center on eye level, unless painting is within .5 of floor
		float y_offset = 0.0f;

		if (!x_only)
		{
			float min_distance_from_floor = .5f;
			if ((i.second->getData()->getHeight() * .67f) + min_distance_from_floor > eye_level)
				y_offset = (i.second->getData()->getHeight() / 2.0f) + min_distance_from_floor;

			else y_offset = eye_level - (i.second->getData()->getHeight() / 6.0f);
		}

		float buffer = (previous_width / 2.0f) + space_between + (i.second->getData()->getWidth() / 2.0f);
		x_offset += buffer;

		previous_width = i.second->getData()->getWidth();
		i.second->moveAbsolute(vec3(x_offset, y_offset, starting_z));
		display_count++;
	}
}

void addFrames(vector< shared_ptr<artwork> > &art_vec, shared_ptr<ogl_context> context, shared_ptr<ogl_camera> camera, string data_path)
{
	string matte_texture = data_path + "model_data\\white_matte.bmp";
	for (auto i : art_vec)
	{
		string frame_material_image_name;
		switch (jep::intRoll(0, 4))
		{
		case 0: frame_material_image_name = "frame_white.bmp"; break;
		case 1: frame_material_image_name = "frame_black.bmp"; break;
		case 2: frame_material_image_name = "frame_pine.bmp"; break;
		case 3: frame_material_image_name = "frame_bamboo.bmp"; break;
		case 4: frame_material_image_name = "frame_aluminum.bmp"; break;
		}

		string frame_texture = data_path + "model_data\\" + frame_material_image_name;

		float random_frame_width = jep::floatRoll(0.05f, .25f, 2);
		float random_matte_width = jep::floatRoll(0.05f, .25f, 2);
		shared_ptr<frame_model> generated_frame(new frame_model(
			i->getData()->getWidth(), i->getData()->getHeight(), context, frame_texture.c_str(), matte_texture.c_str(), random_frame_width));

		i->loadFrame(generated_frame);
	}
}

//TODO typdef vector of pairs
vector<pair<int, shared_ptr<artwork> > >::iterator sortArtVec(vector<pair<int, shared_ptr<artwork> > > &art_vec, sort_options sort, bool ascending)
{
	switch (sort)
	{
	case ARTIST_NAME:
		std::sort(art_vec.begin(), art_vec.end(), 
			[&](pair<int, shared_ptr<artwork> > first_work, pair<int, shared_ptr<artwork> > second_work)
		{
			string first_work_name = first_work.second->getData()->getArtistName();
			std::transform(first_work_name.begin(), first_work_name.end(), first_work_name.begin(), ::tolower);
			string second_work_name = second_work.second->getData()->getArtistName();
			std::transform(second_work_name.begin(), second_work_name.end(), second_work_name.begin(), ::tolower);
			return (ascending ? first_work_name < second_work_name : first_work_name > second_work_name);
		});
		break;

	case VALUE:
		std::sort(art_vec.begin(), art_vec.end(),
			[&](pair<int, shared_ptr<artwork> > first_work, pair<int, shared_ptr<artwork> > second_work)
		{ 
			return (ascending ? first_work.second->getValue() < second_work.second->getValue() :
								first_work.second->getValue() > second_work.second->getValue());
		});
		break;

	case TITLE:
		std::sort(art_vec.begin(), art_vec.end(),
			[&](pair<int, shared_ptr<artwork> > first_work, pair<int, shared_ptr<artwork> > second_work)
		{
			string first_work_title = first_work.second->getData()->getTitle();
			std::transform(first_work_title.begin(), first_work_title.end(), first_work_title.begin(), ::tolower);
			string second_work_title = second_work.second->getData()->getTitle();
			std::transform(second_work_title.begin(), second_work_title.end(), second_work_title.begin(), ::tolower);
			return (ascending ? first_work_title < second_work_title : first_work_title > second_work_title);
		});
		break;

	case RARITY:
		std::sort(art_vec.begin(), art_vec.end(),
			[&](pair<int, shared_ptr<artwork> > first_work, pair<int, shared_ptr<artwork> > second_work)
		{
			return (ascending ? first_work.second->getData()->getRarity() < second_work.second->getData()->getRarity() :
				first_work.second->getData()->getRarity() > second_work.second->getData()->getRarity());
		});
		break;

	case AREA:
		std::sort(art_vec.begin(), art_vec.end(),
			[&](pair<int, shared_ptr<artwork> > first_work, pair<int, shared_ptr<artwork> > second_work)
		{
			float first_work_area = first_work.second->getData()->getHeight() * first_work.second->getData()->getWidth();
			float second_work_area = second_work.second->getData()->getHeight() * second_work.second->getData()->getWidth();
			return (ascending ? first_work_area < second_work_area :
				first_work_area > second_work_area);
		});
		break;

	case DATE:
		std::sort(art_vec.begin(), art_vec.end(),
			[&](pair<int, shared_ptr<artwork> > first_work, pair<int, shared_ptr<artwork> > second_work)
		{
			return (ascending ? first_work.second->getData()->getDate() < second_work.second->getData()->getDate() :
				first_work.second->getData()->getDate() > second_work.second->getData()->getDate());
		});
		break;

	default: return art_vec.begin();
	}

	//ARTIST_NAME, GENRE, STYLE, MEDIUM, DATE, BASE_VALUE, VALUE, TITLE, AREA, HEIGHT, WIDTH, RARITY, PAINTING_ID, NO_SORT

	return art_vec.begin();
}

void printArtwork(const shared_ptr<artwork> &target)
{
	shared_ptr<artwork_data> data = target->getData();
	cout << data->getTitle() << " by " << data->getArtistName() << " (" << getDateString(data->getDate(), false) << ")" << endl;
	cout << "\t" << data->getHeight() * 100.0f << "cm x " << data->getWidth() * 100.0f << "cm" << endl;
	cout << "\tEstimated value: $" << data->getBaseValue().getNumberString(true, false, 2) << endl;
	cout << "\tRarity: " << stringFromRarity(data->getRarity()) << endl;
}

mat4 calcImageScale(const shared_ptr<artwork> &target, float width_max, float height_max)
{
	//overall dimensions provides height, width respectively
	float scale_for_x = width_max / target->getOverallDimensions().second;
	float scale_for_y = height_max / target->getOverallDimensions().first;

	return (scale_for_x < scale_for_y ?
		glm::scale(mat4(1.0f), vec3(scale_for_x, scale_for_x, scale_for_x)) :
		glm::scale(mat4(1.0f), vec3(scale_for_y, scale_for_y, scale_for_y))
		);
}

void makeHighlight(shared_ptr<artwork> target, float top_margin, float bottom_margin, float cell_width)
{
	float cell_height = 2.0 - top_margin - bottom_margin;
	float y_translate = (bottom_margin + (cell_height / 2.0f)) - 1.0f;

	mat4 initial(1.0f);
	mat4 scale = calcImageScale(target, cell_width, cell_height);
	mat4 translation = glm::translate(mat4(1.0f), vec3(0.0f, y_translate, 0.0f));
	mat4 position_matrix = translation * scale * initial;
	target->setModelMatrix(position_matrix);
}

//manipulates paintings to be viewed as thumbnails returns iterator to next starting point of the sequence
void makeThumbnails(vector<pair<int, shared_ptr<artwork> > > &art_vec, float margin_size, float cell_size)
{
	int items_to_display = art_vec.size();

	float cell_height = cell_size;
	float cell_width = cell_size;
	float cell_padding = cell_width * 0.05f;

	float max_painting_width = cell_width - (2 * cell_padding);
	float max_painting_height = cell_height - (2 * cell_padding);

	float y_translate = 1.0f - margin_size - (cell_height / 2.0f);

	float total_width = cell_width * (float)items_to_display;
	float initial_x_offset = (total_width / -2.0f) + (cell_width / 2.0f);

	int item_counter = 0;
	for (auto i : art_vec)
	{
		mat4 scale = calcImageScale(i.second, max_painting_width, max_painting_height);

		float x_offset = initial_x_offset + (item_counter * cell_width);
		mat4 translation = glm::translate(mat4(1.0f), vec3(x_offset, y_translate * -1.0f, 0.0f));
		mat4 position_matrix = translation * scale;

		i.second->setModelMatrix(position_matrix);

		item_counter++;
	}
}

vector<pair<int, shared_ptr<artwork> > >::const_iterator findChunkFirst(
	vector<pair<int, shared_ptr<artwork> > >::const_iterator first,
	const vector<pair<int, shared_ptr<artwork> > > &art_vec, int chunk_size, bool forward)
{
	int distance_to_end = std::distance(first, art_vec.end());
	int distance_to_begin = std::distance(art_vec.begin(), first);

	switch (forward)
	{
	case true:
		if (distance_to_end <= chunk_size)
			return first;

		for (int i = 0; i < chunk_size; i++)
			first++;

		return first;

	case false:
		if (distance_to_begin <= chunk_size)
			return art_vec.begin();

		for (int i = 0; i < chunk_size; i++)
			first--;

		return first;
	}
}

vector<pair<int, shared_ptr<artwork> > >::const_iterator findChunkLast(
	vector<pair<int, shared_ptr<artwork> > >::const_iterator first,
	const vector<pair<int, shared_ptr<artwork> > > &art_vec, int chunk_size)
{
	int distance_to_end = std::distance(first, art_vec.end());
	if (distance_to_end <= chunk_size)
		return art_vec.end() - 1;

	return first + (chunk_size - 1);
}

vector<pair<int, shared_ptr<artwork> > >::const_iterator findChunkEnd(
	vector<pair<int, shared_ptr<artwork> > >::const_iterator first,
	const vector<pair<int, shared_ptr<artwork> > > &art_vec, int chunk_size)
{
	int distance_to_end = std::distance(first, art_vec.end());
	if (distance_to_end <= chunk_size)
		return art_vec.end();

	return first + chunk_size;
}

pair<vec3, vec3> getRayFromCursorPosition(shared_ptr<key_handler> &keys, const shared_ptr<ogl_camera> &camera)
{
	mat4 test_view_matrix = glm::lookAt(
		glm::vec3(1.0f, 2.0f, 3.0f),	//position of camera
		glm::vec3(1.0f, 2.0f, -3.0f),			//position of focal point
		glm::vec3(0, 1, 0));								//up axis

	mat4 test_projection_matrix = glm::perspective(45.0f, 1.6f, .1f, 150.0f);
	mat4 test_model_matrix = glm::translate(mat4(1.0f), vec3(10.0f, 20.0f, 30.0f));

	mat4 i_test_view_matrix = glm::inverse(test_view_matrix);
	mat4 i_test_projection_matrix = glm::inverse(test_projection_matrix);
	mat4 i_test_model_matrix = glm::inverse(test_model_matrix);

	vec4 point1(0.5f, -0.75f, 1.0f, 1.0f);
	vec4 point2(0.5f, -0.75f, -1.0f, 1.0f);
	cout << "------------test" << endl;
	cout << "original point 1: " << point1.x << ", " << point1.y << ", " << point1.z << endl;
	cout << "original point 2: " << point2.x << ", " << point2.y << ", " << point2.z << endl;
	point1 = test_model_matrix * test_view_matrix * test_projection_matrix * point1;
	point2 = test_model_matrix * test_view_matrix * test_projection_matrix * point2;
	cout << "position_matrix point 1: " << point1.x << ", " << point1.y << ", " << point1.z << endl;
	cout << "position_matrix point 2: " << point2.x << ", " << point2.y << ", " << point2.z << endl;
	point1 = i_test_projection_matrix * i_test_view_matrix * i_test_model_matrix * point1;
	point2 = i_test_projection_matrix * i_test_view_matrix * i_test_model_matrix * point2;
	cout << "inverse_position_matrix point 1: " << point1.x << ", " << point1.y << ", " << point1.z << endl;
	cout << "inverse_position_matrix point 2: " << point2.x << ", " << point2.y << ", " << point2.z << endl;
	cout << "------------test" << endl;

	vec2 cursor_position = keys->getCursorPosition();

	vec3 vector_first(camera->getPosition());

	vec4 ray_clip(cursor_position.x, cursor_position.y, -0.1f, 1.0f);
	mat4 projection_matrix = camera->getProjectionMatrix();
	mat4 inverted_projection_matrix = glm::inverse(projection_matrix);
	vec4 ray_eye = inverted_projection_matrix * ray_clip;
	ray_eye = vec4(ray_eye.x, ray_eye.y, -1.0, 0.0);

	mat4 view_matrix = camera->getViewMatrix();
	mat4 inverted_view_matrix = glm::inverse(view_matrix);
	vec3 ray_wor(inverted_view_matrix * ray_eye);
	// don't forget to normalise the vector at some point
	ray_wor = glm::normalize(ray_wor);

	ray_wor = vec3(glm::translate(mat4(1.0f), camera->getPosition()) * vec4(ray_wor, 1.0f));

	return pair<vec3, vec3>(vector_first, vec3(ray_wor));
}