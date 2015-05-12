#ifndef GALLERY_H
#define GALLERY_H

#include "header.h"

class display_wall
{
public:
	//class constructed with vec3's, then converts to vec2's with a model matrix to simplify raytracing
	//first point is used as the wall origin, which is used to determing the painting location matrix
	display_wall(const shared_ptr<ogl_context> &context, string texture_path,
		const vector<float> &vec_vertices, int geometry_offset, int normal_offset, int uv_offset, int stride);
	~display_wall(){}

	bool validPlacement(const shared_ptr<artwork> &placed, const vec2 &position);
	mat4 getWallModelMatrix() const { return wall_model_matrix; }
	bool wallClicked(shared_ptr<key_handler> &keys, const shared_ptr<ogl_camera> &camera);
	//vec2 getCursorLocationOnWall(shared_ptr<key_handler> &keys, const shared_ptr<ogl_camera> &camera) const;

	void addPainting(const vec2 &position, const shared_ptr<artwork> &toAdd) { wall_contents.push_back(pair<vec2, shared_ptr<artwork> >(position, toAdd)); }
	void draw(const shared_ptr<ogl_context> &context, const shared_ptr<ogl_camera> &camera);

private:
	//wall_edges is used to determine whether a point is inside or outside the wall
	vector< pair<vec3, vec3> > wall_edges;
	//wall_triangles is used to determine raytraced cursor position on wall
	vector< vector<vec3> > wall_triangles;
	//wall_origin is used to determine translation offset of paintings
	vec2 wall_origin;
	vec2 click_position;
	//vec2 indicates position on wall, which is NOT directly related to the model matrix. model matrix of painting is identified separately
	//and incorporates the wall_model_matrix. position vector is stored separately for artwork collision purposes
	vector< pair<vec2, shared_ptr<artwork> > >wall_contents;
	mat4 wall_model_matrix;

	shared_ptr<jep::ogl_data> opengl_data;
};

class gallery
{
public:
	//TODO modify constructors to take a model file, containing specific display points
	gallery(const shared_ptr<ogl_context> &context, string model_path, string material_path,
		string display_model_filename, string filler_model_filename, string display_material_filename, string filler_material_filename);
	~gallery(){};

	void addPainting(int index, const shared_ptr<artwork> &work);
	void renderGallery(const shared_ptr<ogl_context> &context, const shared_ptr<ogl_camera> &camera) const
	{
		for (auto i : display_walls)
			i.second->draw(context, camera);
	}
	

private:
	//int is the index of the specific position, mat4 is the position matrix
	int max_paintings;
	map <int, mat4> work_positions;
	map <int, shared_ptr<display_wall> > display_walls;
	float width;
	float height;

	//vector<player> players_present;
};

#endif