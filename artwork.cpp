#include "header.h"
#include "artwork.h"

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

void artwork_data::loadData(shared_ptr<ogl_context> ogl_con, shared_ptr<ogl_camera> ogl_cam)
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

void artwork::draw(const shared_ptr<ogl_context> &ogl_con, const shared_ptr<ogl_camera> &ogl_cam, bool absolute)
{
	mat4 frame_offset(glm::translate(mat4(1.0f), vec3(0.0f, 0.0f, 0.0f)));
	if (p_frame != nullptr)
	{
		p_frame->draw(model_matrix, ogl_cam, absolute);
		float z_offset = p_frame->getPaintingDistanceToWall();
		frame_offset = glm::translate(mat4(1.0f), vec3(0.0f, 0.0f, z_offset));
	}

	if (data->getSurface() == nullptr)
		data->loadData(ogl_con, ogl_cam);
	data->getSurface()->draw(model_matrix * frame_offset, ogl_cam, absolute);
}

const artwork& artwork::operator = (const artwork &other)
{
	data = other.getData();

	forgery = other.isForgery();
	condition = other.getCondition();
	model_matrix = other.getModelMatrix();
	p_frame = other.getFrame();
	centerpoint = other.getCenter();

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

void artwork::applyFrameTemplate(const frame_model &frame_template)
{
	p_frame = shared_ptr<frame_model>(new frame_model(
		data->getWidth(), data->getHeight(), frame_template.getContext(), frame_template.getFrameTexturePath(), frame_template.getMatteTexturePath(),
		frame_template.getFrameWidth(), frame_template.getFrameDepth(), frame_template.getMatteWidth(), frame_template.getMatteSetback(), frame_template.getPaintingSetback()));
}

void artwork::setValue()
{
	value = data->getBaseValue() * condition;
	if (forgery)
		value *= .15;
}

pair<float, float> artwork::getOverallDimensions() const
{
	float total_width = data->getWidth();
	float total_height = data->getHeight();
	
	if (p_frame != nullptr)
	{
		total_width += p_frame->getFrameWidth() * 2.0f;
		total_width += p_frame->getMatteWidth() * 2.0f;
		total_height += p_frame->getFrameWidth() * 2.0f;
		total_height += p_frame->getMatteWidth() * 2.0f;
	}

	return pair<float, float>(total_height, total_width);
}