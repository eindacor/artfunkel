#include "gallery.h"
#include "artwork.h"
#include "utility_funcs.h"

//dislay walls first need their vertex data modified to be flat on the z-axis, so raytracing only needs to modify the ray created for click
//detection, rather than matrix-translating all of the walls on-click
display_wall::display_wall(const shared_ptr<ogl_context> &context, string texture_path, 
	const vector<float> &vec_vertices, int geometry_offset, int normal_offset, int uv_offset, int stride)
{
	//TODO make data collection more robust for bad model files
	//wall_triangles = getTriangles(vec_vertices, geometry_offset, stride);

	if (vec_vertices.size() < 3)
		throw;

	vec3 anchor_point(vec_vertices.at(0), vec_vertices.at(1), vec_vertices.at(2));
	//vec3 anchor_point = anchor_triangle.at(0);
	mat4 adjustment_position_matrix = glm::translate(mat4(1.0f), anchor_point * -1.0f);

	float normal_rotation = getNormalRotation(vec_vertices, normal_offset, stride);
	float surface_rotation = normal_rotation - 90.0f;
	mat4 adjustment_rotation_matrix = glm::rotate(mat4(1.0f), surface_rotation * -1.0f, vec3(0.0f, 1.0f, 0.0f));

	//this matrix modifies the modeled geometry to be on the z-axis for easy click intersection detection
	mat4 adjustment_matrix = adjustment_rotation_matrix * adjustment_position_matrix;

	vector<float> modified_vec_vertices;
	vector<float> point_found;
	int stride_count = stride / sizeof(float);
	int offset_count = uv_offset / sizeof(float);
	for (int i = 0; i < vec_vertices.size(); i++)
	{
		int local_index = i % stride_count;
		//skip if local index is before or after target range
		if (local_index >= 3)
			modified_vec_vertices.push_back(vec_vertices.at(i));

		else point_found.push_back(vec_vertices.at(i));

		if (point_found.size() == 3)
		{
			vec4 point_from_original(point_found.at(0), point_found.at(1), point_found.at(2), 1.0f);
			vec3 adjusted_point = vec3(adjustment_matrix * point_from_original);
			modified_vec_vertices.push_back(adjusted_point.x);
			modified_vec_vertices.push_back(adjusted_point.y);
			modified_vec_vertices.push_back(adjusted_point.z);
			point_found.clear();
		}
	}

	//for each vector of vec3's in wall_triangles
	wall_triangles = getTriangles(modified_vec_vertices, geometry_offset, stride);

	//wall model matrix is the inverse of the original adjustment, returning it to its proper location when drawn
	wall_model_matrix = glm::inverse(adjustment_matrix);
	wall_edges = getOuterEdges(wall_triangles);

	//TODO remove lines once graphics are improved, for visual clarity only
	for (auto i : wall_edges)
	{
		vec4 first(vec4(i.first, 1.0f));
		vec4 second(vec4(i.second, 1.0f));
		lines.push_back(shared_ptr<line>(new line(wall_model_matrix * first, wall_model_matrix * second, vec4(0.0f, 0.0f, 0.0f, 1.0f))));
	}

	//vec_vertices need to be modified before passing to GPU
	opengl_data = shared_ptr<jep::ogl_data>(new jep::ogl_data(
		context,
		texture_path.c_str(), 
		GL_STATIC_DRAW, 
		modified_vec_vertices,
		3, 
		2, 
		stride, 
		uv_offset));
}

