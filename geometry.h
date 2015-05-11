#ifndef GEOMETRY_H
#define GEOMETRY_H

#include "header.h"

class triangle
{
public:
	triangle(vec3 p1, vec3 p2, vec3 p3);
	~triangle(){};

private:

};

class painting_surface
{
public:
	painting_surface(float width, float height, shared_ptr<ogl_context> ogl_con, const char* texture_path);
	~painting_surface(){};

	void draw(const shared_ptr<ogl_context> &context, const mat4 &model_matrix, const shared_ptr<ogl_camera> &camera, bool absolute = false) const;

private:
	shared_ptr<ogl_context> context;
	shared_ptr<jep::ogl_data> opengl_data;
};

class frame_model
{
public:
	frame_model(float painting_width, float painting_height, shared_ptr<ogl_context> ogl_con, string frame_texture, string matte_texture,
		float frame_width = 0.10f, float frame_depth = .08f, float matte_width = 0.1f, float matte_setback = 0.025f, float painting_setback = 0.003125f);
	~frame_model(){};

	void draw(const shared_ptr<ogl_context> &context, const mat4 &model_matrix, const shared_ptr<ogl_camera> &camera, 
		bool absolute = false) const;

	float getPaintingDistanceToWall() const { return painting_to_wall_dimension; }
	
	shared_ptr<ogl_context> getContext() const { return context; }
	string getFrameTexturePath() const { return frame_texture_path; }
	string getMatteTexturePath() const { return matte_texture_path; }
	float getFrameWidth() const { return frame_width; }
	float getFrameDepth() const { return frame_depth; }
	float getMatteWidth() const { return matte_width; }
	float getMatteSetback() const { return matte_setback; }
	float getPaintingSetback() const { return painting_setback; }

private:
	shared_ptr<ogl_context> context;

	string frame_texture_path;
	string matte_texture_path;

	float frame_width, frame_depth, matte_width, matte_setback, painting_setback;

	shared_ptr<jep::ogl_data> frame_opengl_data;
	shared_ptr<jep::ogl_data> matte_opengl_data;

	float painting_to_wall_dimension;
};

class line
{
public:
	line(vec4 first, vec4 second, vec4 c);
	~line();

	void moveFirstRelative(mat4 translation) { p1 = translation * p1; }
	void moveFirstAbsolute(vec4 new_point) { p1 = new_point; }
	void moveSecondRelative(mat4 translation) { p2 = translation * p2; }
	void moveSecondAbsolute(vec4 new_point) { p2 = new_point; }

	void draw(const shared_ptr<ogl_context> &ogl_con, const shared_ptr<ogl_camera> &ogl_cam, bool absolute = false) const;

private:
	vec4 p1;
	vec4 p2;

	shared_ptr<GLuint> VBO;
	shared_ptr<GLuint> VAO;
	vec4 color;
};

#endif