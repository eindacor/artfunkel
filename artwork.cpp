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

pair<float, float> artwork_instance::getOverallDimensions() const
{
	float total_width = getWidth();
	float total_height = getHeight();
	
	if (p_frame != nullptr)
	{
		total_width += p_frame->getFrameWidth() * 2.0f;
		total_width += p_frame->getMatteWidth() * 2.0f;
		total_height += p_frame->getFrameWidth() * 2.0f;
		total_height += p_frame->getMatteWidth() * 2.0f;
	}

	return pair<float, float>(total_height, total_width);
}

const artwork_instance& artwork_instance::operator = (const artwork_instance &other)
{
	setID(other.getID());
	setTitle(other.getTitle());
	setArtist(other.getArtist());
	setGenre(other.getGenre());
	setRarity(other.getRarity());
	setHeight(other.getHeight());
	setWidth(other.getWidth());
	setImagePath(other.getImagePath());
	setDate(other.getDate());
	setBaseValue(other.getBaseValue());
	setSurface(other.getSurface());

	forgery = other.isForgery();
	condition = other.getCondition();
	model_matrix = other.getModelMatrix();
	p_frame = other.getFrame();
	centerpoint = other.getCenter();

	setValue();
	return *this;
}

bool artwork_instance::operator == (const artwork_instance &other) const
{
	if (getID() != other.getID())
		return false;

	if (forgery != other.isForgery())
		return false;

	if (condition != other.getCondition())
		return false;

	return true;
}

void artwork_instance::applyFrameTemplate(const frame_model &frame_template)
{
	p_frame = shared_ptr<frame_model>(new frame_model(
		getWidth(), getHeight(), frame_template.getContext(), frame_template.getFrameTexturePath(), frame_template.getMatteTexturePath(),
		frame_template.getFrameWidth(), frame_template.getFrameDepth(), frame_template.getMatteWidth(), frame_template.getMatteSetback(), frame_template.getPaintingSetback()));
}

void artwork_instance::setValue()
{
	value = getBaseValue() * condition;
	if (forgery)
		value *= .15;
}