bool display_wall::validPlacement(const shared_ptr<artwork> &placed, const vec2 &position)
{
	vec3 dimensions(placed->getOverallDimensions());
	float width(dimensions.x);
	float height(dimensions.y);

	float half_width(width / 2.0f);
	float half_height(height / 2.0f);

	vector<vec2> placed_points_to_check{
		vec2(half_width * -1.0f, half_height) + position,			//upper left
		vec2(half_width, half_height) + position, 					//upper right
		vec2(half_width * -1.0f, half_height * -1.0f) + position,	//lower left	
		vec2(half_width, half_height * -1.0f) + position			//lower right
	};

	for (auto i : placed_points_to_check)
	{
		//verifies paintings is within bounds of wall
		if (!jep::pointInPolygon(wall_edges, i))
			return false;

		else cout << "point not within wall bounds: " << i.x << ", " << i.y << endl;

		//verifies painting does not collide with other paintings
		for (auto p : wall_contents)
		{
			vec3 other_dimensions = p.second->getOverallDimensions();
			float other_half_width = other_dimensions.x / 2.0f;
			float other_half_height = other_dimensions.y / 2.0f;

			vector<vec2> other_bounding_points{
				vec2(other_half_width * -1.0f, other_half_height) + p.first,			//upper left
				vec2(other_half_width, other_half_height) + p.first, 					//upper right
				vec2(other_half_width * -1.0f, other_half_height * -1.0f) + p.first,	//lower left	
				vec2(other_half_width, other_half_height * -1.0f) + p.first				//lower right
			};

			if (jep::pointInPolygon(other_bounding_points, i))
				return false;
		}
	}

	return true;
}

void display_wall::addArtwork(const vec2 &position, artwork to_add) 
{
	mat4 local_translation_matrix = glm::translate(mat4(1.0f), vec3(position.x, position.y, 0.0f));
	to_add.setModelMatrix(wall_model_matrix * local_translation_matrix);
	shared_ptr<artwork> to_add_ptr(new artwork(to_add));
	wall_contents.push_back(pair<vec2, shared_ptr<artwork> >(position, to_add_ptr));
}

bool display_wall::removeArtwork(const shared_ptr<artwork> &to_remove)
{
	vector< pair<vec2, shared_ptr<artwork> > >::iterator found = wall_contents.end();
	for (vector< pair<vec2, shared_ptr<artwork> > >::iterator it = wall_contents.begin(); it != wall_contents.end(); it++)
	{
		if ((*it).second->getData()->getID() == to_remove->getData()->getID())
		{
			found = it;
			break;
		}
	}

	if (found == wall_contents.end())
		return false;

	else
	{
		wall_contents.erase(found);
		return true;
	}
}

void display_wall::draw(const shared_ptr<ogl_context> &context, const shared_ptr<ogl_camera> &camera)
{
	for (auto i : lines)
		i->draw(context, camera);

	for (auto i : wall_contents)
		i.second->draw(context, camera);

	shared_ptr<GLuint> temp_vao = opengl_data->getVAO();
	shared_ptr<GLuint> temp_vbo = opengl_data->getVBO();
	shared_ptr<GLuint> temp_tex = opengl_data->getTEX();

	glBindVertexArray(*temp_vao);
	glBindTexture(GL_TEXTURE_2D, *temp_tex);

	//TODO modify values passed to be more explicit in code (currently enumerated in ogl_tools)
	//wall_model_matrix = glm::translate(mat4(1.0f), vec3(0.0f, 0.0f, 0.0f));
	camera->setMVP(context, wall_model_matrix, (render_type)0);

	glDrawArrays(GL_TRIANGLES, 0, opengl_data->getVertexCount());
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindVertexArray(0);
}

bool display_wall::isClicked(shared_ptr<key_handler> &keys, const shared_ptr<ogl_camera> &camera,
	const pair<vec3, vec3> &ray, float &scale)
{
	mat4 inverse_model_matrix = glm::inverse(wall_model_matrix);

	vec3 origin = vec3(inverse_model_matrix * vec4(ray.first, 1.0f));
	vec3 direction = vec3(inverse_model_matrix * vec4(ray.second, 1.0f)) - origin;

	//cycle through each surface, testing ray intersection
	for (auto i : wall_triangles)
	{
		if (i.size() != 3)
		{
			cout << "surface tested is missing vertices" << endl;
			return false;
		}

		vec3 A(i.at(0));
		vec3 B(i.at(1));
		vec3 C(i.at(2));
		vec3 result;

		if (glm::intersectRayTriangle(origin, direction, A, B, C, result))
		{
			vec3 sum_result = (A * (1.0f - result.x - result.y)) + (B * result.x) + (C * result.y);
			click_position = vec2(sum_result.x, sum_result.y);
			scale = result.z;
			return true;
		}
	}
	return false;
}

