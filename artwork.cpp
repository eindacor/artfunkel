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
	for (int i = 0; (artwork_attribute)i != NULL_ATTRIBUTE; i++)
	{
		artwork_attribute att = (artwork_attribute)i;

		if (attributeIsDefault(att))
		{
			//if (attributeIsBignum(att))
				//bignum_attributes[att] = rollNewBignumValue(att);

			//else float_attributes[att] = rollNewFloatValue(att);

			attributes[att] = rollNewFloatValue(att);
		}
	}

	int primary_attribute_count, secondary_attribute_count;

	switch (r)
	{
	case COMMON: primary_attribute_count = 1; secondary_attribute_count = 0; break;
	case UNCOMMON: primary_attribute_count = 2; secondary_attribute_count = 0; break;
	case RARE: primary_attribute_count = 3; secondary_attribute_count = 1; break;
	case LEGENDARY: primary_attribute_count = 4; secondary_attribute_count = 2; break;
	case MASTERPIECE: primary_attribute_count = 5; secondary_attribute_count = 3; break;
	default: primary_attribute_count = 0; secondary_attribute_count = 0; break;
	}

	for (int i = 0; i < primary_attribute_count; i++)
		addNewNonDefaultAttribute(true);

	for (int i = 0; i < secondary_attribute_count; i++)
		addNewNonDefaultAttribute(false);

	printAttributes();
	setAttributeStrings();
}

work_attributes::work_attributes(const work_attributes &other)
{
	//float_attributes = other.getFloatAttributes();
	//bignum_attributes = other.getBignumAttributes();
	attributes = other.getAttributes();
	setAttributeStrings();
}

void work_attributes::addNewNonDefaultAttribute(bool primary)
{
	vector<artwork_attribute> potential_rolls;

	for (int i = 0; (artwork_attribute)i != NULL_ATTRIBUTE; i++)
	{
		artwork_attribute att = (artwork_attribute)i;

		//if the primary factor doesn't match
		if (attributeIsPrimary(att) != primary)
			continue;

		//default attributes cannot be rolled
		if (attributeIsDefault(att))
			continue;

		//if (bignum_attributes.find(att) != bignum_attributes.end())
			//continue;

		//if (float_attributes.find(att) != float_attributes.end())
			//continue;

		if (attributes.find(att) != attributes.end())
			continue;

		potential_rolls.push_back(att);
	}

	int random_roll = rand() % potential_rolls.size();

	artwork_attribute rolled = potential_rolls.at(random_roll);

	//if (attributeIsBignum(rolled))
		//bignum_attributes[rolled] = rollNewBignumValue(rolled);

	//else float_attributes[rolled] = rollNewFloatValue(rolled);

	attributes[rolled] = rollNewFloatValue(rolled);

	setAttributeStrings();
}

//replaces stored attribute aa with a new attribute and new value
void work_attributes::rollNewAttribute(artwork_attribute aa)
{
	if (attributeIsDefault(aa))
		return;

	//first remove attribute from current attributes
	//if (bignum_attributes.find(aa) != bignum_attributes.end())
		//bignum_attributes.erase(aa);

	//else if (float_attributes.find(aa) != float_attributes.end())
		//float_attributes.erase(aa);

	if (attributes.find(aa) != attributes.end())
		attributes.erase(aa);

	vector<artwork_attribute> potential_rolls;

	for (int i = 0; (artwork_attribute)i != NULL_ATTRIBUTE; i++)
	{
		artwork_attribute att = (artwork_attribute)i;

		//if its the same attribute
		if (att == aa)
			continue;

		//if the primary factor doesn't match
		if (attributeIsPrimary(att) != attributeIsPrimary(aa))
			continue;

		//default attributes cannot be rolled
		if (attributeIsDefault(att))
			continue;

		potential_rolls.push_back(att);
	}

	int random_roll = rand() % potential_rolls.size();

	artwork_attribute rolled = potential_rolls.at(random_roll);

	//if (attributeIsBignum(rolled))
		//bignum_attributes[rolled] = rollNewBignumValue(rolled);

	//else float_attributes[rolled] = rollNewFloatValue(rolled);

	attributes[rolled] = rollNewFloatValue(rolled);

	setAttributeStrings();
}

/*
//rolls a new value for stored attribute aa
bignum work_attributes::rollNewBignumValue(artwork_attribute aa)
{
	pair<bignum, bignum> minmax = getAttributeMinMaxBignum(aa);
	return jep::randomNumberAddPrecision(minmax.first, minmax.second, 2);
	setAttributeStrings();
}
*/

float work_attributes::rollNewFloatValue(artwork_attribute aa)
{
	pair<float, float> minmax = getAttributeMinMax(aa);
	return jep::floatRoll(minmax.first, minmax.second, 4);
	setAttributeStrings();
}

bool attributeIsDefault(artwork_attribute aa)
{
	switch (aa)
	{
	case BASE_XP_EARNED:									
	case BASE_XP_DURATION:
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
	case NPC_MARKET_EXPERT_BASE: return true;
	default: return false;
	}
}

void work_attributes::printAttributes() const
{
	cout << "------attributes------" << endl;
	cout << "\tbase:" << endl;
	cout << default_attributes_string << endl;

	cout << "\tprimary:" << endl;
	cout << primary_attributes_string << endl;

	cout << "\tsecondary:" << endl;
	cout << secondary_attributes_string << endl;
}

void work_attributes::setAttributeStrings()
{
	vector<artwork_attribute> default_attributes;
	vector<artwork_attribute> primary_attributes;
	vector<artwork_attribute> secondary_attributes;

	for (const auto &att : attributes)
	{
		if (attributeIsDefault(att.first))
			default_attributes.push_back(att.first);

		else if (attributeIsPrimary(att.first))
			primary_attributes.push_back(att.first);

		else secondary_attributes.push_back(att.first);
	}

	default_attributes_string = "";
	for (const auto &att : default_attributes)
	{
		bignum att_num(std::to_string(getAttributeRating(att, attributes.at(att))));
		att_num.roundToIndex(ONES_PLACE - 2);
		att_num.leftShift(2);
		default_attributes_string += stringFromAttribute(att) + ": " + att_num.getNumberString(false, false, 0) + "\n";
	}

	primary_attributes_string = "";
	for (const auto &att : primary_attributes)
	{
		bignum att_num(std::to_string(getAttributeRating(att, attributes.at(att))));
		att_num.roundToIndex(ONES_PLACE - 2);
		att_num.leftShift(2);
		primary_attributes_string += stringFromAttribute(att) + ": " + att_num.getNumberString(false, false, 0) + "\n";
	}

	secondary_attributes_string = "";
	for (const auto &att : secondary_attributes)
	{
		bignum att_num(std::to_string(getAttributeRating(att, attributes.at(att))));
		att_num.roundToIndex(ONES_PLACE - 2);
		att_num.leftShift(2);
		secondary_attributes_string += stringFromAttribute(att) + ": " + att_num.getNumberString(false, false, 0) + "\n";
	}
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