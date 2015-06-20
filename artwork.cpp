#include "header.h"
#include "artwork.h"
#include "utility_funcs.h"

artwork_data::artwork_data(int work_id,
	string work_title,
	shared_ptr<artist> work_artist,
	genre work_genre,
	rarity work_rarity,
	float work_height,
	float work_width,
	string work_image_path,
	jep::date work_date,
	bignum work_value)
{
	ID = work_id;
	_genre = work_genre;
	title = work_title;
	artist_ptr = work_artist;
	Rarity = work_rarity;
	height = work_height;
	width = work_width;
	image_path = work_image_path;
	date = work_date;
	base_value = work_value;
	surface = shared_ptr<painting_surface>(nullptr);
}

artwork_data::artwork_data()
{
	ID = 0;
	_genre = UNKNOWN_GENRE;
	title = "";
	artist_ptr = nullptr;
	Rarity = UNKNOWN_RARITY;
	height = 0.0f;
	width = 0.0f;
	image_path = "";
	date = jep::date("0000????");
	base_value = bignum();
	surface = nullptr;
}

void artwork_data::loadData(const shared_ptr<ogl_context> &ogl_con, const shared_ptr<ogl_camera> &ogl_cam)
{
	shared_ptr<painting_surface> work_surface(new painting_surface(width, height, ogl_con, image_path.c_str()));
	surface = work_surface;
}

work_attributes::work_attributes(rarity r)
{
	rollBaseVisitorXP();
}

work_attributes::work_attributes(const work_attributes &other)
{
	
}

void work_attributes::rollBaseVisitorXP()
{
	roll_base_visitor_xp_count++;
	base_visitor_xp_earned = jep::randomNumberAddPrecision(bignum(".00001"), bignum(".00002"), 2);
}
void work_attributes::rollBaseVisitorXPDuration()
{
	roll_base_visitor_xp_duration_count++;
	base_visitor_xp_earning_duration = jep::randomNumberAddPrecision(bignum("20"), bignum("60"), 2);
}

//rolls current primary attribute to be a new primary attribute
void work_attributes::rollPrimary(primary_attribute attribute_to_reroll)
{
	vector<primary_attribute> rollable_attributes;

	for (int i = 0; i != int(NULL_PRIMARY_ATTRIBUTE); i++)
	{
		primary_attribute att = (primary_attribute)i;
		if (att != attribute_to_reroll && primary_attributes.find(att) == primary_attributes.end())
			rollable_attributes.push_back(att);
	}

	int random_index = rand() % rollable_attributes.size();

	rollPrimaryValue
}

//rolls current primary attribute to have a new value
void work_attributes::rollPrimaryValue(primary_attribute pa, npc_type nt)
{
	if (primary_attributes.find(pa) != primary_attributes.end())
	{
		switch (pa)
		{
		case ENTRY_FEE_REDUCITON:
			primary_attributes.at(pa) = jep::randomNumberAddPrecision(bignum(".00002"), bignum(".0002"), 2);
			return;

		case NPC_SPAWN_CHANCE_INCREASE:
			npc_spawn_boosts.at(nt) = jep::randomNumberAddPrecision(bignum(".00002"), bignum(".0002"), 2);
			return;

		case INCREASE_XP_FROM_SET_PAINTINGS_FOR_VISITORS:
			primary_attributes.at(pa) = jep::randomNumberAddPrecision(bignum(".00002"), bignum(".0002"), 2);
			return;

		case INCREASE_XP_FROM_GALLERY_PAINTINGS_FOR_VISITORS:
			primary_attributes.at(pa) = jep::randomNumberAddPrecision(bignum(".00002"), bignum(".0002"), 2);
			return;

		case EARNING_DURATION_REDUCTION_FROM_SET_PAINTINGS_FOR_VISITORS:
			primary_attributes.at(pa) = jep::randomNumberAddPrecision(bignum(".00002"), bignum(".0002"), 2);
			return;

		case EARNING_DURATION_REDUCTION_FROM_GALLERY_PAINTINGS_FOR_VISITORS:
			primary_attributes.at(pa) = jep::randomNumberAddPrecision(bignum(".00002"), bignum(".0002"), 2);
			return;

		default: return;
		}
	}
}



//rolls current secondary attribute to be a new primary attribute
void work_attributes::rollSecondary(secondary_attribute sa)
{

}

//rolls current secondary attribute to have a new value
void work_attributes::rollSecondaryValue(secondary_attribute sa)
{

}

//rolls current spawn chance for a particular npc type to a new spawn chance
void work_attributes::rollSpawnChance(npc_type nt)
{

}

int work_attributes::getRollPrimaryCount(primary_attribute pa) const
{
	if (roll_primary_counts.find(pa) == roll_primary_counts.end())
		return -1;

	else return roll_primary_counts.at(pa);
}

