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
	context->setBackgroundColor(vec4(0.0f, 0.2f, 0.1f, 1.0f));

	map<int, pair<painting_surface, frame_model> > options;

	shared_ptr<ogl_camera> camera(new ogl_camera(keys, context, vec3(0.0f, 0.0f, camera_distance_from_items), vec3(0.0f, 0.0f, 0.0f), 45.0f));
	//TODO CLEAN UP THIS MESS 
	options.insert(pair<int, pair<painting_surface, frame_model> > (0, pair<painting_surface, frame_model>(
		painting_surface(menu_item_width, menu_item_height, context, (data_path + "images\\menu_art\\enter_gallery.bmp").c_str()),
		frame_model(menu_item_width, menu_item_height, context, (data_path + "model_data\\frame_black.bmp").c_str(), (data_path + "model_data\\white_matte.bmp").c_str())
		)));

	options.insert(pair<int, pair<painting_surface, frame_model> >(1, pair<painting_surface, frame_model>(
		painting_surface(menu_item_width, menu_item_height, context, (data_path + "images\\menu_art\\view_inventory.bmp").c_str()),
		frame_model(menu_item_width, menu_item_height, context, (data_path + "model_data\\frame_black.bmp").c_str(), (data_path + "model_data\\white_matte.bmp").c_str())
		)));

	options.insert(pair<int, pair<painting_surface, frame_model> >(2, pair<painting_surface, frame_model>(
		painting_surface(menu_item_width, menu_item_height, context, (data_path + "images\\menu_art\\open_crate.bmp").c_str()),
		frame_model(menu_item_width, menu_item_height, context, (data_path + "model_data\\frame_black.bmp").c_str(), (data_path + "model_data\\white_matte.bmp").c_str())
		)));

	options.insert(pair<int, pair<painting_surface, frame_model> >(3, pair<painting_surface, frame_model>(
		painting_surface(menu_item_width, menu_item_height, context, (data_path + "images\\menu_art\\options.bmp").c_str()),
		frame_model(menu_item_width, menu_item_height, context, (data_path + "model_data\\frame_black.bmp").c_str(), (data_path + "model_data\\white_matte.bmp").c_str())
		)));

	options.insert(pair<int, pair<painting_surface, frame_model> >(4, pair<painting_surface, frame_model>(
		painting_surface(menu_item_width, menu_item_height, context, (data_path + "images\\menu_art\\exit.bmp").c_str()),
		frame_model(menu_item_width, menu_item_height, context, (data_path + "model_data\\frame_black.bmp").c_str(), (data_path + "model_data\\white_matte.bmp").c_str())
		)));

	int current_selection = 0;
	glfwSetTime(0);
	float render_fps = 60.0f;
	while (!item_selected)
	{
		if (glfwGetTime() > 1.0f / render_fps)
		{
			glfwPollEvents();
			context->clearBuffers();

			if (keys->checkPress(GLFW_KEY_LEFT, false) && current_selection != 0)
				current_selection--;

			else if (keys->checkPress(GLFW_KEY_RIGHT, false) && current_selection != (options.size() - 1))
				current_selection++;

			for (auto i : options)
			{
				float x_offset = (float(current_selection) - float(i.first)) * menu_item_offset * -1.0f;
				mat4 model_matrix(glm::translate(mat4(1.0f), vec3(x_offset, 0.0f, 0.0f)));

				if (i.first == current_selection)
				{
					model_matrix = glm::scale(mat4(1.0f), vec3(1.3f, 1.3f, 1.3f));
					i.second.first.draw(model_matrix, camera);
					i.second.second.draw(model_matrix, camera);			
				}

				else
				{
					glUniform1f(context->getShaderGLint("dim_factor"), 0.5f);
					i.second.first.draw(model_matrix, camera);
					i.second.second.draw(model_matrix, camera);
					glUniform1f(context->getShaderGLint("dim_factor"), 1.0f);
				}
			}

			item_selected = keys->checkPress(GLFW_KEY_ENTER, false);

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

	int display_count = 10;

	//remove inventory copy mechanic. use actual inventory container with active iterators
	//add copies of the artwork instances to the local vector, so position can be manipulated
	vector<pair<int, shared_ptr<artwork> > > inventory_copy = current_player->getInventoryCopy();

	//add player's default frames to each
	for (auto i : inventory_copy)
		i.second->applyFrameTemplate(*(current_player->getDefaultFrame()));


	//take a chunk of the inventory paintings, using first, last, end iterators. update this vectore to go through pages of inventory
	vector<pair<int, shared_ptr<artwork> > >::const_iterator chunk_first = inventory_copy.begin();
	vector<pair<int, shared_ptr<artwork> > >::const_iterator chunk_last =
		findChunkLast(chunk_first, inventory_copy, display_count);
	vector<pair<int, shared_ptr<artwork> > >::const_iterator chunk_end =
		findChunkEnd(chunk_first, inventory_copy, display_count);

	vector<pair<int, shared_ptr<artwork> > > paintings_to_display;
	paintings_to_display.reserve(display_count);
	paintings_to_display.insert(paintings_to_display.begin(), chunk_first, chunk_end);

	//adjusts model matrices of each image
	makeThumbnails(paintings_to_display, 0.1f, 0.3f);

	shared_ptr<ogl_camera> camera(new ogl_camera(keys, context, vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 0.0f, 0.0f), 45.0f));

	vector<pair<int, shared_ptr<artwork> > >::iterator current_selection = paintings_to_display.begin();
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

			if (keys->checkPress(GLFW_KEY_LEFT, false))
			{
				if (current_selection == paintings_to_display.begin())
				{
					//verifies highlight does not return if on the last page
					int previous_distance = std::distance(chunk_first, inventory_copy.cbegin());

					//find new chunk_first based on previous chunk_first, find last and end iterators
					chunk_first = findChunkFirst(chunk_first, inventory_copy, display_count, false);

					int new_distance = std::distance(chunk_first, inventory_copy.cbegin());

					//if the page is refreshed
					if (previous_distance != new_distance)
					{
						chunk_last = findChunkLast(chunk_first, inventory_copy, display_count);
						chunk_end = findChunkEnd(chunk_first, inventory_copy, display_count);

						//clear paintings vec, get new paintings from inventory copy
						paintings_to_display.clear();
						paintings_to_display.insert(paintings_to_display.begin(), chunk_first, chunk_end);

						//space artwork without in the x axis only
						makeThumbnails(paintings_to_display, 0.1f, 0.3f);

						current_selection = paintings_to_display.end() - 1;
					}
				}

				else current_selection--;
			}

			else if (keys->checkPress(GLFW_KEY_RIGHT, false))
			{
				if (current_selection == paintings_to_display.end() - 1)
				{
					//verifies highlight does not return if on the last page
					int previous_distance = std::distance(chunk_first, inventory_copy.cbegin());

					//find new chunk_first based on previous chunk_first, find last and end iterators
					chunk_first = findChunkFirst(chunk_first, inventory_copy, display_count, true);

					int new_distance = std::distance(chunk_first, inventory_copy.cbegin());

					//if the page is refreshed
					if (previous_distance != new_distance)
					{
						chunk_last = findChunkLast(chunk_first, inventory_copy, display_count);
						chunk_end = findChunkEnd(chunk_first, inventory_copy, display_count);

						//clear paintings vec, get new paintings from inventory copy
						paintings_to_display.clear();
						paintings_to_display.insert(paintings_to_display.begin(), chunk_first, chunk_end);

						//space artwork without in the x axis only
						makeThumbnails(paintings_to_display, 0.1f, 0.3f);

						current_selection = paintings_to_display.begin();
					}
				}

				else current_selection++;
			}

			for (auto i : paintings_to_display)
			{
				if ((*current_selection).first != i.first)
				{
					glUniform1f(context->getShaderGLint("dim_factor"), 0.5f);
					i.second->draw(context, camera, true);
					glUniform1f(context->getShaderGLint("dim_factor"), 1.0f);
				}		

				else
				{
					i.second->draw(context, camera, true);

					shared_ptr<artwork> copy(new artwork(*(i.second)));
					mat4 original_matrix = copy->getModelMatrix();
					makeHighlight(copy, 0.1f, 0.5f, 2.5f);
					copy->draw(context, camera, true);
				}
			}

			if (keys->checkPress(GLFW_KEY_ENTER, false))
			{
				//if painting is already on display, take it off. if it's not, add it to display
				switch (current_player->isOnDisplay((*current_selection).first))
				{
				case true: current_player->removePaintingFromDisplay(*current_selection); 
					cout << (*current_selection).second->getData()->getTitle() << " has been removed from the gallery" << endl;
					break;
				case false: current_player->addPaintingToDisplay(*current_selection);
					cout << (*current_selection).second->getData()->getTitle() << " has been added to the gallery" << endl;
					break;
				}
			}

			if (keys->checkPress(GLFW_KEY_1, false))
			{
				current_selection = sortArtVec(inventory_copy, ARTIST_NAME);
				//take a chunk of the inventory paintings, using first, last, end iterators. update this vectore to go through pages of inventory
				chunk_first = inventory_copy.begin();
				chunk_last = findChunkLast(chunk_first, inventory_copy, display_count);
				chunk_end = findChunkEnd(chunk_first, inventory_copy, display_count);

				//clear paintings vec, get new paintings from inventory copy
				paintings_to_display.clear();
				paintings_to_display.insert(paintings_to_display.begin(), chunk_first, chunk_end);

				//space artwork without in the x axis only
				makeThumbnails(paintings_to_display, 0.1f, 0.3f);

				current_selection = paintings_to_display.begin();

				cout << endl << "-----Sorted by Artist-----" << endl;

				for (auto i : paintings_to_display)
					printArtwork(i.second);
			}

			if (keys->checkPress(GLFW_KEY_2, false))
			{
				current_selection = sortArtVec(inventory_copy, VALUE);
				//take a chunk of the inventory paintings, using first, last, end iterators. update this vectore to go through pages of inventory
				chunk_first = inventory_copy.begin();
				chunk_last = findChunkLast(chunk_first, inventory_copy, display_count);
				chunk_end = findChunkEnd(chunk_first, inventory_copy, display_count);

				//clear paintings vec, get new paintings from inventory copy
				paintings_to_display.clear();
				paintings_to_display.insert(paintings_to_display.begin(), chunk_first, chunk_end);

				//space artwork without in the x axis only
				makeThumbnails(paintings_to_display, 0.1f, 0.3f);

				current_selection = paintings_to_display.begin();

				cout << endl << "-----Sorted by Value-----" << endl;

				for (auto i : paintings_to_display)
					printArtwork(i.second);
			}

			if (keys->checkPress(GLFW_KEY_3, false))
			{
				current_selection = sortArtVec(inventory_copy, DATE);
				//take a chunk of the inventory paintings, using first, last, end iterators. update this vectore to go through pages of inventory
				chunk_first = inventory_copy.begin();
				chunk_last = findChunkLast(chunk_first, inventory_copy, display_count);
				chunk_end = findChunkEnd(chunk_first, inventory_copy, display_count);

				//clear paintings vec, get new paintings from inventory copy
				paintings_to_display.clear();
				paintings_to_display.insert(paintings_to_display.begin(), chunk_first, chunk_end);

				//space artwork without in the x axis only
				makeThumbnails(paintings_to_display, 0.1f, 0.3f);

				current_selection = paintings_to_display.begin();

				cout << endl << "-----Sorted by Date-----" << endl;

				for (auto i : paintings_to_display)
					printArtwork(i.second);
			}

			if (keys->checkPress(GLFW_KEY_4, false))
			{
				current_selection = sortArtVec(inventory_copy, RARITY);
				//take a chunk of the inventory paintings, using first, last, end iterators. update this vectore to go through pages of inventory
				chunk_first = inventory_copy.begin();
				chunk_last = findChunkLast(chunk_first, inventory_copy, display_count);
				chunk_end = findChunkEnd(chunk_first, inventory_copy, display_count);

				//clear paintings vec, get new paintings from inventory copy
				paintings_to_display.clear();
				paintings_to_display.insert(paintings_to_display.begin(), chunk_first, chunk_end);

				//space artwork without in the x axis only
				makeThumbnails(paintings_to_display, 0.1f, 0.3f);

				current_selection = paintings_to_display.begin();

				cout << endl << "-----Sorted by Rarity-----" << endl;

				for (auto i : paintings_to_display)
					printArtwork(i.second);
			}

			if (keys->checkPress(GLFW_KEY_5, false))
			{
				current_selection = sortArtVec(inventory_copy, TITLE);
				//take a chunk of the inventory paintings, using first, last, end iterators. update this vectore to go through pages of inventory
				chunk_first = inventory_copy.begin();
				chunk_last = findChunkLast(chunk_first, inventory_copy, display_count);
				chunk_end = findChunkEnd(chunk_first, inventory_copy, display_count);

				//clear paintings vec, get new paintings from inventory copy
				paintings_to_display.clear();
				paintings_to_display.insert(paintings_to_display.begin(), chunk_first, chunk_end);

				//space artwork without in the x axis only
				makeThumbnails(paintings_to_display, 0.1f, 0.3f);

				current_selection = paintings_to_display.begin();

				cout << endl << "-----Sorted by Title-----" << endl;

				for (auto i : paintings_to_display)
					printArtwork(i.second);
			}

			context->swapBuffers();

			if (keys->checkPress(GLFW_KEY_ESCAPE, false))
			{
				menu_return = mainMenu(data_path, context, keys);
				finished = (menu_return != 1);
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
	
	int drop_count = 6;

	//TODO revise so function doesn't rely on so many containers created/copied per run
	//add copies of the artwork instances to the local vector, so position can be manipulated
	vector<pair<int, shared_ptr<artwork> > > paintings_to_display = lg->generateArtworks(drop_count, 1.0f);

	//add player's default frames to each
	for (auto i : paintings_to_display)
		i.second->applyFrameTemplate(*(current_player->getDefaultFrame()));

	//adjusts model matrices of each image
	makeThumbnails(paintings_to_display, 0.1f, 0.3f);

	shared_ptr<ogl_camera> camera(new ogl_camera(keys, context, vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 0.0f, 0.0f), 45.0f));

	vector<pair<int, shared_ptr<artwork> > >::iterator current_selection = paintings_to_display.begin();
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

			if (keys->checkPress(GLFW_KEY_LEFT, false) && current_selection != paintings_to_display.begin())
				current_selection--;

			else if (keys->checkPress(GLFW_KEY_RIGHT, false) && current_selection != paintings_to_display.end() - 1)
				current_selection++;

			for (auto i : paintings_to_display)
			{
				if ((*current_selection).first != i.first)
				{
					glUniform1f(context->getShaderGLint("dim_factor"), 0.5f);
					i.second->draw(context, camera, true);
					glUniform1f(context->getShaderGLint("dim_factor"), 1.0f);
				}

				else
				{
					i.second->draw(context, camera, true);

					shared_ptr<artwork> copy(new artwork(*(i.second)));
					mat4 original_matrix = copy->getModelMatrix();
					makeHighlight(copy, 0.1f, 0.5f, 2.5f);
					copy->draw(context, camera, true);
				}
			}

			if (keys->checkPress(GLFW_KEY_ENTER, false))
			{
				int selected_index = (*current_selection).second->getData()->getID();
				bool already_owned = current_player->alreadyOwned(selected_index);

				switch (already_owned)
				{
				case true: cout << (*current_selection).second->getData()->getTitle() << " is already in your inventory" << endl;
					break;
				case false: current_player->addWorkToInventory((*current_selection).second);
					cout << (*current_selection).second->getData()->getTitle() << " has been added to your inventory" << endl;
					cout << "Collection value: $" << current_player->getCollectionValue().getNumberString(true, false, 2) << endl;
					break;
				}
			}

			if (keys->checkPress(GLFW_KEY_A, false))
			{
				for (auto i : paintings_to_display)
				{
					int selected_index = i.second->getData()->getID();
					bool already_owned = current_player->alreadyOwned(selected_index);

					switch (already_owned)
					{
					case true: cout << i.second->getData()->getTitle() << " is already in your inventory" << endl;
						break;
					case false: current_player->addWorkToInventory(i.second);
						cout << i.second->getData()->getTitle() << " has been added to your inventory" << endl;
						break;
					}
				}
				cout << "Collection value: $" << current_player->getCollectionValue().getNumberString(true, false, 2) << endl;
			}

			if (keys->checkPress(GLFW_KEY_1, false))
			{
				current_selection = sortArtVec(paintings_to_display, ARTIST_NAME);
				makeThumbnails(paintings_to_display, 0.1f, 0.3f);

				cout << endl << "-----Sorted by Artist-----" << endl;

				for (auto i : paintings_to_display)
					printArtwork(i.second);
			}

			if (keys->checkPress(GLFW_KEY_2, false))
			{
				current_selection = sortArtVec(paintings_to_display, VALUE);
				makeThumbnails(paintings_to_display, 0.1f, 0.3f);

				cout << endl << "-----Sorted by Value-----" << endl;

				for (auto i : paintings_to_display)
					printArtwork(i.second);
			}

			if (keys->checkPress(GLFW_KEY_3, false))
			{
				current_selection = sortArtVec(paintings_to_display, DATE);
				makeThumbnails(paintings_to_display, 0.1f, 0.3f);

				cout << endl << "-----Sorted by Date-----" << endl;

				for (auto i : paintings_to_display)
					printArtwork(i.second);
			}

			if (keys->checkPress(GLFW_KEY_4, false))
			{
				current_selection = sortArtVec(paintings_to_display, RARITY);
				makeThumbnails(paintings_to_display, 0.1f, 0.3f);

				cout << endl << "-----Sorted by Rarity-----" << endl;

				for (auto i : paintings_to_display)
					printArtwork(i.second);
			}

			if (keys->checkPress(GLFW_KEY_5, false))
			{
				current_selection = sortArtVec(paintings_to_display, TITLE);
				makeThumbnails(paintings_to_display, 0.1f, 0.3f);

				cout << endl << "-----Sorted by Title-----" << endl;

				for (auto i : paintings_to_display)
					printArtwork(i.second);
			}

			//TODO fix so crate doesn't disappear when going to the main menu
			if (keys->checkPress(GLFW_KEY_ESCAPE, false))
			{
				menu_return = mainMenu(data_path, context, keys);
				finished = true;
			}

			context->swapBuffers();
			glfwSetTime(0.0f);
		}
	}

	context->setBackgroundColor(original_background);
	return menu_return;
}