gallery::gallery(const shared_ptr<ogl_context> &context, string model_path, string material_path, 
	string display_model_filename, string filler_model_filename, string display_material_filename, string filler_material_filename)
{
	string display_model_path = model_path + display_model_filename;
	string filler_model_path = model_path + filler_model_filename;
	string display_material_path = material_path + display_material_filename;
	string filler_material_path = material_path + filler_material_filename;

	vector<mesh_data> display_wall_meshes = generateMeshes(display_model_path.c_str());
	map<string, material_data> display_wall_materials = generateMaterials(display_material_path.c_str());

	int display_wall_counter = 0;
	for (auto i : display_wall_meshes)
	{
		string full_texture_path = material_path + display_wall_materials.at(i.getMaterialName()).getTextureFilename();

		shared_ptr<display_wall> new_wall(new display_wall(
			context,
			full_texture_path,
			i.getInterleaveData(),
			0,
			i.getInterleaveVNOffset(),
			i.getInterleaveVTOffset(),
			i.getInterleaveStride()));

		display_walls.insert(pair<int, shared_ptr<display_wall> >(display_wall_counter++, new_wall));
	}

	//TODO add code for filler geometry
	//TODO add code for floor model
}

void gallery::renderGallery(const shared_ptr<ogl_context> &context, const shared_ptr<ogl_camera> &camera) const
{
	for (auto i : display_walls)
		i.second->draw(context, camera);

	for (auto i : lines)
		i->draw(context, camera);
}

shared_ptr<display_wall> gallery::checkWallClicks(shared_ptr<key_handler> &keys, const shared_ptr<ogl_camera> &camera, float &distance)
{
	pair<vec3, vec3> ray(getRayFromCursorPosition(keys, camera));

	pair<float, shared_ptr<display_wall> >wall_clicked(0.0f, nullptr);
	float wall_distance = 0.0f;
	for (auto i : display_walls)
	{
		if (i.second->isClicked(keys, camera, ray, wall_distance))
		{
			if (wall_clicked.second == nullptr || (wall_clicked.second != nullptr && wall_distance < wall_clicked.first))
				wall_clicked = pair<float, shared_ptr<display_wall> >(wall_distance, i.second);
		}
	}

	distance = wall_clicked.first;
	return wall_clicked.second;
}

shared_ptr<artwork> gallery::checkArtworkClicks(shared_ptr<key_handler> &keys, const shared_ptr<ogl_camera> &camera, float &distance)
{
	pair<vec3, vec3> ray(getRayFromCursorPosition(keys, camera));

	pair<float, shared_ptr<artwork> >artwork_clicked(0.0f, nullptr);
	float artwork_distance = 0.0f;
	for (auto i : display_walls)
	{
		vector< pair<vec2, shared_ptr<artwork> > > wall_contents = i.second->getWallContents();
		for (auto j : wall_contents)
		{
			if (j.second->isClicked(keys, camera, ray, artwork_distance))
			{
				if (artwork_clicked.second == nullptr || (artwork_clicked.second != nullptr && artwork_distance < artwork_clicked.first))
					artwork_clicked = pair<float, shared_ptr<artwork> >(artwork_distance, j.second);
			}
		}
	}

	distance = artwork_clicked.first;
	return artwork_clicked.second;
}

void gallery::removeArtwork(const shared_ptr<artwork> &to_remove)
{
	for (auto i : display_walls)
	{
		if (i.second->removeArtwork(to_remove))
			return;
	}
}