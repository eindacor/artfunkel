#include "gallery.h"
#include "artwork.h"
#include "utility_funcs.h"

//dislay walls first need their vertex data modified to be flat on the z-axis, so raytracing only needs to modify the ray created for click
//detection, rather than matrix-translating all of the walls on-click

display_wall::display_wall(const shared_ptr<ogl_context> &context, mesh_data mesh, 
	const shared_ptr<GLuint> &TEX, string texture_filename, unsigned short index)
{
	tex_filename = texture_filename;
	wall_index = index;
	vector<float> vec_vertices = mesh.getInterleaveData();

	//TODO make data collection more robust for bad model files

	if (vec_vertices.size() < 3)
		throw;

	vec3 anchor_point(vec_vertices.at(0), vec_vertices.at(1), vec_vertices.at(2));
	mat4 adjustment_position_matrix = glm::translate(mat4(1.0f), anchor_point * -1.0f);

	float normal_rotation = getNormalRotation(vec_vertices, mesh.getInterleaveVNOffset(), mesh.getInterleaveStride());
	float surface_rotation = normal_rotation - 90.0f;
	mat4 adjustment_rotation_matrix = glm::rotate(mat4(1.0f), surface_rotation * -1.0f, vec3(0.0f, 1.0f, 0.0f));

	//TODO currently rotating the mesh modifies the positions but does not modify the normals
	//TODO these should stay related, find a better way to appropriately render normals
	mesh.modifyPosition(adjustment_position_matrix);
	mesh.rotate(adjustment_rotation_matrix);

	//this matrix modifies the modeled geometry to be on the z-axis for easy click intersection detection
	mat4 adjustment_matrix = adjustment_rotation_matrix * adjustment_position_matrix;

	vector<unsigned short> vertex_indices;
	vector<float> modified_vec_vertices =  mesh.getIndexedVertexData(vertex_indices);
	
	//for each vector of vec3's in wall_triangles
	wall_triangles = mesh.getMeshTrianglesVec3();

	//wall model matrix is the inverse of the original adjustment, returning it to its proper location when drawn
	wall_model_matrix = glm::inverse(adjustment_matrix);
	//wall_model_matrix = glm::translate(mat4(1.0f), vec3(0.0f, 0.0f, 0.0f));
	wall_edges = mesh.getMeshEdgesVec3();

	//TODO remove lines once graphics are improved, for visual clarity only
	for (const auto &i : wall_edges)
	{
		vec4 first(vec4(i.first, 1.0f));
		vec4 second(vec4(i.second, 1.0f));
		lines.push_back(shared_ptr<line>(new line(wall_model_matrix * first, wall_model_matrix * second, vec4(0.0f, 0.0f, 0.0f, 1.0f))));
	}

	//vec_vertices need to be modified before passing to GPU
	opengl_data = shared_ptr<jep::ogl_data>(new jep::ogl_data(
		context,
		TEX,
		GL_STATIC_DRAW,
		vertex_indices,
		modified_vec_vertices,
		mesh.getVSize(),
		mesh.getVTSize(),
		mesh.getVNSize()
		));
}

