#ifndef GALLERY_H
#define GALLERY_H

#include "header.h"

class display_wall
{
public:
	display_wall(vec3 p1, vec3 p2, vec3 p3, vec3 p4);
	~display_wall(){}

	bool validPlacement(const shared_ptr<artwork> &placed, const vec2 &position);
	mat4 getWallModelMatrix() const { return wall_model_matrix; }
	vec2 getCursorLocation(const shared_ptr<key_handler> &keys, const shared_ptr<ogl_camera> &camera) const;

	void addPainting(const vec2 &position, const shared_ptr<artwork> &toAdd) { wall_contents.push_back(pair<vec2, shared_ptr<artwork> >(position, toAdd)); }

private:
	//contains center position and artwork
	vector<vec2> wall_points;
	vector< pair<vec2, shared_ptr<artwork> > >wall_contents;
	mat4 wall_model_matrix;
};

class gallery
{
public:
	//TODO modify constructors to take a model file, containing specific display points
	gallery(const char* model_path);
	~gallery(){};

	void addPainting(int index, const shared_ptr<artwork> &work);
	void renderGallery(const shared_ptr<ogl_context> &ogl_con, const shared_ptr<ogl_camera> &ogl_cam) const;

private:
	//int is the index of the specific position, mat4 is the position matrix
	int max_paintings;
	map <int, mat4> work_positions;
	map <int, shared_ptr<artwork> > works_displayed;
	float width;
	float height;

	//vector<player> players_present;
};

#endif