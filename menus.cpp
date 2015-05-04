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
		painting_surface(menu_item_width, menu_item_height, context, (data_path + "images\\menu_art\\enter_gallery.bmp").c_str()),
		frame_model(menu_item_width, menu_item_height, context, (data_path + "model_data\\frame_bamboo.bmp").c_str(), (data_path + "model_data\\white_matte.bmp").c_str())
		)));

	options.insert(pair<int, pair<painting_surface, frame_model> >(1, pair<painting_surface, frame_model>(
		painting_surface(menu_item_width, menu_item_height, context, (data_path + "images\\menu_art\\view_inventory.bmp").c_str()),
		frame_model(menu_item_width, menu_item_height, context, (data_path + "model_data\\frame_bamboo.bmp").c_str(), (data_path + "model_data\\white_matte.bmp").c_str())
		)));

	options.insert(pair<int, pair<painting_surface, frame_model> >(2, pair<painting_surface, frame_model>(
		painting_surface(menu_item_width, menu_item_height, context, (data_path + "images\\menu_art\\open_crate.bmp").c_str()),
		frame_model(menu_item_width, menu_item_height, context, (data_path + "model_data\\frame_bamboo.bmp").c_str(), (data_path + "model_data\\white_matte.bmp").c_str())
		)));

	options.insert(pair<int, pair<painting_surface, frame_model> >(3, pair<painting_surface, frame_model>(
		painting_surface(menu_item_width, menu_item_height, context, (data_path + "images\\menu_art\\options.bmp").c_str()),
		frame_model(menu_item_width, menu_item_height, context, (data_path + "model_data\\frame_bamboo.bmp").c_str(), (data_path + "model_data\\white_matte.bmp").c_str())
		)));

	options.insert(pair<int, pair<painting_surface, frame_model> >(4, pair<painting_surface, frame_model>(
		painting_surface(menu_item_width, menu_item_height, context, (data_path + "images\\menu_art\\exit.bmp").c_str()),
		frame_model(menu_item_width, menu_item_height, context, (data_path + "model_data\\frame_bamboo.bmp").c_str(), (data_path + "model_data\\white_matte.bmp").c_str())
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

				i.second.first.draw(model_matrix, camera);
				i.second.second.draw(model_matrix, camera);
			}

			item_selected = keys->checkPress(GLFW_KEY_ENTER);

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
	map<int, shared_ptr<artwork_instance> > paintings_to_display = current_player->getInventoryCopy();

	//space artwork without in the x axis only
	offsetArtworks(paintings_to_display, 0.5f, 0.0f, 0.0f, true);

	//add player's default frames to each
	for (auto i : paintings_to_display)
		i.second->applyFrameTemplate(*(current_player->getDefaultFrame()));

	float camera_distance_from_items = 10.0f;
	shared_ptr<ogl_camera> camera(new ogl_camera(keys, vec3(0.0f, 0.0f, camera_distance_from_items), vec3(0.0f, 0.0f, 0.0f)));

	map<int, shared_ptr<artwork_instance> >::iterator current_selection = paintings_to_display.begin();
	map<int, shared_ptr<artwork_instance> >::iterator last_item = paintings_to_display.end();
	last_item--;
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

			if (keys->checkPress(GLFW_KEY_LEFT) && current_selection != paintings_to_display.begin())
				current_selection--;

			else if (keys->checkPress(GLFW_KEY_RIGHT) && current_selection != last_item)
				current_selection++;

			//TODO modify height/width of paintings to be world units instead of cm
			float current_selection_height = ((*current_selection).second->getHeight()) * 2.0f;
			float current_selection_width = ((*current_selection).second->getWidth()) * 2.0f;
			camera_distance_from_items = (current_selection_height > current_selection_width ?
			current_selection_height : current_selection_width);

			vec4 camera_target = (*current_selection).second->getCenter();
			camera->setFocus(vec3(camera_target.x, camera_target.y, camera_target.z));
			camera->setPosition(vec3(camera_target.x, camera_target.y, camera_distance_from_items));
			camera->updateCamera();

			for (auto i : paintings_to_display)
				i.second->draw(context, camera);

			if (keys->checkPress(GLFW_KEY_ENTER))
			{
				//if painting is already on display, take it off. if it's not, add it to display
				switch (current_player->isOnDisplay((*current_selection).first))
				{
				case true: current_player->removePaintingFromDisplay(*current_selection); 
					cout << (*current_selection).second->getTitle() << " has been removed from the gallery" << endl;
					break;
				case false: current_player->addPaintingToDisplay(*current_selection);
					cout << (*current_selection).second->getTitle() << " has been added to the gallery" << endl;
					break;
				}
			}

			context->swapBuffers();

			if (keys->checkPress(GLFW_KEY_ESCAPE))
			{
				menu_return = mainMenu(data_path, context, keys);
				if (menu_return != 1)
					finished = true;
			}

			glfwSetTime(0.0f);			
		}
	}

	context->setBackgroundColor(original_background);
	return menu_return;
}

