#include "utility_funcs.h"
#include "artwork.h"
#include "player.h"

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
		max = bignum("25000");
		break;
	case UNCOMMON:
		min = bignum("25000");
		max = bignum("65000");
		break;
	case RARE:
		min = bignum("65000");
		max = bignum("225000");
		break;
	case LEGENDARY:
		min = bignum("225000");
		max = bignum("1505000");
		break;
	case MASTERPIECE:
		min = bignum("1505000");
		max = bignum("21985000");
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

string stringFromAttribute(artwork_attribute aa)
{
	switch (aa)
	{
	case BASE_XP_EARNED: return "Quality";
	case BASE_XP_DURATION: return "Accessibility";

	case NPC_AUCTIONEER_BASE: return "Auctioneer rating";
	case NPC_DEALER_BASE: return "Art Dealer rating";
	case NPC_COLLECTOR_BASE: return "Art Collector rating";
	case NPC_DONOR_BASE: return "Art Donor rating";
	case NPC_BENEFACTOR_BASE: return "Benefactor rating";
	case NPC_ENTHUSIAST_BASE: return "Enthusiast rating";
	case NPC_DESIGNER_BASE: return "Designer rating";
	case NPC_FORGER_BASE: return "Forger rating";
	case NPC_ART_EXPERT_BASE: return "Art Expert rating";
	case NPC_HISTORIAN_BASE: return "Art Historian rating";
	case NPC_PRESERVATIONIST_BASE: return "Preservationist rating";
	case NPC_MARKET_EXPERT_BASE: return "Market Expert rating";

	case ENTRY_FEE_REDUCTION_VISITORS: return "Visitor discount";
	case XP_FROM_SET_WORKS_INCREASE_VISITORS: return "Set bonus (visitors)";
	case XP_FROM_WORKS_INCREASE_VISITORS: return "Gallery bonus (visitors)";
	case XP_DURATION_FOR_SET_WORKS_DECREASE_VISITORS: return "Set accessibility bonus (visitors)";
	case XP_DURATION_FOR_WORKS_DECREASE_VISITORS: return "Gallery accessibility bonus (visitors)";

	case NPC_AUCTIONEER_BOOST: return "Auctioneer boost";
	case NPC_DEALER_BOOST: return "Art Dealer boost";
	case NPC_COLLECTOR_BOOST: return "Art Collector boost";
	case NPC_DONOR_BOOST: return "Art Donor boost";
	case NPC_BENEFACTOR_BOOST: return "Benefactor boost";
	case NPC_ENTHUSIAST_BOOST: return "Enthusiast boost";
	case NPC_DESIGNER_BOOST: return "Designer boost";
	case NPC_FORGER_BOOST: return "Forger boost";
	case NPC_ART_EXPERT_BOOST: return "Art Expert boost";
	case NPC_HISTORIAN_BOOST: return "Art Historian boost";
	case NPC_PRESERVATIONIST_BOOST: return "Preservationist boost";
	case NPC_MARKET_EXPERT_BOOST: return "Market Expert boost";

	case ENTRY_FEE_REDUCTION_MEMBERS: return "Member discount";
	case XP_FROM_SET_WORKS_INCREASE_MEMBERS: return "Set bonus (members)";
	case XP_FROM_WORKS_INCREASE_MEMBERS: return "Gallery bonus (members)";
	case XP_DURATION_FOR_SET_WORKS_DECREASE_MEMBERS: return "Set accessibility bonus (members)";
	case XP_DURATION_FOR_WORKS_DECREASE_MEMBERS: return "Gallery accessibility bonus (members)";

	case XP_GAIN_PER_VISITOR_INTERACTION: return "Owner interaction bonus";
	case MONEY_GAIN_PER_VISITOR_INTERACTION: return "Owner interaction profit";

	default: return "undefined attribute";
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

//TODO revise to automatically detect u and v vectors instead of passing 'x', 'y', or 'z'
//TODO detect normal direction and include
vector<float> generateFrameVertices(vec3 bottom_left, vec3 top_left, vec3 top_right, vec3 bottom_right,
		float uv_map_dimension, char u_axis, char v_axis, vector<unsigned short> &indices)
{
	vec3 normal_rotation_axis;
	vec3 calibration_point;
	float rotation_angle = -90.0f;
	switch (v_axis)
	{
	case 'y': 
		normal_rotation_axis = vec3(0.0f, 1.0f, 0.0f);
		if (bottom_left.y > top_left.y)
			rotation_angle *= -1.0f;
		break;

	case 'z':
		normal_rotation_axis = vec3(0.0f, 0.0f,1.0f);
		if (bottom_left.z > top_left.z)
			rotation_angle *= -1.0f;
		break;

	case 'x':
		normal_rotation_axis = vec3(1.0f, 0.0f, 0.0f);
		if (bottom_left.x > top_left.x)
			rotation_angle *= -1.0f;
		break;
	}

	switch (u_axis)
	{
	case 'x':
		calibration_point = vec3(1.0f, 0.0f, 0.0f);
		if (bottom_left.x > bottom_left.x)
			calibration_point *= -1.0f;
		break;

	case 'y':
		calibration_point = vec3(0.0f, 1.0f, 0.0f);
		if (bottom_left.y > bottom_left.y)
			calibration_point *= -1.0f;
		break;

	case 'z':
		calibration_point = vec3(0.0f, 0.0f, 1.0f);
		if (bottom_left.z > bottom_left.z)
			calibration_point *= -1.0f;
		break;
	}

	vec3 normal_direction = vec3(glm::rotate(mat4(1.0f), rotation_angle, normal_rotation_axis) * vec4(calibration_point, 1.0f));
	normal_direction = glm::normalize(vec3(normal_direction));

	normal_direction.x = (jep::floatsAreEqual(normal_direction.x, 0.0f) ? 0.0f : normal_direction.x);
	normal_direction.y = (jep::floatsAreEqual(normal_direction.y, 0.0f) ? 0.0f : normal_direction.y);
	normal_direction.z = (jep::floatsAreEqual(normal_direction.z, 0.0f) ? 0.0f : normal_direction.z);

	vector<float> geometry_data = {
		bottom_left.x, bottom_left.y, bottom_left.z,					//vert data
		0.0f, 0.0f,														//uv data
		normal_direction.x, normal_direction.y, normal_direction.z,		//normal

		top_left.x, top_left.y, top_left.z,
		getDelta(bottom_left, top_left, u_axis) / uv_map_dimension, getDelta(bottom_left, top_left, v_axis) / uv_map_dimension,
		normal_direction.x, normal_direction.y, normal_direction.z,

		top_right.x, top_right.y, top_right.z,
		getDelta(bottom_left, top_right, u_axis) / uv_map_dimension, getDelta(bottom_left, top_right, v_axis) / uv_map_dimension,
		normal_direction.x, normal_direction.y, normal_direction.z,

		bottom_right.x, bottom_right.y, bottom_right.z,
		getDelta(bottom_left, bottom_right, u_axis) / uv_map_dimension, getDelta(bottom_left, bottom_right, v_axis) / uv_map_dimension,
		normal_direction.x, normal_direction.y, normal_direction.z,
	};

	vector<unsigned short> index_pattern = { 0, 1, 2, 2, 3, 0 };

	int next_index = 0;

	for (const auto &i : indices)
		if (i > next_index)
			next_index = i;

	if (indices.size() != 0)
		next_index += 1;

	indices.push_back(next_index);
	indices.push_back(next_index + 1);
	indices.push_back(next_index + 2);
	indices.push_back(next_index + 2);
	indices.push_back(next_index + 3);
	indices.push_back(next_index);

	return geometry_data;
}

//TODO templatize function
void offsetArtworks(vector<pair<int, shared_ptr<artwork> > > &art_vec, float space_between, float eye_level, float starting_z, bool x_only)
{
	float x_offset = 0.0f;
	float previous_width = 0.0f;
	int display_count = 0;

	for (const auto &i : art_vec)
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

//TODO typdef vector of pairs
vector< shared_ptr<artwork> >::iterator sortArtVec(vector< shared_ptr<artwork> > &art_vec, sort_option sort, const shared_ptr<player> &current_player, bool ascending)
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

	case IN_GALLERY:
		std::sort(art_vec.begin(), art_vec.end(),
			[&](shared_ptr<artwork> first_work, shared_ptr<artwork> second_work)
		{
			return (ascending ? current_player->isOnDisplay(first_work) || !current_player->isOnDisplay(second_work) :
				!current_player->isOnDisplay(first_work) || current_player->isOnDisplay(second_work));
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
	for (const auto &i : art_vec)
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
	for (const auto &i : art_vec)
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
	for (const auto &i : select_surfaces)
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

vector< vector<vec3> > getTriangles(const vector<float> &vec_vertices, const vector<unsigned short> &vertex_indices, int offset, int stride)
{
	map<unsigned short, vector<float> > vertex_map;
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
	for (const auto &i : triangles)
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

	for (const auto &i : edges)
	{
		if (std::find(shared_edges.begin(), shared_edges.end(), i.first) != shared_edges.end())
			continue;

		pair<vec3, vec3> i_edge = i.second;

		bool unique = true;

		for (const auto &j : edges)
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

	for (const auto &i : unique_edges)
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

void highlight(const shared_ptr<ogl_context> &context, const shared_ptr<ogl_camera> &camera)
{
	glUniform1f(context->getShaderGLint("dim_factor"), 1.5f);
}

void highlightSubtle(const shared_ptr<ogl_context> &context, const shared_ptr<ogl_camera> &camera)
{
	glUniform1f(context->getShaderGLint("dim_factor"), 1.2f);
}

void fullBrightness(const shared_ptr<ogl_context> &context, const shared_ptr<ogl_camera> &camera)
{
	glUniform1f(context->getShaderGLint("dim_factor"), 1.0f);
}

template <typename T>
auto renderFunction(const shared_ptr<ogl_context> &context, const shared_ptr<ogl_camera> &camera, const shared_ptr<T> &passed) -> decltype(shared_ptr<T>)
{


}

//TODO load textures from the directory automatically
void loadTextures(shared_ptr<texture_handler> &textures)
{
	textures->addTexture("act.bmp");
	textures->addTexture("brick.bmp");
	textures->addTexture("carpet_blue.bmp");
	textures->addTexture("carpet_gray.bmp");
	textures->addTexture("carpet_green.bmp");
	textures->addTexture("carpet_orange.bmp");
	textures->addTexture("carpet_purple.bmp");
	textures->addTexture("concrete.bmp");
	textures->addTexture("dark_wood.bmp");
	textures->addTexture("frame_aluminum.bmp");
	textures->addTexture("frame_bamboo.bmp");
	textures->addTexture("frame_black.bmp");
	textures->addTexture("frame_pine.bmp");
	textures->addTexture("frame_white.bmp");
	textures->addTexture("hardwood.bmp");
	textures->addTexture("plaster.bmp");
	textures->addTexture("poured_concrete.bmp");
	textures->addTexture("white_matte.bmp");
	textures->addTexture("text_template.bmp");
	textures->addTexture("plaster_green.bmp");
	textures->addTexture("plaster_dark_green.bmp");
	textures->addTexture("plaster_blue.bmp");
	textures->addTexture("plaster_red.bmp");
	textures->addTexture("plaster_yellow.bmp");
	textures->addTexture("plaster_orange.bmp");
	textures->addTexture("plaster_purple.bmp");
	textures->addTexture("plaster_black.bmp");
	textures->addTexture("plaster_navy.bmp");
	textures->addTexture("wall_covering_01.bmp");
	textures->addTexture("wall_covering_02.bmp");
	textures->addTexture("wall_covering_03.bmp");
	textures->addTexture("wall_covering_04.bmp");
	textures->addTexture("wall_covering_05.bmp");
	textures->addTexture("wall_covering_06.bmp");
	textures->addTexture("wall_covering_07.bmp");
	textures->addTexture("wall_covering_08.bmp");
	textures->addTexture("wall_covering_09.bmp");
	textures->addTexture("wall_covering_10.bmp");
	textures->addTexture("wall_covering_11.bmp");
	textures->addTexture("wall_covering_12.bmp");
	textures->addTexture("wall_covering_13.bmp");
	textures->addTexture("wall_covering_14.bmp");
	textures->addTexture("old_brick_white.bmp");
	textures->addTexture("brick_white.bmp");
	textures->addTexture("brick_black.bmp");
}

void readConfigFile(string &data_path, string &username)
{
	std::ifstream config_in;

	config_in.open("settings.config", std::ifstream::in);

	if (config_in.good())
	{
		string data_path_line;
		std::getline(config_in, data_path_line, '\n');
		data_path.clear();
		data_path = data_path_line.substr(11);

		string username_line;
		std::getline(config_in, username_line, '\n');
		username.clear();
		username = username_line.substr(10);
	}
	
	else
	{
		std::ofstream config_out("settings.config");
		config_out << "DATA PATH: .\\" << endl;
		config_out << "USERNAME: default_user" << endl;

		data_path = ".\\";
		username = "default_user";
	}

	config_in.close();
}

bool fileExists(string filename)
{
	const char *c_name = filename.c_str();
	std::ifstream infile(c_name);
	return infile.good();
}

pair<float, float> getAttributeMinMax(artwork_attribute aa)
{
	switch (aa)
	{
	case NPC_AUCTIONEER_BASE:
	case NPC_DEALER_BASE:
	case NPC_COLLECTOR_BASE:
	case NPC_DONOR_BASE:
	case NPC_BENEFACTOR_BASE:
	case NPC_ENTHUSIAST_BASE:
	case NPC_DESIGNER_BASE:
	case NPC_FORGER_BASE:
	case NPC_ART_EXPERT_BASE:
	case NPC_HISTORIAN_BASE:
	case NPC_PRESERVATIONIST_BASE:
	case NPC_MARKET_EXPERT_BASE: return pair<float, float>(0.0002f, 0.001f);

	case NPC_AUCTIONEER_BOOST:
	case NPC_DEALER_BOOST:
	case NPC_COLLECTOR_BOOST:
	case NPC_DONOR_BOOST:
	case NPC_BENEFACTOR_BOOST:
	case NPC_ENTHUSIAST_BOOST:
	case NPC_DESIGNER_BOOST:
	case NPC_FORGER_BOOST:
	case NPC_ART_EXPERT_BOOST:
	case NPC_HISTORIAN_BOOST:
	case NPC_PRESERVATIONIST_BOOST:
	case NPC_MARKET_EXPERT_BOOST: return pair<float, float>(0.001f, 0.002f);

	case BASE_XP_EARNED: return pair<float, float>(10.0f, 20.0f);
	case BASE_XP_DURATION: return pair<float, float>(30.0f, 60.0f);
	case ENTRY_FEE_REDUCTION_VISITORS: return pair<float, float>(0.002f, 0.008f);
	case XP_FROM_SET_WORKS_INCREASE_VISITORS: return pair<float, float>(0.006f, 0.012f);
	case XP_FROM_WORKS_INCREASE_VISITORS: return pair<float, float>(0.002f, 0.008f);
	case XP_DURATION_FOR_SET_WORKS_DECREASE_VISITORS: return pair<float, float>(0.006f, 0.012f);
	case XP_DURATION_FOR_WORKS_DECREASE_VISITORS: return pair<float, float>(0.002f, 0.008f);
	case ENTRY_FEE_REDUCTION_MEMBERS: return pair<float, float>(0.006f, 0.012f);
	case XP_FROM_SET_WORKS_INCREASE_MEMBERS: return pair<float, float>(0.008f, 0.016f);
	case XP_FROM_WORKS_INCREASE_MEMBERS: return pair<float, float>(0.006f, 0.012f);
	case XP_DURATION_FOR_SET_WORKS_DECREASE_MEMBERS: return pair<float, float>(0.008f, 0.016f);
	case XP_DURATION_FOR_WORKS_DECREASE_MEMBERS: return pair<float, float>(0.006f, 0.012f);
	case XP_GAIN_PER_VISITOR_INTERACTION: return pair<float, float>(2.0f, 8.0f);
	case MONEY_GAIN_PER_VISITOR_INTERACTION: return pair<float, float>(2.0f, 8.0f);

	default: return pair<float, float>(0.0f, 0.0f);
	}
}

float getAttributeRating(artwork_attribute aa, float actual_rating)
{
	pair<float, float> minmax = getAttributeMinMax(aa);
	float delta = actual_rating - minmax.first;
	float range = minmax.second - minmax.first;
	float rating = (delta / range);
	return rating;
}

bool attributeIsPrimary(artwork_attribute aa)
{
	switch (aa)
	{
	case ENTRY_FEE_REDUCTION_VISITORS:
	case XP_FROM_SET_WORKS_INCREASE_VISITORS:
	case XP_FROM_WORKS_INCREASE_VISITORS:
	case XP_DURATION_FOR_SET_WORKS_DECREASE_VISITORS:
	case XP_DURATION_FOR_WORKS_DECREASE_VISITORS:
	case NPC_AUCTIONEER_BOOST:
	case NPC_DEALER_BOOST:
	case NPC_COLLECTOR_BOOST:
	case NPC_DONOR_BOOST:
	case NPC_BENEFACTOR_BOOST:
	case NPC_ENTHUSIAST_BOOST:
	case NPC_DESIGNER_BOOST:
	case NPC_FORGER_BOOST:
	case NPC_ART_EXPERT_BOOST:
	case NPC_HISTORIAN_BOOST:
	case NPC_PRESERVATIONIST_BOOST:
	case NPC_MARKET_EXPERT_BOOST: return true;
	default: return false;
	}
}

bool attributeIsSecondary(artwork_attribute aa)
{
	switch (aa)
	{
	case ENTRY_FEE_REDUCTION_MEMBERS:
	case XP_FROM_SET_WORKS_INCREASE_MEMBERS:
	case XP_FROM_WORKS_INCREASE_MEMBERS:
	case XP_DURATION_FOR_SET_WORKS_DECREASE_MEMBERS:
	case XP_DURATION_FOR_WORKS_DECREASE_MEMBERS:
	case XP_GAIN_PER_VISITOR_INTERACTION:
	case MONEY_GAIN_PER_VISITOR_INTERACTION: return true;
	default: return false;
	}
}

vec4 getRarityColor(rarity r)
{
	switch (r)
	{
	case COMMON: return V4C_GREEN;
	case UNCOMMON: return V4C_BLUE;
	case RARE: return V4C_YELLOW;
	case LEGENDARY: return V4C_ORANGE;
	case MASTERPIECE: return V4C_CYAN;
	defualt: return V4C_BLACK;
	}
}

vec4 getCrateColor(crate_quality cq)
{
	switch (cq)
	{
	case BRONZE: return V4C_BRONZE;
	case SILVER: return V4C_SILVER;
	case GOLD: return V4C_GOLD;
	case PLATINUM: return V4C_PLATINUM;
	case DIAMOND: return V4C_DIAMOND;
	default: return V4C_BLACK;
	}
}