int viewGallery(string data_path, const shared_ptr<ogl_context> &context, shared_ptr<key_handler> keys, const shared_ptr<player> &current_player)
{
	float eye_level = 1.65f;
	shared_ptr<ogl_camera> camera(new ogl_camera_free(keys, context, vec3(0.0f, eye_level, 5.0f), 45.0f));

	vec4 original_background = context->getBackgroundColor();
	context->setBackgroundColor(vec4(0.5f, 0.5f, 0.5f, 1.0f));

	//TODO why does this require a pair with int first?
	vector<pair<int, shared_ptr<artwork> > > paintings_to_display = current_player->getDisplayedCopy();
	offsetArtworks(paintings_to_display, eye_level);
	
	for (auto i : paintings_to_display)
		i.second->applyFrameTemplate(*(current_player->getDefaultFrame()));

	glfwSetTime(0);
	float render_fps = 60.0f;

	bool finished = false;
	int menu_return = 0;

	vector<shared_ptr<line> > lines;

	lines.push_back(shared_ptr<line>(new line(
		vec4(0.25f, 0.0f, 0.0f, 1.0f),
		vec4(-0.25f, 0.0f, 0.0f, 1.0f), 
		vec4(0.0f, 0.0f, 0.0f, 1.0f)
		)));

	lines.push_back(shared_ptr<line>(new line(
		vec4(0.0f, 0.25f, 0.0f, 1.0f),
		vec4(0.0f, -0.25, 0.0f, 1.0f),
		vec4(0.0f, 0.0f, 0.0f, 1.0f)
		)));

	lines.push_back(shared_ptr<line>(new line(
		vec4(0.0f, 0.0f, 0.25f, 1.0f),
		vec4(0.0f, 0.0f, -0.25, 1.0f),
		vec4(0.0f, 0.0f, 0.0f, 1.0f)
		)));

	while (!finished)
	{
		if (glfwGetTime() > 1.0f / render_fps)
		{
			glfwPollEvents();
			context->clearBuffers();

			camera->updateCamera();
			for (auto i : paintings_to_display)
				i.second->draw(context, camera);

			for (auto i : lines)
				i->draw(context, camera);

			if (keys->checkPress(GLFW_KEY_ESCAPE))
			{
				menu_return = mainMenu(data_path, context, keys);
				if (menu_return != 0)
					finished = true;
			}

			if (keys->checkMouse(GLFW_MOUSE_BUTTON_LEFT, false))
			{
				for (auto i : paintings_to_display)
				{
					if (paintingSelected(keys, camera, i.second))
						printArtwork(i.second);
				}
			}

			context->swapBuffers();
			glfwSetTime(0.0f);
		}
	}

	for (auto i : paintings_to_display)
		i.second->getData()->unloadData();

	context->setBackgroundColor(original_background);
	return menu_return;
}