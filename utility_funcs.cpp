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
vector< shared_ptr<artwork> >::iterator sortArtVec(vector< shared_ptr<artwork> > &art_vec, sort_options sort, bool ascending)
{
	switch (sort)
	{
	case ARTIST_NAME:
		std::sort(art_vec.begin(), art_vec.end(), 
			[&](shared_ptr<artwork> first_work, shared_ptr<artwork> second_work)
		{
			string first_work_name = first_work->getData()->getArtistName();
			std::transform(first_work_name.begin(), first_work_name.end(), first_work_name.begin(), ::tolower);
			string second_work_name = second_work->getData()->getArtistName();
			std::transform(second_work_name.begin(), second_work_name.end(), second_work_name.begin(), ::tolower);
			return (ascending ? first_work_name < second_work_name : first_work_name > second_work_name);
		});
		break;

	case VALUE:
		std::sort(art_vec.begin(), art_vec.end(),
			[&](shared_ptr<artwork> first_work, shared_ptr<artwork> second_work)
		{ 
			return (ascending ? first_work->getValue() < second_work->getValue() :
								first_work->getValue() > second_work->getValue());
		});
		break;

	case TITLE:
		std::sort(art_vec.begin(), art_vec.end(),
			[&](shared_ptr<artwork> first_work, shared_ptr<artwork> second_work)
		{
			string first_work_title = first_work->getData()->getTitle();
			std::transform(first_work_title.begin(), first_work_title.end(), first_work_title.begin(), ::tolower);
			string second_work_title = second_work->getData()->getTitle();
			std::transform(second_work_title.begin(), second_work_title.end(), second_work_title.begin(), ::tolower);
			return (ascending ? first_work_title < second_work_title : first_work_title > second_work_title);
		});
		break;

	case RARITY:
		std::sort(art_vec.begin(), art_vec.end(),
			[&](shared_ptr<artwork> first_work, shared_ptr<artwork> second_work)
		{
			return (ascending ? first_work->getData()->getRarity() < second_work->getData()->getRarity() :
				first_work->getData()->getRarity() > second_work->getData()->getRarity());
		});
		break;

	case AREA:
		std::sort(art_vec.begin(), art_vec.end(),
			[&](shared_ptr<artwork> first_work, shared_ptr<artwork> second_work)
		{
			float first_work_area = first_work->getData()->getHeight() * first_work->getData()->getWidth();
			float second_work_area = second_work->getData()->getHeight() * second_work->getData()->getWidth();
			return (ascending ? first_work_area < second_work_area :
				first_work_area > second_work_area);
		});
		break;

	case DATE:
		std::sort(art_vec.begin(), art_vec.end(),
			[&](shared_ptr<artwork> first_work, shared_ptr<artwork> second_work)
		{
			return (ascending ? first_work->getData()->getDate() < second_work->getData()->getDate() :
				first_work->getData()->getDate() > second_work->getData()->getDate());
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
	float scale_for_x = width_max / target->getOverallDimensions().x;
	float scale_for_y = height_max / target->getOverallDimensions().y;

	return (scale_for_x < scale_for_y ?
		glm::scale(mat4(1.0f), vec3(scale_for_x, scale_for_x, scale_for_x)) :
		glm::scale(mat4(1.0f), vec3(scale_for_y, scale_for_y, scale_for_y))
		);
}

//TODO create thumbnail_table class to handle thumbnails, include adding/removing
//manipulates paintings to be viewed as thumbnails returns iterator to next starting point of the sequence
map<int, mat4> getThumbnailMatrixMap(const shared_ptr<ogl_context> &context, const vector<shared_ptr<artwork> > &art_vec,
	float margin_size, float cell_size, float cell_padding_factor)
{
	map<int, mat4> matrix_map;

	int items_to_display = art_vec.size();

	float cell_height = cell_size;
	float cell_width = cell_size / context->getAspectRatio();
	float cell_padding = cell_width * cell_padding_factor;

	float max_painting_width = cell_width - (2 * cell_padding);
	float max_painting_height = cell_height - (2 * cell_padding);

	float y_translate = 1.0f - margin_size - (cell_height / 2.0f);

	float total_width = cell_width * (float)items_to_display;
	//adding half-width because the work is initially centered on y-axis
	float initial_x_offset = (total_width / -2.0f) + (cell_width / 2.0f);

	int item_counter = 0;
	for (auto i : art_vec)
	{
		//TODO reduce confusion of scaling funciton not taking aspect-ratio-modified size
		//must take cell size since aspect ratio is not accounted for in scaling
		mat4 scale = calcImageScale(i, cell_size - (2 * cell_padding), max_painting_height);

		float x_offset = initial_x_offset + (item_counter * cell_width);
		mat4 translation = glm::translate(mat4(1.0f), vec3(x_offset, y_translate * -1.0f, 0.0f));
		mat4 position_matrix = translation * scale;

		matrix_map.insert(pair<int, mat4>(item_counter++, position_matrix));
	}

	return matrix_map;
}

vector<vec2> getThumbnailLocations(const shared_ptr<ogl_context> &context, int thumbnail_count, float bottom_margin, float thumbnail_height)
{
	vector<vec2> centerpoints;

	float cell_width = 2.0f / (float)thumbnail_count;
	float y_offset = (1.0f - bottom_margin - (thumbnail_height / 2.0f)) * -1.0f;
	float initial_x_offset = -1.0f + (cell_width / 2.0f);

	for (int i = 0; i < thumbnail_count; i++)
	{
		float x_offset = initial_x_offset + ((float)i * cell_width);
		centerpoints.push_back(vec2(x_offset, y_offset));
	}

	return centerpoints;
}

map<int, mat4> getHighlightMatrixMap(const shared_ptr<ogl_context> &context, const vector<shared_ptr<artwork> > &art_vec,
	const vec2 &screen_position, float cell_size)
{
	map<int, mat4> matrix_map;

	int items_to_display = art_vec.size();

	float max_painting_height = cell_size;
	float max_painting_width = cell_size / context->getAspectRatio();

	float y_translate = screen_position.y;
	float x_offset = screen_position.x;

	int item_counter = 0;
	for (auto i : art_vec)
	{
		//TODO reduce confusion of scaling funciton not taking aspect-ratio-modified size
		//must take cell size since aspect ratio is not accounted for in scaling
		mat4 scale = calcImageScale(i, cell_size, max_painting_height);
		mat4 translation = glm::translate(mat4(1.0f), vec3(x_offset, y_translate, 0.0f));
		mat4 position_matrix = translation * scale;

		matrix_map.insert(pair<int, mat4>(item_counter++, position_matrix));
	}

	return matrix_map;
}

vector< shared_ptr<artwork> >::const_iterator findChunkFirst(
	vector< shared_ptr<artwork> >::const_iterator first,
	vector< shared_ptr<artwork> > &art_vec, int chunk_size, bool forward)
{
	int distance_to_end = std::distance(first, art_vec.cend());
	int distance_to_begin = std::distance(art_vec.cbegin(), first);

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

vector< shared_ptr<artwork> >::const_iterator findChunkLast(
	vector< shared_ptr<artwork> >::const_iterator first,
	const vector< shared_ptr<artwork> > &art_vec, int chunk_size)
{
	int distance_to_end = std::distance(first, art_vec.end());
	if (distance_to_end <= chunk_size)
		return art_vec.end() - 1;

	return first + (chunk_size - 1);
}

vector< shared_ptr<artwork> >::const_iterator findChunkEnd(
	vector< shared_ptr<artwork> >::const_iterator first,
	const vector< shared_ptr<artwork> > &art_vec, int chunk_size)
{
	int distance_to_end = std::distance(first, art_vec.end());
	if (distance_to_end <= chunk_size)
		return art_vec.end();

	return first + chunk_size;
}

pair<vec3, vec3> getRayFromCursorPosition(shared_ptr<key_handler> &keys, const shared_ptr<ogl_camera> &camera)
{
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

bool paintingSelected(shared_ptr<key_handler> &keys, const shared_ptr<ogl_camera> &camera, const shared_ptr<artwork> &art, float &scale)
{	
	pair<vec3, vec3> ray(getRayFromCursorPosition(keys, camera));

	mat4 model_matrix = art->getModelMatrix();
	mat4 inverse_model_matrix = glm::inverse(model_matrix);

	ray.first = vec3(inverse_model_matrix * vec4(ray.first, 1.0f));
	ray.second = vec3(inverse_model_matrix * vec4(ray.second, 1.0f));

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
			scale = result.z;
			return true;
		}
	}

	scale = 0.0f;
	return false;
}

vector< vector<vec3> > getTriangles(const vector<float> &vec_vertices, int offset, int stride)
{
	vector<vec3> vertices;
	//stride output from obj parser is in bit size, not count
	int stride_count = stride / sizeof(float);
	//for each stride there will be 3 extracted vertices
	vertices.reserve((vec_vertices.size() / stride_count) * 3);

	vector<float> point_found;
	for (int i = offset; i < vec_vertices.size(); i++)
	{
		int local_index = i % stride_count;
		//skip if local index is before or after target range
		if (local_index < offset || local_index >= offset + 3)
			continue;

		point_found.push_back(vec_vertices.at(i));
		if (point_found.size() == 3)
		{
			vertices.push_back(vec3(point_found.at(0), point_found.at(1), point_found.at(2)));
			point_found.clear();
		}
	}

	vector< vector<vec3> > triangles;
	triangles.reserve(vertices.size() / 3);

	vector<vec3> triangle_found;
	for (int i = 0; i < vertices.size(); i++)
	{
		triangle_found.push_back(vertices.at(i));

		if (triangle_found.size() == 3)
		{
			triangles.push_back(triangle_found);
			triangle_found.clear();
		}
	}

	return triangles;
}

vector< pair<vec3, vec3> > getOuterEdges(const vector< vector<vec3> > &triangles)
{
	map< int, pair<vec3, vec3> > edges;
	int edge_counter = 0;

	//for each side of each triangle
	for (auto i : triangles)
	{
		pair<vec3, vec3> edge1(i.at(0), i.at(1));
		pair<vec3, vec3> edge2(i.at(1), i.at(2));
		pair<vec3, vec3> edge3(i.at(2), i.at(0));

		edges.insert(pair<int, pair<vec3, vec3> >(edge_counter++, edge1));
		edges.insert(pair<int, pair<vec3, vec3> >(edge_counter++, edge2));
		edges.insert(pair<int, pair<vec3, vec3> >(edge_counter++, edge3));
	}

	vector<int> unique_edges;
	vector<int> shared_edges;

	for (auto i : edges)
	{
		if (std::find(shared_edges.begin(), shared_edges.end(), i.first) != shared_edges.end())
			continue;

		pair<vec3, vec3> i_edge = i.second;

		bool unique = true;

		for (auto j : edges)
		{
			//skip if the edge has already been confirmed as unique or shared
			if (std::find(shared_edges.begin(), shared_edges.end(), j.first) != shared_edges.end())
				continue;

			if (std::find(unique_edges.begin(), unique_edges.end(), j.first) != unique_edges.end())
				continue;

			pair<vec3, vec3> j_edge = j.second;

			//if each edge has similar points, in any order, add to shared edges
			if ((i_edge.second == j_edge.first && i_edge.second == j_edge.second) || 
				(i_edge.first == j_edge.second && i_edge.second == j_edge.first))
			{
				shared_edges.push_back(i.first);
				shared_edges.push_back(j.first);
				unique = false;
				break;
			}
		}

		if (unique)
			unique_edges.push_back(i.first);
	}

	vector< pair<vec3, vec3> > outer_edges;

	for (auto i : unique_edges)
		outer_edges.push_back(edges.at(i));

	return outer_edges;
}

//function applies to vertical walls only, based on first normal
float getNormalRotation(const vector<float> &vec_vertices, int normal_offset, int stride)
{
	vector<float> normal_vertex;
	int normal_count = normal_offset / sizeof(float);
	for (int i = normal_count; i < normal_count + 3; i++)
		normal_vertex.push_back(vec_vertices.at(i));

	vec3 normal_vec(normal_vertex.at(0), normal_vertex.at(1), normal_vertex.at(2));

	//TODO experiment with left-handed vs. right-handed
	//passes z location as y-value of second point because surface should be vertical
	float normal_angle = jep::getLineAngle(vec2(0.0f, 0.0f), vec2(normal_vec.x, normal_vec.z), true);

	return normal_angle;
}

