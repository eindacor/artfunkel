#include "geometry.h"

render_data::render_data(boost::shared_ptr<ogl_camera> c, boost::shared_ptr<ogl_context> ch,
	const char* texture_path, GLenum dt, std::vector<float> vec_vertices)
{
	/*
	cam = c;
	context = ch;
	draw_type = dt;
	//color_type = MTL_TEXTURE;

	glGenBuffers(1, VBO.get());
	glBindBuffer(GL_ARRAY_BUFFER, *VBO);
	glBufferData(GL_ARRAY_BUFFER, vec_vertices.size() * sizeof(float), &vec_vertices[0], draw_type);

	glGenVertexArrays(1, VAO.get());
	glBindVertexArray(*VAO);

	//load texture
	jep::loadTexture(texture_path, *texture);
	GLuint texture_ID = glGetUniformLocation(context->getProgramID(), "myTextureSampler");
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, *texture);
	glUniform1i(texture_ID, 0);
	glBindTexture(GL_TEXTURE_2D, 0);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	//points passed were vec3's, so size = 3
	glVertexAttribPointer(0, mesh.getVSize(), GL_FLOAT, GL_FALSE, mesh.getInterleaveStride(), (void*)0);
	glVertexAttribPointer(1, mesh.getVTSize(), GL_FLOAT, GL_FALSE, mesh.getInterleaveStride(), (void*)(mesh.getInterleaveVTOffset()));

	glBindVertexArray(0);

	std::cout << "-------------" << std::endl;

	std::cout << "getFloatCount(): " << mesh.getFloatCount() << std::endl;

	std::cout << "getInterleaveStride(): " << mesh.getInterleaveStride() << std::endl;
	std::cout << "getInterleaveVTOffset(): " << mesh.getInterleaveVTOffset() << std::endl;
	std::cout << "getInterleaveVNOffset(): " << mesh.getInterleaveVNOffset() << std::endl;

	std::cout << "getVData().size(): " << mesh.getVData().size() << std::endl;
	std::cout << "getFaceCount(): " << mesh.getFaceCount() << std::endl;

	std::cout << "getVSize(): " << mesh.getVSize() << std::endl;
	std::cout << "getVTSize(): " << mesh.getVTSize() << std::endl;
	std::cout << "getVertexCount(): " << mesh.getVertexCount() << std::endl;
	*/
}