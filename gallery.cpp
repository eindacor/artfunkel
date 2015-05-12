#include "gallery.h"
#include "artwork.h"
#include "utility_funcs.h"

//dislay walls first need their vertex data modified to be flat on the z-axis, so raytracing only needs to adjust on click, instead of 
//transforming all surfaces per click
display_wall::display_wall(const shared_ptr<ogl_context> &context, string texture_path, 
	const vector<float> &vec_vertices, int geometry_offset, int normal_offset, int uv_offset, int stride)
{
	//TODO make all necessary adjustments to vec_vertices before extracting wall_triangles for the first time
	wall_triangles = getTriangles(vec_vertices, geometry_offset, stride);

	if (wall_triangles.size() == 0)
		throw;

	vector<vec3> anchor_triangle(wall_triangles.at(0));
	vec3 anchor_point = anchor_triangle.at(0);
	//TODO test to see if anchor_point * -1.0f works as well;
	mat4 adjustment_position_matrix = glm::translate(mat4(1.0f), vec3(anchor_point.x * -1.0f, anchor_point.y * -1.0f, anchor_point.z * -1.0f));

	float normal_rotation = getNormalRotation(vec_vertices, normal_offset, stride);
	//offset 270 since default normal position is +z, which is directly south in plan view
	//offset 90 degrees because wall surface must be perpendicular to normal direction
	float surface_rotation = normal_rotation - 90.0f;
	cout << "surface_rotation: " << surface_rotation << endl;
	mat4 adjustment_rotation_matrix = glm::rotate(mat4(1.0f), surface_rotation * -1.0f, vec3(0.0f, 1.0f, 0.0f));

	//this matrix modifies the modeled geometry to 
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

	wall_model_matrix = glm::inverse(adjustment_matrix);
	wall_edges = getOuterEdges(wall_triangles);

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

void display_wall::draw(const shared_ptr<ogl_context> &context, const shared_ptr<ogl_camera> &camera)
{
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

bool display_wall::wallClicked(shared_ptr<key_handler> &keys, const shared_ptr<ogl_camera> &camera)
{
	pair<vec3, vec3> ray(getRayFromCursorPosition(keys, camera));

	mat4 inverse_model_matrix = glm::inverse(wall_model_matrix);

	ray.first = vec3(inverse_model_matrix * vec4(ray.first, 1.0f));
	ray.second = vec3(inverse_model_matrix * vec4(ray.second, 1.0f));

	vec3 origin = ray.first;
	vec3 direction = ray.second - ray.first;

	//cycle through each surface, testing ray intersection
	for (auto i : wall_triangles)
	{
		if (i.size() != 3)
		{
			cout << "surface tested is missing vertices" << endl;
			return false;
		}

		vec3 first_point(i.at(1));
		vec3 second_point(i.at(0));
		vec3 third_point(i.at(2));
		vec3 result;

		if (glm::intersectRayTriangle(origin, direction, first_point, second_point, third_point, result))
			return true;
	}

	return false;
}

/*
vec2 display_wall::getCursorLocationOnWall(shared_ptr<key_handler> &keys, const shared_ptr<ogl_camera> &camera) const
{
	pair<vec3, vec3> ray(getRayFromCursorPosition(keys, camera));

	mat4 inverse_wall_model_matrix = glm::inverse(wall_model_matrix);

	ray.first = vec3(inverse_wall_model_matrix * vec4(ray.first, 1.0f));
	ray.second = vec3(inverse_wall_model_matrix * vec4(ray.second, 1.0f));

	vec3 origin = ray.first;
	vec3 direction = ray.second - ray.first;

	vector< vector<vec3> > select_surfaces = art->getSelectSurfaces();

	//cycle through each surface, testing ray intersection
	for (auto i : select_surfaces)
	{
		if (i.size() != 3)
		{
			cout << "surface tested is missing vertices" << endl;
			return false;
		}

		vec3 first_point(i.at(1));
		vec3 second_point(i.at(0));
		vec3 third_point(i.at(2));
		vec3 result;

		if (glm::intersectRayTriangle(origin, direction, first_point, second_point, third_point, result))
		{

		}
	}
}
*/

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

void gallery::addPainting(int index, const shared_ptr<artwork> &work)
{
	/*
	//TODO check to verify painting position is not occupied
	//creates a copy of the instance passed, so the translation matrix applied doesn't affect the original
	works_displayed[index] = shared_ptr<artwork>(new artwork(*work));

	//center on eye level, unless painting is within .5 of floor
	float eye_level = 1.65f;
	float y_offset = 0.0f;
	float min_distance_from_floor = .5f;
	if ((works_displayed[index]->getData()->getHeight() * .0067f) + min_distance_from_floor > eye_level)
		y_offset = (works_displayed[index]->getData()->getHeight() / 200.0f) + min_distance_from_floor;

	else y_offset = eye_level - (works_displayed[index]->getData()->getHeight() / 600.0f);

	works_displayed[index]->moveRelative(glm::translate(mat4(1.0f), vec3(0.0f, y_offset, 0.0f)));
	works_displayed[index]->moveRelative(work_positions[index]);
	*/
}