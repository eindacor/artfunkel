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

class artwork
{
public: 
	artwork();
	artwork(const shared_ptr<artwork_data> &work_data, bool work_forgery, float work_condition);
	artwork(const artwork &original);
	~artwork(){};

	bool isForgery() const { return forgery; }
	float getCondition() const { return condition; }
	mat4 getModelMatrix() const { return model_matrix; }
	shared_ptr<frame_model> getFrame() const { return p_frame; }
	vec4 getCenter() const { return centerpoint; }
	bignum getValue() const { return value; }
	pair<float, float> getOverallDimensions() const;
	shared_ptr<artwork_data> getData() const { return data; }

	void moveRelative(mat4 move_matrix);
	void moveAbsolute(vec3 position);
	void setModelMatrix(mat4 m);

	void setValue();
	void loadFrame(const shared_ptr<frame_model> &work_frame) { p_frame = work_frame; }
	void draw(const shared_ptr<ogl_context> &ogl_con, const shared_ptr<ogl_camera> &ogl_cam, bool absolute = false);

	const artwork& operator = (const artwork &other);
	bool operator == (const artwork &other) const;
	bool operator != (const artwork &other) const { return !(*this == other); }

	void applyFrameTemplate(const frame_model &frame_template);

private:
	bignum value;
	bool forgery;
	float condition;
	vec4 centerpoint;
	mat4 model_matrix;

	shared_ptr<frame_model> p_frame;
	shared_ptr<artwork_data> data;
};

#endif