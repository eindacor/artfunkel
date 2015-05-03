#ifndef GEOMETRY_H
#define GEOMETRY_H

#include "header.h"

class painting_surface
{
public:
	painting_surface(float width, float height, shared_ptr<ogl_context> ogl_con, shared_ptr<ogl_camera> ogl_cam, const char* texture_path);
	~painting_surface(){};

	void draw(const mat4 &model_matrix) const;

private:
	shared_ptr<ogl_context> context;
	shared_ptr<ogl_camera> camera;

	shared_ptr<jep::ogl_data> opengl_data;
};

class frame_model
{
public:
	frame_model(float painting_width, float painting_height, shared_ptr<ogl_context> ogl_con, shared_ptr<ogl_camera> ogl_cam, const char* frame_texture, const char* matte_texture,
		float frame_width = 0.10f, float frame_depth = .08f, float matte_width = 0.1f, float matte_setback = 0.025f, float painting_setback = 0.003125f);
	~frame_model(){};

	void draw(const mat4 &model_matrix) const;

	float getPaintingDistanceToWall() const { return painting_to_wall_dimension; }

private:
	shared_ptr<ogl_context> context;
	shared_ptr<ogl_camera> camera;

	shared_ptr<jep::ogl_data> frame_opengl_data;
	shared_ptr<jep::ogl_data> matte_opengl_data;

	float painting_to_wall_dimension;
};

#endif