int work_attributes::getRollPrimaryValueCount(primary_attribute pa) const
{
	if (roll_primary_value_counts.find(pa) == roll_primary_value_counts.end())
		return -1;

	else return roll_primary_value_counts.at(pa);
}

int work_attributes::getRollSecondaryCount(secondary_attribute sa) const
{
	if (roll_secondary_counts.find(sa) == roll_secondary_counts.end())
		return -1;

	else return roll_secondary_counts.at(sa);
}

int work_attributes::getRollSecondaryValueCount(secondary_attribute sa) const
{
	if (roll_secondary_value_counts.find(sa) == roll_secondary_value_counts.end())
		return -1;

	else return roll_secondary_value_counts.at(sa);
}

int work_attributes::getRollSpawnChanceCount(npc_type nt) const
{
	if (roll_spawn_chance_counts.find(nt) == roll_spawn_chance_counts.end())
		return -1;

	else return roll_spawn_chance_counts.at(nt);
}

artwork::artwork()
{
	value = 0;
	forgery = false;
	condition = 0.0f;
	centerpoint = vec4(0.0f, 0.0f, 0.0f, 1.0f);
	model_matrix = glm::translate(mat4(1.0f), vec3(0.0f, 0.0f, 0.0f));
	p_frame = nullptr;
	data = nullptr;
	updateOverallDimensions();
	profited = false;
}

artwork::artwork(const shared_ptr<artwork_data> &work_data, bool work_forgery, float work_condition) : attributes(work_data->getRarity())
{
	data = work_data;
	forgery = work_forgery;
	condition = work_condition;
	model_matrix = glm::translate(mat4(1.0f), vec3(0.0f, 0.0f, 0.0f));
	p_frame = nullptr;
	centerpoint = vec4(0.0f, 0.0f, 0.0f, 1.0f);
	profited = false;
	setValue();
	updateOverallDimensions();
}

artwork::artwork(const artwork &original) : attributes(original.getWorkAttributes())
{
	forgery = original.isForgery();
	condition = original.getCondition();
	model_matrix = original.getModelMatrix();
	p_frame = original.getFrame();
	centerpoint = original.getCenter();
	value = original.getValue();
	data = original.getData();
	updateOverallDimensions();
	profited = original.getProfited();
}

void artwork::moveRelative(mat4 move_matrix) 
{ 
	model_matrix = model_matrix * move_matrix; centerpoint = centerpoint * move_matrix; 
}

void artwork::moveAbsolute(vec3 position) 
{
	mat4 translation_matrix = glm::translate(mat4(1.0f), position);
	model_matrix = translation_matrix;
	centerpoint = vec4(0.0f, 0.0f, 0.0f, 1.0f);
	centerpoint = translation_matrix * centerpoint;
}

void artwork::setModelMatrix(mat4 m) 
{
	model_matrix = m;
	centerpoint = vec4(0.0f, 0.0f, 0.0f, 1.0f);
	centerpoint = model_matrix * centerpoint;
}

void artwork::draw(const shared_ptr<ogl_context> &ogl_con, const shared_ptr<ogl_camera> &ogl_cam) const
{
	mat4 frame_offset(glm::translate(mat4(1.0f), vec3(0.0f, 0.0f, 0.0f)));
	if (p_frame != nullptr)
	{
		p_frame->draw(ogl_con, model_matrix, ogl_cam, false);
		float z_offset = p_frame->getPaintingDistanceToWall();
		frame_offset = glm::translate(mat4(1.0f), vec3(0.0f, 0.0f, z_offset));
	}

	if (data->getSurface() == nullptr)
		data->loadData(ogl_con, ogl_cam);
	data->getSurface()->draw(ogl_con, model_matrix * frame_offset, ogl_cam, false);
}

void artwork::draw2D(const shared_ptr<ogl_context> &ogl_con, const shared_ptr<ogl_camera> &ogl_cam, const mat4 &position_matrix) const
{
	mat4 aspect_matrix = glm::scale(mat4(1.0f), vec3((1.0f / ogl_con->getAspectRatio(), 1.0f, 1.0f)));

	mat4 frame_offset(glm::translate(mat4(1.0f), vec3(0.0f, 0.0f, 0.0f)));
	if (p_frame != nullptr)
		p_frame->draw(ogl_con, position_matrix, ogl_cam, true);

	if (data->getSurface() == nullptr)
		data->loadData(ogl_con, ogl_cam);
	data->getSurface()->draw(ogl_con, position_matrix, ogl_cam, true);
}

const artwork& artwork::operator = (const artwork &other)
{
	data = other.getData();

	forgery = other.isForgery();
	condition = other.getCondition();
	model_matrix = other.getModelMatrix();
	p_frame = other.getFrame();
	centerpoint = other.getCenter();
	updateOverallDimensions();

	setValue();
	return *this;
}

