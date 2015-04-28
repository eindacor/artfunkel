#ifndef ARTWORK_H
#define ARTWORK_H
#include "header.h"
#include "artist.h"
#include "geometry.h"

class artwork
{
public:
	artwork(int work_id,
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
		shared_ptr<painting_surface> work_surface);
	~artwork(){};

	int getID() const { return ID; }
	genre getGenre() const { return _genre; }
	double getValue() const { return value; }
	string getTitle() const { return title; }
	const shared_ptr<artist> getArtist() const { return artist_ptr; }
	string getArtistName() const { return artist_ptr->getName(); }
	rarity getRarity() const { return Rarity; }
	bool isForgery() const { return forgery; }
	float getHeight() const { return height; }
	float getWidth() const { return width; }
	jep::date getDate() const { return date; }
	shared_ptr<painting_surface> getSurface() { return surface; }

	void setValue() { value = lookupValue(Rarity); }

	void test(genre work_genre);

private:
	int ID;
	double value;
	genre _genre;
	string title;
	shared_ptr<artist> artist_ptr;
	rarity Rarity;
	bool forgery;
	float condition;
	float height;
	float width;
	string image_name;
	jep::date date;

	shared_ptr<painting_surface> surface;
};

#endif