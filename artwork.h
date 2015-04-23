#ifndef ARTWORK_H
#define ARTWORK_H
#include "header.h"
#include "artist.h"

class artwork
{
public:
	artwork(string work_title, 
			artist work_artist, 
			genre work_genre, 
			rarity work_rarity, 
			bool work_forgery, 
			float work_condition, 
			jep::date work_dates...) :
			title(work_title), 	
			artist(work_artist), 
			genre(work_genre), 
			rarity(work_rarity), 
			forgery(work_forgery),  
			condition(work_condition) { setValue(); }
	~artwork(){};

	genre getGenre() const { return genre; }
	double getValue() const { return value; }
	string getTitle() const { return title; }
	artist getArtist() const { return artist; }
	string getArtistName() const { return artist.getName(); }
	bool isForgery() const { return forgery; }

	void setValue() { value = lookupValue(rarity); }

private:
	double value;
	genre genre;
	string title;
	artist artist;
	rarity rarity;
	bool forgery;
	float condition;

	vector<jep::date> dates;
};

#endif