bool display_wall::validPlacement(const shared_ptr<artwork> &placed, const vec2 &position)
{
	vec3 dimensions(placed->getOverallDimensions());
	float width(dimensions.x);
	float height(dimensions.y);

	float half_width(width / 2.0f);
	float half_height(height / 2.0f);

	vector<vec2> placed_points_to_check{
		vec2(position.x - half_width, position.y + half_height),		//upper left
		vec2(position.x + half_width, position.y + half_height), 		//upper right
		vec2(position.x - half_width, position.y - half_height),		//lower left	
		vec2(position.x + half_width, position.y - half_height)			//lower right
	};

	//TODO verify that painting edges don't intersect, in addition to four corners not being within other paintings
	for (const auto &i : placed_points_to_check)
	{
		//break;
		//verifies paintings is within bounds of wall
		if (!jep::pointInPolygon(wall_edges, i))
			return false;

		//verifies painting does not collide with other paintings
		for (const auto &p : wall_contents)
		{
			vec3 other_dimensions = p.second->getOverallDimensions();
			float other_half_width = other_dimensions.x / 2.0f;
			float other_half_height = other_dimensions.y / 2.0f;
			vec2 other_centerpoint = p.first;

			vector<vec2> other_bounding_points{
				vec2(other_centerpoint.x - other_half_width, other_centerpoint.y + other_half_height),		//upper left
				vec2(other_centerpoint.x + other_half_width, other_centerpoint.y + other_half_height), 		//upper right
				vec2(other_centerpoint.x - other_half_width, other_centerpoint.y - other_half_height),		//lower left	
				vec2(other_centerpoint.x + other_half_width, other_centerpoint.y - other_half_height)		//lower right
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
	//for (const auto &i : lines)
		//i->draw(context, camera);

	for (const auto &i : wall_contents)
		i.second->draw(context, camera);

	glBindVertexArray(*(opengl_data->getVAO()));
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glBindTexture(GL_TEXTURE_2D, *(opengl_data->getTEX()));

	//TODO modify values passed to be more explicit in code (currently enumerated in ogl_tools)
	camera->setMVP(context, wall_model_matrix, (render_type)0);

	//TODO try removing this line
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *(opengl_data->getIND()));

	//glDrawArrays(GL_TRIANGLES, 0, opengl_data->getVertexCount());
	glDrawElements(GL_TRIANGLES, opengl_data->getIndexCount(), GL_UNSIGNED_SHORT, (void*)0);
	glBindTexture(GL_TEXTURE_2D, 0);
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void display_wall::setTexture(string texture_filename, const shared_ptr<texture_handler> &textures)
{
	if (textures->getTexture(texture_filename) != nullptr)
	{
		opengl_data->overrideTEX(textures->getTexture(texture_filename));
		tex_filename = texture_filename;
	}
}

bool display_wall::cursorTouches(shared_ptr<key_handler> &keys, const shared_ptr<ogl_camera> &camera,
	const pair<vec3, vec3> &ray, float &scale)
{
	mat4 inverse_model_matrix = glm::inverse(wall_model_matrix);

	vec3 origin = vec3(inverse_model_matrix * vec4(ray.first, 1.0f));
	vec3 direction = vec3(inverse_model_matrix * vec4(ray.second, 1.0f)) - origin;

	//cycle through each surface, testing ray intersection
	for (const auto &i : wall_triangles)
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
			cursor_position = vec2(sum_result.x, sum_result.y);
			scale = result.z;
			return true;
		}
	}
	return false;
}

gallery::gallery(const shared_ptr<ogl_context> &context, shared_ptr<texture_handler> &textures, string model_path, string material_path,
	string template_name_string, string owner_name, string gallery_name)
{
	template_name = template_name_string;
	owner = owner_name;
	max_artwork_count = 64;

	string display_model_path = model_path + template_name_string + "_display.obj";
	string filler_model_path = model_path + template_name_string + "_filler.obj";
	string display_material_path = material_path + template_name_string + "_display.mtl";
	string filler_material_path = material_path + template_name_string + "_filler.mtl";

	vector<mesh_data> display_wall_meshes = generateMeshes(display_model_path.c_str());
	map<string, material_data> display_wall_materials = generateMaterials(display_material_path.c_str());

	unsigned short display_wall_counter = 0;

	for (const auto &i : display_wall_meshes)
	{
		//string full_texture_path = material_path + display_wall_materials.at(i.getMaterialName()).getTextureFilename();
		string texture_filename = display_wall_materials.at(i.getMaterialName()).getTextureFilename();
		if (textures->getTexture(texture_filename) == nullptr)
			textures->addTexture(texture_filename);
		shared_ptr<display_wall> new_wall(new display_wall(context, i, textures->getTexture(texture_filename), 
			texture_filename, display_wall_counter));
		display_walls.insert(pair<unsigned short, shared_ptr<display_wall> >(display_wall_counter++, new_wall));
	}

	vector<mesh_data> environment_meshes = generateMeshes(filler_model_path.c_str());
	map<string, material_data> environment_materials = generateMaterials(filler_material_path.c_str());

	for (const auto &i : environment_meshes)
	{
		vector<unsigned short> mesh_indices;
		vector<float> vertex_data = i.getIndexedVertexData(mesh_indices);
		//string full_texture_path = material_path + environment_materials.at(i.getMaterialName()).getTextureFilename();
		string texture_filename = environment_materials.at(i.getMaterialName()).getTextureFilename();
		if (textures->getTexture(texture_filename) == nullptr)
			textures->addTexture(texture_filename);
		shared_ptr<jep::ogl_data> env_mesh(new jep::ogl_data(
			context,
			textures->getTexture(texture_filename),
			GL_STATIC_DRAW,
			mesh_indices,
			vertex_data,
			i.getVSize(),
			i.getVTSize(),
			i.getVNSize()
			));

		environment_mesh_data.push_back(env_mesh);
	}

	bool draw_grid = false;

	if (draw_grid)
	{
		int lines_x = 10;
		int lines_z = 10;
		float line_spacing = 4.0f;
		float x_start = ((float)lines_x * line_spacing) / -2.0f;
		float z_start = ((float)lines_z * line_spacing) / -2.0f;
		for (int i = 0; i <= lines_x; i++)
		{
			float z_end = z_start * -1.0f;
			vec4 start(((float)i * line_spacing) + x_start, 0.0f, z_start, 1.0f);
			vec4 end(((float)i * line_spacing) + x_start, 0.0f, z_end, 1.0f);
			lines.push_back(shared_ptr<line>(new line(start, end, vec4(0.1f, 0.1f, 0.1f, 0.5f))));
		}

		for (int i = 0; i <= lines_z; i++)
		{
			float x_end = x_start * -1.0f;
			vec4 start(x_start, 0.0f, ((float)i * line_spacing) + z_start, 1.0f);
			vec4 end(x_end, 0.0f, ((float)i * line_spacing) + z_start, 1.0f);
			//lines.push_back(shared_ptr<line>(new line(start, end, vec4(0.1f, 0.1f, 0.1f, 0.5f))));
		}

		vec4 origin_start(0.0f, 1.0f, 0.0f, 1.0f);
		vec4 origin_end(0.0f, -1.0f, 0.0f, 1.0f);
		//lines.push_back(shared_ptr<line>(new line(origin_start, origin_end, vec4(0.1f, 0.1f, 0.1f, 0.5f))));
	}

	//TODO add code for filler geometry
	//TODO add code for floor model
}

void gallery::renderGallery(const shared_ptr<ogl_context> &context, const shared_ptr<ogl_camera> &camera) const
{
	for (const auto &i : display_walls)
		i.second->draw(context, camera);

	for (const auto &mesh : environment_mesh_data)
	{
		glBindVertexArray(*(mesh->getVAO()));
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(2);
		glBindTexture(GL_TEXTURE_2D, *(mesh->getTEX()));

		//TODO modify values passed to be more explicit in code (currently enumerated in ogl_tools)
		camera->setMVP(context, mat4(1.0f), (render_type)0);

		//TODO try removing this line
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *(mesh->getIND()));

		//glDrawArrays(GL_TRIANGLES, 0, opengl_data->getVertexCount());
		glDrawElements(GL_TRIANGLES, mesh->getIndexCount(), GL_UNSIGNED_SHORT, (void*)0);
		glBindTexture(GL_TEXTURE_2D, 0);

		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
		glDisableVertexAttribArray(2);
		glBindVertexArray(0);
	}

	//for (const auto &i : lines)
		//i->draw(context, camera);
}

