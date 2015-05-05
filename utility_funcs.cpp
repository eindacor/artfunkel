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
void offsetArtworks(vector<pair<int, shared_ptr<artwork_instance> > > &art_vec, float space_between, float eye_level, float starting_z, bool x_only)
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
			if ((i.second->getHeight() * .67f) + min_distance_from_floor > eye_level)
				y_offset = (i.second->getHeight() / 2.0f) + min_distance_from_floor;

			else y_offset = eye_level - (i.second->getHeight() / 6.0f);
		}

		float buffer = (previous_width / 2.0f) + space_between + (i.second->getWidth() / 2.0f);
		x_offset += buffer;

		//code below was for wrapping paintings to a new line after 10 were displayed
		/*
		if (display_count % 10 == 0 && display_count != 0)
		{
			x_offset = 0.0f;
			starting_z -= 4.0f;
			previous_width = 0.0f;
			buffer = 0.0f;
		}

		else
			x_offset += buffer;
		*/

		previous_width = i.second->getWidth();
		i.second->moveAbsolute(vec3(x_offset, y_offset, starting_z));
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
			i->getWidth(), i->getHeight(), context, frame_texture.c_str(), matte_texture.c_str(), random_frame_width));

		i->loadFrame(generated_frame);
	}
}

//TODO typdef vector of pairs
vector<pair<int, shared_ptr<artwork_instance> > >::iterator sortArtVec(vector<pair<int, shared_ptr<artwork_instance> > > &art_vec, sort_options sort, bool ascending)
{
	switch (sort)
	{
	case ARTIST_NAME:
		std::sort(art_vec.begin(), art_vec.end(), 
			[&](pair<int, shared_ptr<artwork_instance> > first_work, pair<int, shared_ptr<artwork_instance> > second_work)
		{
			string first_work_name = first_work.second->getArtistName();
			std::transform(first_work_name.begin(), first_work_name.end(), first_work_name.begin(), ::tolower);
			string second_work_name = second_work.second->getArtistName();
			std::transform(second_work_name.begin(), second_work_name.end(), second_work_name.begin(), ::tolower);
			return (ascending ? first_work_name < second_work_name : first_work_name > second_work_name);
		});
		break;

	case VALUE:
		std::sort(art_vec.begin(), art_vec.end(),
			[&](pair<int, shared_ptr<artwork_instance> > first_work, pair<int, shared_ptr<artwork_instance> > second_work)
		{ 
			return (ascending ? first_work.second->getValue() < second_work.second->getValue() :
								first_work.second->getValue() > second_work.second->getValue());
		});
		break;

	case TITLE:
		std::sort(art_vec.begin(), art_vec.end(),
			[&](pair<int, shared_ptr<artwork_instance> > first_work, pair<int, shared_ptr<artwork_instance> > second_work)
		{
			string first_work_title = first_work.second->getTitle();
			std::transform(first_work_title.begin(), first_work_title.end(), first_work_title.begin(), ::tolower);
			string second_work_title = second_work.second->getTitle();
			std::transform(second_work_title.begin(), second_work_title.end(), second_work_title.begin(), ::tolower);
			return (ascending ? first_work_title < second_work_title : first_work_title > second_work_title);
		});
		break;

	case RARITY:
		std::sort(art_vec.begin(), art_vec.end(),
			[&](pair<int, shared_ptr<artwork_instance> > first_work, pair<int, shared_ptr<artwork_instance> > second_work)
		{
			return (ascending ? first_work.second->getRarity() < second_work.second->getRarity() :
				first_work.second->getRarity() > second_work.second->getRarity());
		});
		break;

	case AREA:
		std::sort(art_vec.begin(), art_vec.end(),
			[&](pair<int, shared_ptr<artwork_instance> > first_work, pair<int, shared_ptr<artwork_instance> > second_work)
		{
			float first_work_area = first_work.second->getHeight() * first_work.second->getWidth();
			float second_work_area = second_work.second->getHeight() * second_work.second->getWidth();
			return (ascending ? first_work_area < second_work_area :
				first_work_area > second_work_area);
		});
		break;

	case DATE:
		std::sort(art_vec.begin(), art_vec.end(),
			[&](pair<int, shared_ptr<artwork_instance> > first_work, pair<int, shared_ptr<artwork_instance> > second_work)
		{
			return (ascending ? first_work.second->getDate() < second_work.second->getDate() :
				first_work.second->getDate() > second_work.second->getDate());
		});
		break;

	default: return art_vec.begin();
	}

	//ARTIST_NAME, GENRE, STYLE, MEDIUM, DATE, BASE_VALUE, VALUE, TITLE, AREA, HEIGHT, WIDTH, RARITY, PAINTING_ID, NO_SORT

	return art_vec.begin();
}

void printArtworkInstance(const shared_ptr<artwork_instance> &target)
{
	cout << target->getTitle() << " by " << target->getArtistName() << " (" << getDateString(target->getDate(), false) << ")" << endl;
	cout << "\t" << target->getHeight() * 100.0f << "cm x " << target->getWidth() * 100.0f << "cm" << endl;
	cout << "\tEstimated value: $" << target->getBaseValue().getNumberString(true, false, 2) << endl;
	cout << "\tRarity: " << stringFromRarity(target->getRarity()) << endl;
}

#endif