#include "geometry.h"
#include "artwork.h"

painting_surface::painting_surface(
	float width, float height, shared_ptr<ogl_context> ogl_con, shared_ptr<ogl_camera> ogl_cam, const char* texture_path)
{
	context = ogl_con;
	camera = ogl_cam;

	float total_height = height / 100.0f;
	float total_width = width / 100.0f;
	
	float width_offset = total_width / 2.0f;
	float height_offset = total_height / 2.0f;

	vec3 lower_left(-1.0f * width_offset, -1.0f * height_offset, 0.0f);
	vec3 lower_right(width_offset, -1.0f * height_offset, 0.0f);
	vec3 upper_left(-1.0f * width_offset, height_offset, 0.0f);
	vec3 upper_right(width_offset, height_offset, 0.0f);

	vector<float> vec_vertices {
			lower_left.x, lower_left.y, lower_left.z,
			0.0f, 0.0f,
			upper_left.x, upper_left.y, upper_left.z,
			0.0f, 1.0f,
			lower_right.x, lower_right.y, lower_right.z,
			1.0f, 0.0f,
			upper_left.x, upper_left.y, upper_left.z,
			0.0f, 1.0f,
			upper_right.x, upper_right.y, upper_right.z,
			1.0f, 1.0f,
			lower_right.x, lower_right.y, lower_right.z,
			1.0f, 0.0f,
	};

	opengl_data = shared_ptr<jep::ogl_data>(new jep::ogl_data(ogl_con, texture_path, GL_STATIC_DRAW, vec_vertices, 3, 2, 5 * sizeof(float), 3 * sizeof(float)));
}

void painting_surface::draw(const mat4 &model_matrix) const
{
	shared_ptr<GLuint> temp_vao = opengl_data->getVAO();
	shared_ptr<GLuint> temp_vbo = opengl_data->getVBO();
	shared_ptr<GLuint> temp_tex = opengl_data->getTEX();

	glBindVertexArray(*temp_vao);
	glBindTexture(GL_TEXTURE_2D, *temp_tex);

	mat4 MVP = context->getProjectionMatrix() * camera->getViewMatrix() * model_matrix;
	glUniformMatrix4fv(context->getMVPID(), 1, GL_FALSE, &MVP[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, opengl_data->getVertexCount());

	glBindTexture(GL_TEXTURE_2D, 0);

	glBindVertexArray(0);
}

frame::frame(float painting_width, float painting_height, shared_ptr<ogl_context> ogl_con, shared_ptr<ogl_camera> ogl_cam, const char* frame_texture, const char* matte_texture,
	float frame_width, float frame_depth, float matte_width, float matte_setback, float painting_setback)
{

}