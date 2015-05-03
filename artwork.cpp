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
	jep::date work_date)
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
	surface = shared_ptr<painting_surface>(nullptr);
}

void artwork_data::loadData(shared_ptr<ogl_context> ogl_con, shared_ptr<ogl_camera> ogl_cam)
{
	shared_ptr<painting_surface> work_surface(new painting_surface(width, height, ogl_con, ogl_cam, image_path.c_str()));
	surface = work_surface;
}