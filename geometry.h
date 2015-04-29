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

	boost::shared_ptr<jep::ogl_data> opengl_data;
};

#endif