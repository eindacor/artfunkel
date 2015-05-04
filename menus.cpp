#include "header.h"
#include "geometry.h"
#include "loot.h"
#include "player.h"
#include "artwork.h"
#include "utility_funcs.h"
#include "menus.h"

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

int viewInventory(string data_path, const shared_ptr<ogl_context> &context, 
	const shared_ptr<key_handler> &keys, const shared_ptr<player> &current_player)
{
	vec4 original_background = context->getBackgroundColor();
	context->setBackgroundColor(vec4(0.5f, 0.0f, 0.0f, 1.0f));

	//TODO revise so function doesn't rely on so many containers created/copied per run
	//add copies of the artwork instances to the local vector, so position can be manipulated
	vector< shared_ptr<artwork_instance> > paintings_to_display = current_player->getInventoryCopy();

	//space artwork without in the x axis only
	offsetArtworks(paintings_to_display, 0.0f, 0.0f, true);

	//add player's default frames to each
	for (auto i : paintings_to_display)
		i->loadFrame(current_player->getDefaultFrame());

	//int indicates index of painting, bool indicates whether that work is "set" or not
	map<int, pair< shared_ptr<artwork_instance>, bool> > artwork_map;

	for (int i = 0; i < paintings_to_display.size(); i++)
	{
		//TODO Clean up!
		pair< int, pair< shared_ptr<artwork_instance>, bool> > 
			map_entry(i, pair< shared_ptr<artwork_instance>, bool>(paintings_to_display.at(i), false));
		artwork_map.insert(map_entry);
	}

	float camera_distance_from_items = 10.0f;
	shared_ptr<ogl_camera> camera(new ogl_camera(keys, vec3(0.0f, 0.0f, camera_distance_from_items), vec3(0.0f, 0.0f, 0.0f)));

	int current_selection = 0;
	glfwSetTime(0);
	float render_fps = 10.0f;
	bool finished = false;
	int menu_return = 0;

	while (!finished)
	{
		if (glfwGetTime() > 1.0f / render_fps)
		{
			glfwPollEvents();
			context->clearBuffers();

			if (keys->checkPress(GLFW_KEY_ESCAPE))
			{
				menu_return = mainMenu(data_path, context, keys);
				if (menu_return != 1)
				{
					finished = true;
					break;
				}
			}

			if (keys->checkPress(GLFW_KEY_LEFT) && current_selection != 0)
				current_selection--;

			else if (keys->checkPress(GLFW_KEY_RIGHT) && current_selection != (artwork_map.size() - 1))
				current_selection++;

			float current_selection_height = artwork_map.at(current_selection).first->getHeight();
			float current_selection_width = artwork_map.at(current_selection).first->getWidth();
			camera_distance_from_items = (current_selection_height > current_selection_width ? 
				current_selection_height : current_selection_width);

			vec4 camera_target = artwork_map.at(current_selection).first->getCenter();
			camera->setFocus(vec3(camera_target.x, camera_target.y, camera_target.z));
			camera->setFocus(vec3(camera_target.x, camera_target.y, camera_target.z + camera_distance_from_items));
			camera->updateCamera();

			for (auto i : artwork_map)
				i.second.first->draw(context, camera);

			if (keys->checkPress(GLFW_KEY_ENTER))
			{
				artwork_map.at(current_selection).second = !(artwork_map.at(current_selection).second);
				cout << artwork_map.at(current_selection).first->getTitle() << ": " << artwork_map.at(current_selection).second << endl;
			}

			context->swapBuffers();
			glfwSetTime(0.0f);
		}
	}

	context->setBackgroundColor(original_background);
	return menu_return;
}

int openCrate(string data_path, const shared_ptr<ogl_context> &context, const shared_ptr<key_handler> &keys, const shared_ptr<player> &current_player, const shared_ptr<loot_generator> &lg)
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
	options.insert(pair<int, pair<painting_surface, frame_model> >(0, pair<painting_surface, frame_model>(
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