bool artwork::operator == (const artwork &other) const
{
	if (data->getID() != other.getData()->getID())
		return false;

	if (forgery != other.isForgery())
		return false;

	if (condition != other.getCondition())
		return false;

	return true;
}

void artwork::applyFrameTemplate(const shared_ptr<ogl_context> &context, const shared_ptr<texture_handler> &textures, const frame_model &frame_template)
{
	p_frame = shared_ptr<frame_model>(new frame_model(
		data->getWidth(), data->getHeight(), context, frame_template.getFrameTextureFilename(), frame_template.getMatteTextureFilename(), textures,
		frame_template.getFrameWidth(), frame_template.getFrameDepth(), frame_template.getMatteWidth(), frame_template.getMatteSetback(), frame_template.getPaintingSetback()));

	updateOverallDimensions();
}

void artwork::applyFrameTemplate2D(const shared_ptr<ogl_context> &context, const shared_ptr<texture_handler> &textures, const frame_model &frame_template)
{
	p_frame = shared_ptr<frame_model>(new frame_model(
		data->getWidth(), data->getHeight(), context, textures, frame_template.getFrameTextureFilename(), frame_template.getMatteTextureFilename(),
		frame_template.getFrameWidth(), frame_template.getMatteWidth()));

	updateOverallDimensions();
}

void artwork::setValue()
{
	value = data->getBaseValue() * condition;
	if (forgery)
		value *= .15;
}

void artwork::updateOverallDimensions()
{
	float total_width = data->getWidth();
	float total_height = data->getHeight();
	float total_depth = 0.0f;
	
	if (p_frame != nullptr)
	{
		total_width += p_frame->getFrameWidth() * 2.0f;
		total_width += p_frame->getMatteWidth() * 2.0f;
		total_height += p_frame->getFrameWidth() * 2.0f;
		total_height += p_frame->getMatteWidth() * 2.0f;
		total_depth += p_frame->getFrameDepth();
	}

	overall_dimensions = vec3(total_width, total_height, total_depth);

	vec3 left_top_front(total_width / -2.0f, total_height / 2.0f, total_depth);
	vec3 left_top_back(total_width / -2.0f, total_height / 2.0f, 0.0f);
	vec3 left_bottom_front(total_width / -2.0f, total_height / -2.0f, total_depth);
	vec3 left_bottom_back(total_width / -2.0f, total_height / -2.0f, 0.0f);
	vec3 right_top_front(total_width / 2.0f, total_height / 2.0f, total_depth);
	vec3 right_top_back(total_width / 2.0f, total_height / 2.0f, 0.0f);
	vec3 right_bottom_front(total_width / 2.0f, total_height / -2.0f, total_depth);
	vec3 right_bottom_back(total_width / 2.0f, total_height / -2.0f, 0.0f);

	select_surfaces.clear();
	//FRONT FACE
	select_surfaces.push_back(vector<vec3>{ left_bottom_front, left_top_front, right_top_front }); //upper left	
	select_surfaces.push_back(vector<vec3>{ left_bottom_front, right_top_front, right_bottom_front }); //lower right

	if (p_frame != nullptr)
	{
		//TOP FACE
		select_surfaces.push_back(vector < vec3 > { left_top_front, left_top_back, right_top_back }); //upper left	
		select_surfaces.push_back(vector < vec3 > { left_top_front, right_top_back, right_top_front }); //lower right
		//LEFT FACE
		select_surfaces.push_back(vector < vec3 > { left_bottom_back, left_top_back, left_top_front }); //upper left	
		select_surfaces.push_back(vector < vec3 > { left_bottom_back, left_top_front, left_bottom_front }); //lower right
		//RIGHT FACE
		select_surfaces.push_back(vector < vec3 > { right_bottom_front, right_top_front, right_top_back }); //upper left	
		select_surfaces.push_back(vector < vec3 > { right_bottom_front, right_top_back, right_bottom_back }); //lower right
		//BOTTOM FACE
		select_surfaces.push_back(vector < vec3 > { left_bottom_back, left_bottom_front, right_bottom_front }); //upper left	
		select_surfaces.push_back(vector < vec3 > { left_bottom_back, right_bottom_front, right_bottom_back }); //lower right
	}
}

//modify 
bool artwork::isClicked(shared_ptr<key_handler> &keys, const shared_ptr<ogl_camera> &camera, const pair<vec3, vec3> &ray, float &scale) const
{
	mat4 inverse_model_matrix = glm::inverse(model_matrix);

	vec3 origin = vec3(inverse_model_matrix * vec4(ray.first, 1.0f));
	vec3 direction = vec3(inverse_model_matrix * vec4(ray.second, 1.0f)) - origin;

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