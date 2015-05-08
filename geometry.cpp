#include "geometry.h"
#include "artwork.h"

painting_surface::painting_surface(
	float width, float height, shared_ptr<ogl_context> ogl_con, const char* texture_path)
{
	context = ogl_con;

	float total_height = height;
	float total_width = width;
	
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

void painting_surface::draw(const mat4 &model_matrix, const shared_ptr<ogl_camera> &camera, bool absolute) const
{
	shared_ptr<GLuint> temp_vao = opengl_data->getVAO();
	shared_ptr<GLuint> temp_vbo = opengl_data->getVBO();
	shared_ptr<GLuint> temp_tex = opengl_data->getTEX();

	glBindVertexArray(*temp_vao);
	glBindTexture(GL_TEXTURE_2D, *temp_tex);

	glUniform1i(context->getShaderGLint("absolute_position"), absolute);

	if (absolute)
	{
		glUniformMatrix4fv(context->getShaderGLint("model_matrix"), 1, GL_FALSE, &model_matrix[0][0]);
		glUniform1f(context->getShaderGLint("aspect_scale"), context->getAspectRatio());
	}

	else
	{
		mat4 MVP = camera->getProjectionMatrix() * camera->getViewMatrix() * model_matrix;
		glUniformMatrix4fv(context->getShaderGLint("MVP"), 1, GL_FALSE, &MVP[0][0]);
	}

	glDrawArrays(GL_TRIANGLES, 0, opengl_data->getVertexCount());
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindVertexArray(0);
}

frame_model::frame_model(float painting_width, float painting_height, shared_ptr<ogl_context> ogl_con, string frame_texture, string matte_texture,
	float f_width, float f_depth, float m_width, float m_setback, float p_setback)
{
	context = ogl_con;

	frame_texture_path = frame_texture;
	matte_texture_path = matte_texture;

	frame_width = f_width;
	frame_depth = f_depth;
	matte_width = m_width;
	matte_setback = m_setback;
	painting_setback = p_setback;

	float default_texture_dimension = 0.3f;

	painting_to_wall_dimension = frame_depth - matte_setback - painting_setback;
	float total_width = painting_width + (matte_width * 2.0f) + (frame_width * 2.0f);
	float total_height = painting_height + (matte_width * 2.0f) + (frame_width * 2.0f);

	//p_ prefix denotes painting dimensions
	float p_half_width = painting_width / 2.0f, p_half_height = painting_height / 2.0f;
	float p_x_left = 0.0f - p_half_width, p_x_right = p_half_width, p_y_top = p_half_height, p_y_bottom = 0.0f - p_half_height;

	//mi_ prefix denotes "matte inner", mo_ prefix denotes "matte outer"
	float mi_x_left = p_x_left, mi_x_right = p_x_right;
	float mi_y_top = p_y_top, mi_y_bottom = p_y_bottom;
	float mo_x_left = p_x_left - matte_width, mo_x_right = p_x_right + matte_width;
	float mo_y_top = p_y_top + matte_width, mo_y_bottom = p_y_bottom - matte_width;
	float mi_z_back = painting_to_wall_dimension, mi_z_front = mi_z_back + painting_setback;

	vec3 mo_right_top_front(mo_x_right, mo_y_top, mi_z_front);
	vec3 mo_right_bottom_front(mo_x_right, mo_y_bottom, mi_z_front);
	vec3 mo_left_top_front(mo_x_left, mo_y_top, mi_z_front);
	vec3 mo_left_bottom_front(mo_x_left, mo_y_bottom, mi_z_front);

	vec3 mi_right_top_front(mi_x_right, mi_y_top, mi_z_front);
	vec3 mi_right_top_back(mi_x_right, mi_y_top, mi_z_back);
	vec3 mi_right_bottom_front(mi_x_right, mi_y_bottom, mi_z_front);
	vec3 mi_right_bottom_back(mi_x_right, mi_y_bottom, mi_z_back);
	vec3 mi_left_top_front(mi_x_left, mi_y_top, mi_z_front);
	vec3 mi_left_top_back(mi_x_left, mi_y_top, mi_z_back);
	vec3 mi_left_bottom_front(mi_x_left, mi_y_bottom, mi_z_front);
	vec3 mi_left_bottom_back(mi_x_left, mi_y_bottom, mi_z_back);

	//fi_ prefix denotes "frame inner", fo_ prefix denotes "frame outer"
	float fi_x_left = mo_x_left, fi_x_right = mo_x_right;
	float fi_y_top = mo_y_top, fi_y_bottom = mo_y_bottom;
	float fi_z_back = mi_z_front, fi_z_front = frame_depth;
	float fo_x_left = mo_x_left - frame_width, fo_x_right = mo_x_right + frame_width;
	float fo_y_top = mo_y_top + frame_width, fo_y_bottom = mo_y_bottom - frame_width;
	float fo_z_back = 0.0f, fo_z_front = frame_depth;

	vec3 fo_right_top_front(fo_x_right, fo_y_top, fo_z_front);
	vec3 fo_right_top_back(fo_x_right, fo_y_top, fo_z_back);
	vec3 fo_right_bottom_front(fo_x_right, fo_y_bottom, fo_z_front);
	vec3 fo_right_bottom_back(fo_x_right, fo_y_bottom, fo_z_back);
	vec3 fo_left_top_front(fo_x_left, fo_y_top, fo_z_front);
	vec3 fo_left_top_back(fo_x_left, fo_y_top, fo_z_back);
	vec3 fo_left_bottom_front(fo_x_left, fo_y_bottom, fo_z_front);
	vec3 fo_left_bottom_back(fo_x_left, fo_y_bottom, fo_z_back);

	vec3 fi_right_top_front(fi_x_right, fi_y_top, fi_z_front);
	vec3 fi_right_top_back(fi_x_right, fi_y_top, fi_z_back);
	vec3 fi_right_bottom_front(fi_x_right, fi_y_bottom, fi_z_front);
	vec3 fi_right_bottom_back(fi_x_right, fi_y_bottom, fi_z_back);
	vec3 fi_left_top_front(fi_x_left, fi_y_top, fi_z_front);
	vec3 fi_left_top_back(fi_x_left, fi_y_top, fi_z_back);
	vec3 fi_left_bottom_front(fi_x_left, fi_y_bottom, fi_z_front);
	vec3 fi_left_bottom_back(fi_x_left, fi_y_bottom, fi_z_back);

	vector<float> frame_vertices;

	vector<float> frame_left_front = generateInterleavedVertices(
		fo_left_bottom_front, fo_left_top_front, fi_left_top_front, fi_left_bottom_front,
		default_texture_dimension, 'x', 'y');

	vector<float> frame_left_outer = generateInterleavedVertices(
		fo_left_bottom_back, fo_left_top_back, fo_left_top_front, fo_left_bottom_front,
		default_texture_dimension, 'z', 'y');

	vector<float> frame_left_inner = generateInterleavedVertices(
		fi_left_bottom_front, fi_left_top_front, fi_left_top_back, fi_left_bottom_back,
		default_texture_dimension, 'z', 'y');

	frame_vertices.insert(frame_vertices.begin(), frame_left_front.begin(), frame_left_front.end());
	frame_vertices.insert(frame_vertices.begin(), frame_left_outer.begin(), frame_left_outer.end());
	frame_vertices.insert(frame_vertices.begin(), frame_left_inner.begin(), frame_left_inner.end());

	vector<float> frame_right_front = generateInterleavedVertices(
		fi_right_bottom_front, fi_right_top_front, fo_right_top_front, fo_right_bottom_front,
		default_texture_dimension, 'x', 'y');

	vector<float> frame_right_outer = generateInterleavedVertices(
		fo_right_bottom_front, fo_right_top_front, fo_right_top_back, fo_right_bottom_back,
		default_texture_dimension, 'z', 'y');

	vector<float> frame_right_inner = generateInterleavedVertices(
		fi_right_bottom_back, fi_right_top_back, fi_right_top_front, fi_right_bottom_front,
		default_texture_dimension, 'z', 'y');

	frame_vertices.insert(frame_vertices.begin(), frame_right_front.begin(), frame_right_front.end());
	frame_vertices.insert(frame_vertices.begin(), frame_right_outer.begin(), frame_right_outer.end());
	frame_vertices.insert(frame_vertices.begin(), frame_right_inner.begin(), frame_right_inner.end());

	vector<float> frame_bottom_front = generateInterleavedVertices(
		fo_right_bottom_front, fo_left_bottom_front, fi_left_bottom_front, fi_right_bottom_front,
		default_texture_dimension, 'y', 'x');

	vector<float> frame_bottom_outer = generateInterleavedVertices(
		fo_right_bottom_back, fo_left_bottom_back, fo_left_bottom_front, fo_right_bottom_front,
		default_texture_dimension, 'z', 'x');

	vector<float> frame_bottom_inner = generateInterleavedVertices(
		fi_right_bottom_front, fi_left_bottom_front, fi_left_bottom_back, fi_right_bottom_back,
		default_texture_dimension, 'z', 'x');

	frame_vertices.insert(frame_vertices.begin(), frame_bottom_front.begin(), frame_bottom_front.end());
	frame_vertices.insert(frame_vertices.begin(), frame_bottom_outer.begin(), frame_bottom_outer.end());
	frame_vertices.insert(frame_vertices.begin(), frame_bottom_inner.begin(), frame_bottom_inner.end());

	vector<float> frame_top_front = generateInterleavedVertices(
		fi_right_top_front, fi_left_top_front, fo_left_top_front, fo_right_top_front,
		default_texture_dimension, 'y', 'x');

	vector<float> frame_top_outer = generateInterleavedVertices(
		fo_right_top_front, fo_left_top_front, fo_left_top_back, fo_right_top_back,
		default_texture_dimension, 'z', 'x');

	vector<float> frame_top_inner = generateInterleavedVertices(
		fi_right_top_back, fi_left_top_back, fi_left_top_front, fi_right_top_front, 
		default_texture_dimension, 'z', 'x');

	frame_vertices.insert(frame_vertices.begin(), frame_top_front.begin(), frame_top_front.end());
	frame_vertices.insert(frame_vertices.begin(), frame_top_outer.begin(), frame_top_outer.end());
	frame_vertices.insert(frame_vertices.begin(), frame_top_inner.begin(), frame_top_inner.end());

	shared_ptr<jep::ogl_data> generated_frame(new jep::ogl_data(
		ogl_con, frame_texture_path.c_str(), GL_STATIC_DRAW, frame_vertices, 3, 2, 5 * sizeof(float), 3 * sizeof(float)));

	frame_opengl_data = generated_frame;
	
	vector<float> matte_vertices;

	vector<float> matte_left_front = generateInterleavedVertices(
		mo_left_bottom_front, mo_left_top_front, mi_left_top_front, mi_left_bottom_front,
		default_texture_dimension, 'x', 'y');

	vector<float> matte_left_inner = generateInterleavedVertices(
		mi_left_bottom_front, mi_left_top_front, mi_left_top_back, mi_left_bottom_back,
		default_texture_dimension, 'z', 'y');

	matte_vertices.insert(matte_vertices.begin(), matte_left_front.begin(), matte_left_front.end());
	matte_vertices.insert(matte_vertices.begin(), matte_left_inner.begin(), matte_left_inner.end());

	vector<float> matte_right_front = generateInterleavedVertices(
		mi_right_bottom_front, mi_right_top_front, mo_right_top_front, mo_right_bottom_front,
		default_texture_dimension, 'x', 'y');

	vector<float> matte_right_inner = generateInterleavedVertices(
		mi_right_bottom_back, mi_right_top_back, mi_right_top_front, mi_right_bottom_front,
		default_texture_dimension, 'z', 'y');

	matte_vertices.insert(matte_vertices.begin(), matte_right_front.begin(), matte_right_front.end());
	matte_vertices.insert(matte_vertices.begin(), matte_right_inner.begin(), matte_right_inner.end());

	vector<float> matte_bottom_front = generateInterleavedVertices(
		mo_right_bottom_front, mo_left_bottom_front, mi_left_bottom_front, mi_right_bottom_front,
		default_texture_dimension, 'y', 'x');

	vector<float> matte_bottom_inner = generateInterleavedVertices(
		mi_right_bottom_front, mi_left_bottom_front, mi_left_bottom_back, mi_right_bottom_back,
		default_texture_dimension, 'z', 'x');

	matte_vertices.insert(matte_vertices.begin(), matte_bottom_front.begin(), matte_bottom_front.end());
	matte_vertices.insert(matte_vertices.begin(), matte_bottom_inner.begin(), matte_bottom_inner.end());

	vector<float> matte_top_front = generateInterleavedVertices(
		mi_right_top_front, mi_left_top_front, mo_left_top_front, mo_right_top_front,
		default_texture_dimension, 'y', 'x');

	vector<float> matte_top_inner = generateInterleavedVertices(
		mi_right_top_back, mi_left_top_back, mi_left_top_front, mi_right_top_front,
		default_texture_dimension, 'z', 'x');

	matte_vertices.insert(matte_vertices.begin(), matte_top_front.begin(), matte_top_front.end());
	matte_vertices.insert(matte_vertices.begin(), matte_top_inner.begin(), matte_top_inner.end());

	shared_ptr<jep::ogl_data> generated_matte(new jep::ogl_data(
		ogl_con, matte_texture_path.c_str(), GL_STATIC_DRAW, matte_vertices, 3, 2, 5 * sizeof(float), 3 * sizeof(float)));

	matte_opengl_data = generated_matte;
}

void frame_model::draw(const mat4 &model_matrix, const shared_ptr<ogl_camera> &camera, bool absolute) const
{
	//TODO revise opengl_data class to contain rendering data
	//draw the frame
	shared_ptr<GLuint> temp_vao = frame_opengl_data->getVAO();
	shared_ptr<GLuint> temp_vbo = frame_opengl_data->getVBO();
	shared_ptr<GLuint> temp_tex = frame_opengl_data->getTEX();

	glBindVertexArray(*temp_vao);
	glBindTexture(GL_TEXTURE_2D, *temp_tex);	

	glUniform1i(context->getShaderGLint("absolute_position"), absolute);

	if (absolute)
	{
		glUniformMatrix4fv(context->getShaderGLint("model_matrix"), 1, GL_FALSE, &model_matrix[0][0]);
		glUniform1f(context->getShaderGLint("ascpect_scale"), context->getAspectRatio());
	}

	else
	{
		mat4 MVP = camera->getProjectionMatrix() * camera->getViewMatrix() * model_matrix;
		glUniformMatrix4fv(context->getShaderGLint("MVP"), 1, GL_FALSE, &MVP[0][0]);
	}
	
	glDrawArrays(GL_TRIANGLES, 0, frame_opengl_data->getVertexCount());

	glBindTexture(GL_TEXTURE_2D, 0);
	glBindVertexArray(0);

	//draw the matte
	temp_vao = matte_opengl_data->getVAO();
	temp_vbo = matte_opengl_data->getVBO();
	temp_tex = matte_opengl_data->getTEX();

	glBindVertexArray(*temp_vao);
	glBindTexture(GL_TEXTURE_2D, *temp_tex);

	//glUniformMatrix4fv(context->getMVPID(), 1, GL_FALSE, &MVP[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, matte_opengl_data->getVertexCount());

	glBindTexture(GL_TEXTURE_2D, 0);
	glBindVertexArray(0);
}

line::line(vec4 first, vec4 second, vec4 c)
{
	p1 = first;
	p2 = second;
	color = c;

	VAO = shared_ptr<GLuint>(new GLuint);
	VBO = shared_ptr<GLuint>(new GLuint);

	glGenVertexArrays(1, VAO.get());
	glBindVertexArray(*VAO);
	glGenBuffers(1, VBO.get());
	glBindBuffer(GL_ARRAY_BUFFER, *VBO);

	vector<float> vec_vertices{ first.x, first.y, first.z, second.x, second.y, second.z };

	glBufferData(GL_ARRAY_BUFFER, vec_vertices.size() * sizeof(float), &vec_vertices[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glDisableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

line::~line()
{
	glDeleteVertexArrays(1, VAO.get());
	glDeleteBuffers(1, VAO.get());
}

void line::draw(const shared_ptr<ogl_context> &context, const shared_ptr<ogl_camera> &camera, bool absolute) const
{
	glBindVertexArray(*VAO);
	glUniform1i(context->getShaderGLint("absolute_position"), absolute);

	GLint line = true;
	glUniform1i(context->getShaderGLint("color_override"), true);
	glUniform4f(context->getShaderGLint("override_color"), color.x, color.y, color.z, color.w);

	mat4 MVP = camera->getProjectionMatrix() * camera->getViewMatrix() * glm::translate(mat4(1.0f), vec3(0.0f, 0.0f, 0.0f));
	glUniformMatrix4fv(context->getShaderGLint("MVP"), 1, GL_FALSE, &MVP[0][0]);

	glEnableVertexAttribArray(0);
	glDrawArrays(GL_LINES, 0, 2);
	glDisableVertexAttribArray(0);

	glUniform1i(context->getShaderGLint("color_override"), false);

	line = false;
	glUniform1i(context->getShaderGLint("draw_line"), line);
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
}