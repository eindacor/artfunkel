#ifndef GEOMETRY_H
#define GEOMETRY_H

#include "header.h"

class render_data
{
public:
	render_data(boost::shared_ptr<ogl_camera> c, boost::shared_ptr<ogl_context> ch,
		const char* texture_path, GLenum dt, std::vector<float> vec_vertices);
	render_data(boost::shared_ptr<ogl_camera> c, boost::shared_ptr<ogl_context> ch,
		glm::vec4 color, GLenum dt, std::vector<float> vec_vertices);
	~render_data();
	void draw(const glm::mat4 &model_matrix = glm::mat4(1.0f)) const;
	const int getFaceCount() const { return face_count; }
private:
	boost::shared_ptr<GLuint> VBO;
	boost::shared_ptr<GLuint> VAO;
	boost::shared_ptr<GLuint> texture;
	boost::shared_ptr<ogl_camera> cam;
	boost::shared_ptr<ogl_context> context;
	int vertex_count;
	int face_count;

	//COLOR_TYPE color_type;

	GLenum draw_type;
};

class painting_surface
{
public:
	painting_surface(shared_ptr<artwork> p_artwork, shared_ptr<ogl_context> oglc){};
	~painting_surface(){};

private:
};

#endif