shared_ptr<display_wall> gallery::getClosestWallUnderCursor(shared_ptr<key_handler> &keys, const shared_ptr<ogl_camera> &camera, float &distance)
{
	pair<vec3, vec3> ray(getRayFromCursorPosition(keys, camera));

	pair<float, shared_ptr<display_wall> >wall_clicked(0.0f, nullptr);
	float wall_distance = 0.0f;
	for (const auto &i : display_walls)
	{
		if (i.second->cursorTouches(keys, camera, ray, wall_distance))
		{
			if (wall_clicked.second == nullptr || (wall_clicked.second != nullptr && wall_distance < wall_clicked.first))
				wall_clicked = pair<float, shared_ptr<display_wall> >(wall_distance, i.second);
		}
	}

	distance = wall_clicked.first;
	return wall_clicked.second;
}

shared_ptr<artwork> gallery::getClosestArtworkUnderCursor(shared_ptr<key_handler> &keys, const shared_ptr<ogl_camera> &camera, float &distance)
{
	pair<vec3, vec3> ray(getRayFromCursorPosition(keys, camera));

	pair<float, shared_ptr<artwork> >artwork_clicked(0.0f, nullptr);
	float artwork_distance = 0.0f;
	for (const auto &i : display_walls)
	{
		vector< pair<vec2, shared_ptr<artwork> > > wall_contents = i.second->getWallContents();
		for (const auto &j : wall_contents)
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

void gallery::addArtwork(int wall_index, const shared_ptr<artwork> &toAdd, vec2 position)
{
	if (display_walls.find(wall_index) != display_walls.end())
	{
		gallery_value += toAdd->getValue();
		display_walls.at(wall_index)->addArtwork(position, *toAdd);
	}

	calcAttributeTotals();
}

void gallery::removeArtwork(const shared_ptr<artwork> &to_remove)
{
	for (const auto &i : display_walls)
	{
		if (i.second->removeArtwork(to_remove))
		{
			gallery_value -= to_remove->getValue();
			calcAttributeTotals();
			return;
		}
	}
}

shared_ptr<display_wall> gallery::getWall(int index)
{
	if (display_walls.find(index) != display_walls.end())
		return display_walls.at(index);
	else return nullptr;
}

//painting id, position, wall index
const map< unsigned, pair<vec2, unsigned short> > gallery::getWorkMap() const
{
	map< unsigned, pair<vec2, unsigned short> > all_works;

	for (int i = 0; i < display_walls.size(); i++)
	{
		//TODO make const
		vector< pair<vec2, shared_ptr<artwork> > > wall_contents = display_walls.at(i)->getWallContents();
		for (auto &work_info : wall_contents)
		{
			unsigned short wall_index = i;
			vec2 local_position = work_info.first;
			unsigned work_id = work_info.second->getData()->getID();

			pair<vec2, unsigned> wall_pair(local_position, wall_index);

			pair< unsigned short, pair<vec2, unsigned> > toAdd(work_id, wall_pair);
			all_works.insert(all_works.end(), toAdd);
		}
	}

	return all_works;
}

int gallery::getArtworkCount() const
{
	int count = 0;
	for (const auto &wall : display_walls)
		count += wall.second->getWallContents().size();

	cout << "gellery count: " << count << endl;
	return count;
}

void gallery::calcAttributeTotals()
{
	attribute_totals.clear();

	for (const auto &wall : display_walls)
	{
		for (const auto &work : wall.second->getWallContents())
		{
			map<artwork_attribute, float> work_atts = work.second->getWorkAttributes().getAttributes();

			for (const auto &att : work_atts)
			{
				if (attribute_totals.find(att.first) == attribute_totals.end())
					attribute_totals[att.first] = att.second;

				else attribute_totals.at(att.first) += att.second;
			}
		}
	}
}

void gallery::printAttributeTotals() const
{
	string attributes_string = "";
	for (const auto &att : attribute_totals)
		attributes_string += stringFromAttribute(att.first) + ": " + std::to_string(att.second) + "\n";

	cout << attributes_string << endl;
}