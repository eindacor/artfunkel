#include "header.h"
#include "artwork.h"

artwork::artwork(int work_id,
	string work_title,
	shared_ptr<artist> work_artist,
	genre work_genre,
	rarity work_rarity,
	bool work_forgery,
	float work_condition,
	float work_height,
	float work_width,
	string work_image_name,
	jep::date work_date,
	shared_ptr<painting_surface> work_surface)
{
	ID = work_id;
	_genre = work_genre;
	title = work_title;
	artist_ptr = work_artist;
	Rarity = work_rarity;
	forgery = work_forgery;
	condition = work_condition;
	height = work_height;
	width = work_width;
	image_name = work_image_name;
	date = work_date;
	surface = work_surface;

	setValue();
}

