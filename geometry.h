#ifndef GEOMETRY_H
#define GEOMETRY_H

#include "header.h"

class painting_surface
{
public:
	painting_surface(float width, float height, shared_ptr<ogl_context> ogl_con, shared_ptr<ogl_camera> ogl_cam, const char* texture_path);
	~painting_surface(){};

	void draw() const;
	void moveRelative(mat4 translation_matrix) { model_matrix = model_matrix * translation_matrix; }
	void moveAbsolute(mat4 position_matrix) { model_matrix = position_matrix; }

private:
	shared_ptr<ogl_context> context;
	shared_ptr<ogl_camera> camera;
	mat4 model_matrix;
	vec3 centroid;

	boost::shared_ptr<jep::ogl_data> opengl_data;
};

#endif