#ifndef UTILITY_FUNCS_HPP
#define UTILITY_FUNCS_HPP

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

double lookupValue(rarity work_rarity)
{
	//placeholder function
	double min, max;
	switch (work_rarity)
	{
	case COMMON:
		min = 5.0f;
		max = 15.0f;
		break;
	case UNCOMMON:
		min = 15.0f;
		max = 45.0f;
		break;
	case RARE:
		min = 45.0f;
		max = 165.0f;
		break;
	case LEGENDARY:
		min = 645.0f;
		max = 2565.0f;
		break;
	case MASTERPIECE:
		min = 2565.0f;
		max = 10245.0f;
		break;
	}

	return jep::floatRoll(min, max, 1);
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

//this function takes a vector of instances and modifies their model matrices for proper display
void offsetArtworks(vector< shared_ptr<artwork_instance> > &art_vec, float eye_level, float starting_z)
{
	float x_offset = 0.0f;
	float previous_width = 0.0f;
	int display_count = 0;

	for (auto i : art_vec)
	{
		//center on eye level, unless painting is within .5 of floor
		float y_offset = 0.0f;
		float min_distance_from_floor = .5f;
		if ((i->getHeight() * .0067f) + min_distance_from_floor > eye_level)
			y_offset = (i->getHeight() / 200.0f) + min_distance_from_floor;

		else y_offset = eye_level - (i->getHeight() / 600.0f);

		float space_between = 2.0f;
		float buffer = (previous_width / 200.0f) + space_between + (i->getWidth() / 200.0f);
		if (display_count % 10 == 0)
		{
			x_offset = 0.0f;
			starting_z -= 4.0f;
			previous_width = 0.0f;
			buffer = 0.0f;
		}

		else
			x_offset += buffer;

		previous_width = i->getWidth();
		i->moveAbsolute(glm::translate(mat4(1.0f), vec3(x_offset, y_offset, starting_z)));
		display_count++;
	}
}

void addFrames(vector< shared_ptr<artwork_instance> > &art_vec, shared_ptr<ogl_context> context, shared_ptr<ogl_camera> camera, string data_path)
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
			i->getWidth() / 100.0f, i->getHeight() / 100.0f, context, camera, frame_texture.c_str(), matte_texture.c_str(), random_frame_width));

		i->loadFrame(generated_frame);
	}
}

#endif