int openCrate(string data_path, const shared_ptr<ogl_context> &context, const shared_ptr<key_handler> &keys, const shared_ptr<player> &current_player, const shared_ptr<loot_generator> &lg)
{
	vec4 original_background = context->getBackgroundColor();
	context->setBackgroundColor(vec4(0.0f, 0.0f, 0.5f, 1.0f));

	//TODO revise so function doesn't rely on so many containers created/copied per run
	//add copies of the artwork instances to the local vector, so position can be manipulated
	map<int, shared_ptr<artwork_instance> > paintings_to_display = lg->generateArtworks(10, 1.0f);

	//space artwork without in the x axis only
	offsetArtworks(paintings_to_display, 0.5f, 0.0f, 0.0f, true);

	//add player's default frames to each
	for (auto i : paintings_to_display)
		i.second->applyFrameTemplate(*(current_player->getDefaultFrame()));

	float camera_distance_from_items = 10.0f;
	shared_ptr<ogl_camera> camera(new ogl_camera(keys, vec3(0.0f, 0.0f, camera_distance_from_items), vec3(0.0f, 0.0f, 0.0f)));

	map<int, shared_ptr<artwork_instance> >::iterator current_selection = paintings_to_display.begin();
	map<int, shared_ptr<artwork_instance> >::iterator last_item = paintings_to_display.end();
	last_item--;
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

			if (keys->checkPress(GLFW_KEY_LEFT) && current_selection != paintings_to_display.begin())
				current_selection--;

			else if (keys->checkPress(GLFW_KEY_RIGHT) && current_selection != last_item)
				current_selection++;

			//TODO modify height/width of paintings to be world units instead of cm
			float current_selection_height = ((*current_selection).second->getHeight()) * 2.0f;
			float current_selection_width = ((*current_selection).second->getWidth()) * 2.0f;
			camera_distance_from_items = (current_selection_height > current_selection_width ?
			current_selection_height : current_selection_width);

			vec4 camera_target = (*current_selection).second->getCenter();
			camera->setFocus(vec3(camera_target.x, camera_target.y, camera_target.z));
			camera->setPosition(vec3(camera_target.x, camera_target.y, camera_distance_from_items));
			camera->updateCamera();

			for (auto i : paintings_to_display)
				i.second->draw(context, camera);

			if (keys->checkPress(GLFW_KEY_ENTER))
			{
				int selected_index = (*current_selection).second->getID();
				bool already_owned = current_player->alreadyOwned(selected_index);

				switch (already_owned)
				{
				case true: cout << (*current_selection).second->getTitle() << " is already in your inventory" << endl;
					break;
				case false: current_player->addWorkToInventory((*current_selection).second);
					cout << (*current_selection).second->getTitle() << " has been added to your inventory" << endl;
					break;
				}
			}

			if (keys->checkPress(GLFW_KEY_A))
			{
				for (auto i : paintings_to_display)
				{
					int selected_index = i.second->getID();
					bool already_owned = current_player->alreadyOwned(selected_index);

					switch (already_owned)
					{
					case true: cout << i.second->getTitle() << " is already in your inventory" << endl;
						break;
					case false: current_player->addWorkToInventory(i.second);
						cout << i.second->getTitle() << " has been added to your inventory" << endl;
						break;
					}
				}
			}

			context->swapBuffers();

			//TODO fix so crate doesn't disappear when going to the main menu
			if (keys->checkPress(GLFW_KEY_ESCAPE))
			{
				menu_return = mainMenu(data_path, context, keys);
				finished = true;
			}

			glfwSetTime(0.0f);
		}
	}

	context->setBackgroundColor(original_background);
	return menu_return;
}

int viewGallery(string data_path, const shared_ptr<ogl_context> &context, const shared_ptr<key_handler> &keys, const shared_ptr<player> &current_player)
{
	float eye_level = 1.65f;
	shared_ptr<ogl_camera> camera(new ogl_camera_free(keys, vec3(0.0f, eye_level, 5.0f)));

	vec4 original_background = context->getBackgroundColor();
	context->setBackgroundColor(vec4(0.5f, 0.5f, 0.5f, 1.0f));

	map<int, shared_ptr<artwork_instance> > paintings_to_display = current_player->getDisplayedCopy();
	offsetArtworks(paintings_to_display, eye_level);
	
	for (auto i : paintings_to_display)
		i.second->applyFrameTemplate(*(current_player->getDefaultFrame()));

	glfwSetTime(0);
	float render_fps = 60.0f;

	bool finished = false;
	int menu_return = 0;

	while (!finished)
	{
		if (glfwGetTime() > 1.0f / render_fps)
		{
			glfwPollEvents();
			context->clearBuffers();

			camera->updateCamera();
			for (auto i : paintings_to_display)
				i.second->draw(context, camera);

			context->swapBuffers();
			glfwSetTime(0.0f);

			if (keys->checkPress(GLFW_KEY_ESCAPE))
			{
				menu_return = mainMenu(data_path, context, keys);
				if (menu_return != 0)
					finished = true;
			}

			glfwSetTime(0.0f);
		}
	}

	for (auto i : paintings_to_display)
		i.second->unloadData();

	context->setBackgroundColor(original_background);
	return menu_return;
}