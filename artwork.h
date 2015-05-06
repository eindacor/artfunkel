#ifndef ARTWORK_H
#define ARTWORK_H

#include "header.h"
#include "artist.h"
#include "geometry.h"

class artwork_data
{
public:
	artwork_data();
	artwork_data(int work_id,
		string work_title,
		shared_ptr<artist> work_artist,
		genre work_genre,
		rarity work_rarity,
		float work_height,
		float work_width,
		string work_image_path,
		date work_date,
		bignum base_value);
	~artwork_data(){};

	int getID() const { return ID; }
	genre getGenre() const { return _genre; }
	string getTitle() const { return title; }
	const shared_ptr<artist> getArtist() const { return artist_ptr; }
	string getArtistName() const { return artist_ptr->getName(); }
	rarity getRarity() const { return Rarity; }
	float getHeight() const { return height; }
	float getWidth() const { return width; }
	string getImagePath() const { return image_path; }
	date getDate() const { return date; }
	bignum getBaseValue() const { return base_value; }
	const shared_ptr<painting_surface> getSurface() const { return surface; }

	void setID(int i) { ID = i; }
	void setGenre(genre g) { _genre = g; }
	void setTitle(string s)  { title = s; }
	void setArtist(const shared_ptr<artist> &a) { artist_ptr = a; }
	void setRarity(rarity r) { Rarity = r; }
	void setHeight(float h) { height = h; }
	void setWidth(float w) { width = w; }
	void setImagePath(string s) { image_path = s; }
	void setDate(date d) { date = d; }
	void setBaseValue(bignum b) { base_value = b; }

	void loadData(shared_ptr<ogl_context> ogl_con, shared_ptr<ogl_camera> ogl_cam);
	void unloadData() { surface = shared_ptr<painting_surface>(nullptr); }
	void setSurface(shared_ptr<painting_surface> surf) { surface = surf; }

private:
	int ID;
	genre _genre;
	string title;
	shared_ptr<artist> artist_ptr;
	rarity Rarity;
	float height;
	float width;
	string image_path;
	date date;
	bignum base_value;

	shared_ptr<painting_surface> surface;
};

//TODO revise so artwork_instance is not a child class, but contains and artwork_data object instead
class artwork_instance : public artwork_data
{
public:
	artwork_instance() : artwork_data(), p_frame(nullptr)
	{
		forgery = false;
		condition = 0.0f;
		model_matrix = glm::translate(mat4(1.0f), vec3(0.0f, 0.0f, 0.0f));
		centerpoint = vec4(0.0f, 0.0f, 0.0f, 1.0f);
	};	//end of default constructor

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
		bignum work_base_value) :
		artwork_data(work_id, work_title, work_artist, work_genre, 
		work_rarity, work_height, work_width, work_image_name, work_date, work_base_value)
	{
		forgery = work_forgery;
		condition = work_condition;
		model_matrix = glm::translate(mat4(1.0f), vec3(0.0f, 0.0f, 0.0f));
		p_frame = nullptr;
		centerpoint = vec4(0.0f, 0.0f, 0.0f, 1.0f);
		setValue();
	}	//end of primary constructor

	//copy constructor
	//TODO modify copy constructor to use existing surface data if it is available in memory
	artwork_instance(const artwork_instance &original) :
		artwork_data(original.getID(), original.getTitle(), original.getArtist(), original.getGenre(),
		original.getRarity(), original.getHeight(), original.getWidth(), original.getImagePath(), original.getDate(), original.getBaseValue())
	{
		forgery = original.isForgery();
		condition = original.getCondition();
		model_matrix = original.getModelMatrix();
		p_frame = original.getFrame();
		centerpoint = original.getCenter();
		value = original.getValue();
	}; //end of copy constructor
	~artwork_instance(){};

	bignum getValue() const { return value; }
	bool isForgery() const { return forgery; }
	float getCondition() const { return condition; }
	vec4 getCenter() const { return centerpoint; }

	void moveRelative(mat4 translation_matrix) { model_matrix = model_matrix * translation_matrix; centerpoint = centerpoint * translation_matrix; }
	void moveAbsolute(vec3 position) { 
		mat4 translation_matrix = glm::translate(mat4(1.0f), position);
		model_matrix = translation_matrix;
		centerpoint = vec4(0.0f, 0.0f, 0.0f, 1.0f);	
		centerpoint = translation_matrix * centerpoint;
	}

	mat4 getModelMatrix() const { return model_matrix; }
	void setModelMatrix(mat4 m) { 
		model_matrix = m; 
		centerpoint = vec4(0.0f, 0.0f, 0.0f, 1.0f);
		centerpoint = model_matrix * centerpoint;
	}

	shared_ptr<frame_model> getFrame() const { return p_frame; }
	pair<float, float> getOverallDimensions() const;

	void setValue();
	void loadFrame(const shared_ptr<frame_model> &work_frame) { p_frame = work_frame; }
	void draw(const shared_ptr<ogl_context> &ogl_con, const shared_ptr<ogl_camera> &ogl_cam, bool absolute=false)
	{
		mat4 frame_offset(glm::translate(mat4(1.0f), vec3(0.0f, 0.0f, 0.0f)));
		if (p_frame != nullptr)
		{
			p_frame->draw(model_matrix, ogl_cam, absolute);
			float z_offset = p_frame->getPaintingDistanceToWall();
			frame_offset = glm::translate(mat4(1.0f), vec3(0.0f, 0.0f, z_offset));
		}

		if (getSurface() == nullptr)
			loadData(ogl_con, ogl_cam);
		getSurface()->draw(model_matrix * frame_offset, ogl_cam, absolute);
	}

	const artwork_instance& operator = (const artwork_instance &other);

	bool operator == (const artwork_instance &other) const;
	bool operator != (const artwork_instance &other) const { return !(*this == other); }

	void applyFrameTemplate(const frame_model &frame_template);

private:
	bignum value;
	bool forgery;
	float condition;
	vec4 centerpoint;
	mat4 model_matrix;

	shared_ptr<frame_model> p_frame;
};

#endif