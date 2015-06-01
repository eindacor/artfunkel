#ifndef GALLERY_H
#define GALLERY_H

#include "header.h"
#include "geometry.h"
#include "utility_funcs.h"

class display_wall
{
public:
	//class constructed with vec3's, then converts to vec2's with a model matrix to simplify raytracing
	//first point is used as the wall origin, which is used to determing the painting location matrix
	display_wall(const shared_ptr<ogl_context> &context, mesh_data mesh, const shared_ptr<GLuint> &TEX);
	~display_wall(){}

	bool validPlacement(const shared_ptr<artwork> &placed, const vec2 &position);
	mat4 getWallModelMatrix() const { return wall_model_matrix; }
	bool cursorTouches(shared_ptr<key_handler> &keys, const shared_ptr<ogl_camera> &camera,
		const pair<vec3, vec3> &ray, float &scale);
	const vector< pair<vec2, shared_ptr<artwork> > > getWallContents() const { return wall_contents; }

	//shared_ptr not used to insure a unique instance of the painting is used for transformation
	//TODO change to addArtwork
	void addArtwork(const vec2 &position, artwork toAdd);
	//returns true if a work was removed
	bool removeArtwork(const shared_ptr<artwork> &to_remove);
	void draw(const shared_ptr<ogl_context> &context, const shared_ptr<ogl_camera> &camera);

	vec4 getCursorPositionWorldspace() const { return wall_model_matrix * vec4(cursor_position.x, cursor_position.y, 0.0f, 1.0f); }
	vec2 getCursorPositionWallspace() const { return cursor_position; }

private:
	//wall_edges is used to determine whether a point is inside or outside the wall
	vector< pair<vec3, vec3> > wall_edges;
	//wall_triangles is used to determine raytraced cursor position on wall
	vector< vector<vec3> > wall_triangles;
	//wall_origin is used to determine translation offset of paintings
	vec2 wall_origin;
	vec2 cursor_position;
	//vec2 indicates position on wall, which is NOT directly related to the model matrix. model matrix of painting is identified separately
	//and incorporates the wall_model_matrix. position vector is stored separately for artwork collision purposes
	vector< pair<vec2, shared_ptr<artwork> > >wall_contents;
	mat4 wall_model_matrix;

	shared_ptr<jep::ogl_data> opengl_data;

	//lines are for testing only
	vector< shared_ptr<line> > lines;
};

class gallery
{
public:
	//gallery(const shared_ptr<ogl_context> &context, shared_ptr<texture_handler> &textures, string model_path, string material_path,
		//string display_model_filename, string filler_model_filename, string display_material_filename, 
		//string filler_material_filename, string template_name_string, string owner);
	gallery(const shared_ptr<ogl_context> &context, shared_ptr<texture_handler> &textures, string model_path, string material_path, 
		string template_name_string, string owner);
	~gallery(){};

	void renderGallery(const shared_ptr<ogl_context> &context, const shared_ptr<ogl_camera> &camera) const;	
	shared_ptr<display_wall> getClosestWallUnderCursor(shared_ptr<key_handler> &keys, const shared_ptr<ogl_camera> &camera, float &distance);
	shared_ptr<artwork> getClosestArtworkUnderCursor(shared_ptr<key_handler> &keys, const shared_ptr<ogl_camera> &camera, float &distance);
	void addArtwork(int wall_index, const shared_ptr<artwork> &toAdd, vec2 position);
	void removeArtwork(const shared_ptr<artwork> &toRemove);
	string getTemplateName() const { return template_name; }
	string getOwnerName() const { return owner; }
	const map <int, shared_ptr<display_wall> > getWalls() const { return display_walls; }

	//work ID, position, wall index
	const map< unsigned, pair<vec2, unsigned short> >  getWorkMap() const;

private:
	//int is the index of the specific position, mat4 is the position matrix
	string template_name;
	string owner;
	int max_paintings;
	map <int, mat4> work_positions;
	map <int, shared_ptr<display_wall> > display_walls;
	float width;
	float height;
	//lines are for testing only
	vector< shared_ptr<line> > lines;

	vector< shared_ptr<jep::ogl_data> > environment_models;
	//vector<player> players_present;
};

#endif