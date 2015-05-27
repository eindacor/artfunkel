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
}

artwork::artwork(const shared_ptr<artwork_data> &work_data, bool work_forgery, float work_condition)
{
	data = work_data;
	forgery = work_forgery;
	condition = work_condition;
	model_matrix = glm::translate(mat4(1.0f), vec3(0.0f, 0.0f, 0.0f));
	p_frame = nullptr;
	centerpoint = vec4(0.0f, 0.0f, 0.0f, 1.0f);
	setValue();
	updateOverallDimensions();
}

artwork::artwork(const artwork &original)
{
	forgery = original.isForgery();
	condition = original.getCondition();
	model_matrix = original.getModelMatrix();
	p_frame = original.getFrame();
	centerpoint = original.getCenter();
	value = original.getValue();
	data = original.getData();
	updateOverallDimensions();
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