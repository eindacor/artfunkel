#ifndef ARTWORK_H
#define ARTWORK_H
#include "header.h"

class artwork
{
public:
	//TODO make constructor take shared_ptr<artist> instead of artist object
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
			jep::date work_date) :
			ID(work_id),
			title(work_title), 	
			artist(work_artist), 
			genre(work_genre), 
			rarity(work_rarity), 
			forgery(work_forgery),  
			condition(work_condition), 
			height(work_height),
			width(work_width),
			image_name(work_image_name),
			date(work_date) { setValue(); }
	~artwork(){};

	int getID() const { return ID; }
	genre getGenre() const { return genre; }
	double getValue() const { return value; }
	string getTitle() const { return title; }
	const shared_ptr<artist> getArtist() const { return artist; }
	string getArtistName() const { return artist->getName(); }
	bool isForgery() const { return forgery; }
	float getHeight() const { return height; }
	float getWidth() const { return width; }
	jep::date getDate() const { return date; }

	void setValue() { value = lookupValue(rarity); }

private:
	int ID;
	double value;
	genre genre;
	string title;
	const shared_ptr<artist> artist;
	rarity rarity;
	bool forgery;
	float condition;
	float height;
	float width;
	string image_name;
	jep::date date;
};

#endif