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

	//load, unload functions increase performance by removing surface data when not needed
	void loadData(const shared_ptr<ogl_context> &ogl_con, const shared_ptr<ogl_camera> &ogl_cam);
	void unloadData() { surface = shared_ptr<painting_surface>(nullptr); }
	void setSurface(const shared_ptr<painting_surface> &surf) { surface = surf; }

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

class work_attributes
{
public:
	work_attributes(rarity r);
	work_attributes(const work_attributes &other);
	work_attributes(){};
	~work_attributes(){};

	//sets new xp and xp duration at random
	void rollBaseVisitorXP();
	void rollBaseVisitorXPDuration();

	//rolls current primary attribute to be a new primary attribute
	void rollPrimary(primary_attribute pa = NULL_PRIMARY_ATTRIBUTE);
	//rolls current primary attribute to have a new value
	void rollPrimaryValue(primary_attribute pa, npc_type nt = NULL_NPC_TYPE);

	//rolls current secondary attribute to be a new primary attribute
	void rollSecondary(secondary_attribute sa);
	//rolls current secondary attribute to have a new value
	void rollSecondaryValue(secondary_attribute sa);
	//rolls current spawn chance for a particular npc type to a new spawn chance
	void rollSpawnChance(npc_type nt);

	//these methods return the number of times a particular attribute or attribute value have been rolled
	int getRollBaseVisitorXPCount() const { return roll_base_visitor_xp_count; }
	int getRollBaseVisitorXPDurationCount() const { return roll_base_visitor_xp_duration_count; }
	int getRollPrimaryCount(primary_attribute pa) const;
	int getRollPrimaryValueCount(primary_attribute pa) const;
	int getRollSecondaryCount(secondary_attribute sa) const;
	int getRollSecondaryValueCount(secondary_attribute sa) const;
	int getRollSpawnChanceCount(npc_type nt) const;

private:
	bignum getRandomPrimaryValue(primary_attribute pa) const;
	bignum getRandomSecondaryValue(secondary_attribute sa) const;
	//----------FEATURED (benefit the visitor)
	//total amount to be earned from this painting, COMMON ROLLS FOR ALL RARITIES
	bignum base_visitor_xp_earned;
	//total time to earn xp (shorter the better, gets averaged with all other works), COMMON ROLLS FOR ALL RARITIES
	bignum base_visitor_xp_earning_duration;
	//npc_type and int so it can be passed to jep::catRoll and return a particular npc, COMMON ROLLS FOR ALL RARITIES
	map<npc_type, float> base_npc_spawn_chances;
	map<npc_type, float> npc_spawn_chance_attribute_boosts;
	//boost the feature attributes, BETTER ROLLS FOR HIGHER RARITY, <attribute, <times rolled, value> >
	map<primary_attribute, pair<int, bignum> > primary_attributes;

	//----------NON-FEATURED (benefit the owner)
	//BETTER ROLLS FOR HIGHER RARITY
	float base_value_gained_per_second;
	//boost the non-feature attributes, BETTER ROLLS FOR HIGHER RARITY
	map<secondary_attribute, bignum> secondary_attributes;

	//this data keeps track of the number of times a particular attribute has been rolled
	int roll_base_visitor_xp_count = 0;
	int roll_base_visitor_xp_duration_count = 0;
	map<primary_attribute, int> roll_primary_counts;
	map<primary_attribute, int> roll_primary_value_counts;
	map<secondary_attribute, int> roll_secondary_counts;
	map<secondary_attribute, int> roll_secondary_value_counts;
	map<npc_type, int> roll_spawn_chance_counts;
};

class artwork
{
public: 
	artwork();
	artwork(const shared_ptr<artwork_data> &work_data, bool work_forgery, float work_condition);
	artwork(const artwork &original);
	~artwork(){ data->unloadData(); }

	bool isForgery() const { return forgery; }
	float getCondition() const { return condition; }
	mat4 getModelMatrix() const { return model_matrix; }
	shared_ptr<frame_model> getFrame() const { return p_frame; }
	vec4 getCenter() const { return centerpoint; }
	bignum getValue() const { return value; }
	void updateOverallDimensions();
	vec3 getOverallDimensions() const { return overall_dimensions; }
	shared_ptr<artwork_data> getData() const { return data; }
	vector < vector<vec3> > getSelectSurfaces() const { return select_surfaces; }

	void moveRelative(mat4 move_matrix);
	void moveAbsolute(vec3 position);
	void setModelMatrix(mat4 m);

	void setValue();
	void loadFrame(const shared_ptr<frame_model> &work_frame) { p_frame = work_frame; updateOverallDimensions(); }
	void draw(const shared_ptr<ogl_context> &ogl_con, const shared_ptr<ogl_camera> &ogl_cam) const;
	void draw2D(const shared_ptr<ogl_context> &ogl_con, const shared_ptr<ogl_camera> &ogl_cam, const mat4 &position_matrix) const;

	const artwork& operator = (const artwork &other);
	bool operator == (const artwork &other) const;
	bool operator != (const artwork &other) const { return !(*this == other); }

	void applyFrameTemplate(const shared_ptr<ogl_context> &context, const shared_ptr<texture_handler> &textures, const frame_model &frame_template);
	void applyFrameTemplate2D(const shared_ptr<ogl_context> &context, const shared_ptr<texture_handler> &textures, const frame_model &frame_template);

	bool isClicked(shared_ptr<key_handler> &keys, const shared_ptr<ogl_camera> &camera,
		const pair<vec3, vec3> &ray, float &scale) const;

	void setProfitedTEMP(bool b) { profited = b; }
	bool getProfited() const { return profited; }
	work_attributes getWorkAttributes() const { return attributes; }
	work_attributes editWorkAttributes() { return attributes; }

private:
	bignum value;
	bool forgery;
	float condition;
	vec4 centerpoint;
	mat4 model_matrix;

	bool profited;

	vec3 overall_dimensions;
	//vector below stores faces to check for click selection;
	vector < vector<vec3> > select_surfaces;

	shared_ptr<frame_model> p_frame;
	shared_ptr<artwork_data> data;

	work_attributes attributes;
};

#endif