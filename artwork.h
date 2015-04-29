#ifndef ARTWORK_H
#define ARTWORK_H
#include "header.h"
#include "artist.h"
#include "geometry.h"

class artwork_data
{
public:
	artwork_data(int work_id,
		string work_title,
		shared_ptr<artist> work_artist,
		genre work_genre,
		rarity work_rarity,
		float work_height,
		float work_width,
		string work_image_name,
		jep::date work_date,
		shared_ptr<painting_surface> work_surface);
	~artwork_data(){};

	int getID() const { return ID; }
	genre getGenre() const { return _genre; }
	double getValue() const { return value; }
	string getTitle() const { return title; }
	const shared_ptr<artist> getArtist() const { return artist_ptr; }
	string getArtistName() const { return artist_ptr->getName(); }
	rarity getRarity() const { return Rarity; }
	float getHeight() const { return height; }
	float getWidth() const { return width; }
	string getImageName() const { return image_name; }
	jep::date getDate() const { return date; }
	const shared_ptr<painting_surface> getSurface() const { return surface; }

	void setValue() { value = lookupValue(Rarity); }

private:
	int ID;
	double value;
	genre _genre;
	string title;
	shared_ptr<artist> artist_ptr;
	rarity Rarity;
	float height;
	float width;
	string image_name;
	jep::date date;

	shared_ptr<painting_surface> surface;
};

class artwork_instance : public artwork_data
{
public:
	//primary constructor
	artwork_instance(int work_id,
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
		shared_ptr<painting_surface> work_surface) :
		artwork_data(work_id, work_title, work_artist, work_genre, 
		work_rarity, work_height, work_width, work_image_name, work_date, work_surface)
	{
		forgery = work_forgery;
		condition = work_condition;
		model_matrix = glm::translate(mat4(1.0f), vec3(0.0f, 0.0f, 0.0f));
	};	//end of primary constructor

	//copy constructor
	artwork_instance(const artwork_instance &original) :
		artwork_data(getID(), getTitle(), getArtist(), getGenre(), 
		getRarity(), getHeight(), getWidth(), getImageName(), getDate(), getSurface())
	{
		forgery = original.isForgery();
		condition = original.getCondition();
		model_matrix = original.getModelMatrix();
	}; //end of copy constructor
	~artwork_instance(){};

	double getValue() const { return value; }
	bool isForgery() const { return forgery; }
	float getCondition() const { return condition; }

	void moveRelative(mat4 translation_matrix) { model_matrix = model_matrix * translation_matrix; }
	void moveAbsolute(mat4 position_matrix) { model_matrix = position_matrix; }
	mat4 getModelMatrix() const { return model_matrix; }

	void setValue() { value = lookupValue(getRarity()); }

	void draw() const { getSurface()->draw(model_matrix); }

private:
	double value;
	bool forgery;
	float condition;
	mat4 model_matrix;
};

#endif