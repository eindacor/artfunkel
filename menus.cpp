#include "header.h"
#include "geometry.h"

int mainMenu(string data_path, const shared_ptr<ogl_context> &context, const shared_ptr<key_handler> &keys)
{
	bool item_selected = false;
	float camera_distance_from_items = 10.0f;
	float menu_item_offset = 3.0f;
	float menu_item_width = 2.0f, menu_item_height = 2.0f;

	vec4 original_background = context->getBackgroundColor();
	context->setBackgroundColor(vec4(0.0f, 0.0f, 0.0f, 1.0f));

	map<int, pair<painting_surface, frame_model> > options;

	shared_ptr<ogl_camera> camera(new ogl_camera(keys, vec3(0.0f, 0.0f, camera_distance_from_items), vec3(0.0f, 0.0f, 0.0f)));
	//TODO CLEAN UP THIS MESS 
	options.insert(pair<int, pair<painting_surface, frame_model> > (0, pair<painting_surface, frame_model>(
		painting_surface(menu_item_width * 100.0f, menu_item_height * 100.0f, context, camera, (data_path + "images\\menu_art\\enter_gallery.bmp").c_str()),
		frame_model(menu_item_width, menu_item_height, context, camera, (data_path + "model_data\\frame_bamboo.bmp").c_str(), (data_path + "model_data\\white_matte.bmp").c_str())
		)));

	options.insert(pair<int, pair<painting_surface, frame_model> >(1, pair<painting_surface, frame_model>(
		painting_surface(menu_item_width * 100.0f, menu_item_height * 100.0f, context, camera, (data_path + "images\\menu_art\\view_inventory.bmp").c_str()),
		frame_model(menu_item_width, menu_item_height, context, camera, (data_path + "model_data\\frame_bamboo.bmp").c_str(), (data_path + "model_data\\white_matte.bmp").c_str())
		)));

	options.insert(pair<int, pair<painting_surface, frame_model> >(2, pair<painting_surface, frame_model>(
		painting_surface(menu_item_width * 100.0f, menu_item_height * 100.0f, context, camera, (data_path + "images\\menu_art\\open_crate.bmp").c_str()),
		frame_model(menu_item_width, menu_item_height, context, camera, (data_path + "model_data\\frame_bamboo.bmp").c_str(), (data_path + "model_data\\white_matte.bmp").c_str())
		)));

	options.insert(pair<int, pair<painting_surface, frame_model> >(3, pair<painting_surface, frame_model>(
		painting_surface(menu_item_width * 100.0f, menu_item_height * 100.0f, context, camera, (data_path + "images\\menu_art\\options.bmp").c_str()),
		frame_model(menu_item_width, menu_item_height, context, camera, (data_path + "model_data\\frame_bamboo.bmp").c_str(), (data_path + "model_data\\white_matte.bmp").c_str())
		)));

	options.insert(pair<int, pair<painting_surface, frame_model> >(4, pair<painting_surface, frame_model>(
		painting_surface(menu_item_width * 100.0f, menu_item_height * 100.0f, context, camera, (data_path + "images\\menu_art\\exit.bmp").c_str()),
		frame_model(menu_item_width, menu_item_height, context, camera, (data_path + "model_data\\frame_bamboo.bmp").c_str(), (data_path + "model_data\\white_matte.bmp").c_str())
		)));

	int current_selection = 0;
	glfwSetTime(0);
	float render_fps = 10.0f;
	while (!item_selected)
	{
		if (glfwGetTime() > 1.0f / render_fps)
		{
			glfwPollEvents();
			context->clearBuffers();

			if (keys->checkPress(GLFW_KEY_LEFT) && current_selection != 0)
				current_selection--;

			else if (keys->checkPress(GLFW_KEY_RIGHT) && current_selection != (options.size() - 1))
				current_selection++;

			for (auto i : options)
			{
				float x_offset = (float(current_selection) - float(i.first)) * menu_item_offset * -1.0f;
				mat4 model_matrix(glm::translate(mat4(1.0f), vec3(x_offset, 0.0f, 0.0f)));
				if (i.first == current_selection)
					model_matrix = glm::scale(mat4(1.0f), vec3(1.3f, 1.3f, 1.3f));

				i.second.first.draw(model_matrix);
				i.second.second.draw(model_matrix);
			}

			float camera_offset = (float)current_selection * menu_item_offset;
			camera->setPosition(vec3(camera_offset, 0.0f, camera_distance_from_items));
			camera->setFocus(vec3(camera_offset, 0.0f, 0.0f));
			camera->updateCamera();

			item_selected = keys->checkPress(GLFW_KEY_ENTER);

			//camera->updateCamera();

			context->swapBuffers();
			glfwSetTime(0.0f);
		}
	}

	context->setBackgroundColor(original_background);
	return current_selection;
}