#include "header.h"
#include "geometry.h"
#include "loot.h"
#include "player.h"
#include "artwork.h"
#include "utility_funcs.h"
#include "menus.h"
#include "gallery.h"
#include "hud.h"

int mainMenu(string data_path, const shared_ptr<ogl_context> &context, const shared_ptr<key_handler> &keys, const shared_ptr<text_handler> &text)
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
					i.second.first.draw(context, model_matrix, camera);
					i.second.second.draw(context, model_matrix, camera);			
				}

				else
				{
					glUniform1f(context->getShaderGLint("dim_factor"), 0.5f);
					i.second.first.draw(context, model_matrix, camera);
					i.second.second.draw(context, model_matrix, camera);
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
	const shared_ptr<key_handler> &keys, const shared_ptr<player> &current_player, const shared_ptr<text_handler> &text)
{
	vec4 original_background = context->getBackgroundColor();
	context->setBackgroundColor(vec4(0.5f, 0.0f, 0.0f, 1.0f));

	int display_count = 10;

	//remove inventory copy mechanic. use actual inventory container with active iterators
	//add copies of the artwork instances to the local vector, so position can be manipulated
	vector<pair<int, shared_ptr<artwork> > > inventory_copy = current_player->getInventoryCopy();

	//add player's default frames to each
	for (auto i : inventory_copy)
		i.second->applyFrameTemplate(context, *(current_player->getDefaultFrame()));


	//take a chunk of the inventory paintings, using first, last, end iterators. update this vectore to go through pages of inventory
	vector<pair<int, shared_ptr<artwork> > >::const_iterator chunk_first = inventory_copy.begin();
	vector<pair<int, shared_ptr<artwork> > >::const_iterator chunk_last =
		findChunkLast(chunk_first, inventory_copy, display_count);
	vector<pair<int, shared_ptr<artwork> > >::const_iterator chunk_end =
		findChunkEnd(chunk_first, inventory_copy, display_count);

	vector<pair<int, shared_ptr<artwork> > > paintings_to_display;

	paintings_to_display.reserve(display_count);
	paintings_to_display.insert(paintings_to_display.begin(), chunk_first, chunk_end);

	//generate a maps of matrices
	float thumbnail_cell_size(0.3f);
	float thumbnail_margin(0.1f);
	float thumbnail_cell_padding_factor(0.05f);
	map<int, mat4> thumbnail_matrix_index = getThumbnailMatrixMap(
		context, paintings_to_display, thumbnail_margin, thumbnail_cell_size, thumbnail_cell_padding_factor);

	float top_margin(0.1f);
	float thumbnail_buffer(0.05f);
	float highlight_image_size(2.0f - thumbnail_cell_size - thumbnail_margin - thumbnail_buffer - top_margin);
	float left_margin(0.1f);
	//TODO text is placed properly in window, find out how aspect ratio is adjusted and make matrix maps work similarly
	//x_offset based on left margin/image size
	float x_offset(-0.4f);
	vec2 highlight_position(x_offset, 0.25f);
	map<int, mat4> highlight_matrix_index = getHighlightMatrixMap(context, paintings_to_display, highlight_position, highlight_image_size);

	//identify positions for text
	vec4 title_color(1.0f, 1.0f, 1.0f, 1.0f);
	vec4 rarity_color;
	vec4 info_color(0.6f, 0.6f, 0.6f, 1.0f);
	vec4 transparent_color(0.0f, 1.0f, 0.0f, 1.0f);
	float title_scale(0.06f);
	float info_scale(0.04f);
	float highlight_buffer(0.1f);
	vec2 title_screen_position(0.2f, 0.75f);
	vec2 rarity_screen_position(title_screen_position.x, title_screen_position.y - title_scale);
	vec2 info_screen_position(title_screen_position.x, rarity_screen_position.y - info_scale);
	float text_box_width(0.7f);

	vector<pair<int, shared_ptr<artwork> > >::iterator current_selection = paintings_to_display.begin();
	shared_ptr<static_text> title_text(nullptr);
	shared_ptr<static_text> rarity_text(nullptr);
	shared_ptr<static_text> info_text(nullptr);

	vec4 alert_color(0.8f, 0.5f, 0.5f, 1.0f);
	float alert_scale(0.04f);
	float alert_buffer(0.1f);
	vec2 alert_screen_position(info_screen_position.x, info_screen_position.y - alert_buffer);
	shared_ptr<static_text> alert_text(nullptr);

	shared_ptr<ogl_camera> camera(new ogl_camera(keys, context, vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 0.0f, 0.0f), 45.0f));

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

						//generate new matrix maps
						thumbnail_matrix_index = getThumbnailMatrixMap(
							context, paintings_to_display, thumbnail_margin, thumbnail_cell_size, thumbnail_cell_padding_factor);
						highlight_matrix_index = getHighlightMatrixMap(context, paintings_to_display, highlight_position, highlight_image_size);

						current_selection = paintings_to_display.end() - 1;
					}
				}

				else
				{
					alert_text = nullptr;
					current_selection--;
				}
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

						//generate new matrix maps
						thumbnail_matrix_index = getThumbnailMatrixMap(
							context, paintings_to_display, thumbnail_margin, thumbnail_cell_size, thumbnail_cell_padding_factor);
						highlight_matrix_index = getHighlightMatrixMap(context, paintings_to_display, highlight_position, highlight_image_size);

						current_selection = paintings_to_display.begin();
					}
				}

				else
				{
					alert_text = nullptr;
					current_selection++;
				}
			}

			glUniform1f(context->getShaderGLint("dim_factor"), 0.5f);
			for (vector<pair<int, shared_ptr<artwork> > >::const_iterator it = paintings_to_display.cbegin(); it != paintings_to_display.cend(); it++)
			{
				int index = std::distance(paintings_to_display.cbegin(), it);
				
				if (current_selection != it)
					(*it).second->draw2D(context, camera, thumbnail_matrix_index.at(index));

				else
				{
					glUniform1f(context->getShaderGLint("dim_factor"), 1.0f);
					(*it).second->draw2D(context, camera, thumbnail_matrix_index.at(index));
					(*it).second->draw2D(context, camera, highlight_matrix_index.at(index));

					title_text = text->getTextArray((*it).second->getData()->getTitle(), context,
						true, title_color, transparent_color, true, title_screen_position, title_scale, text_box_width);

					switch ((*it).second->getData()->getRarity())
					{
					case COMMON: rarity_color = vec4(0.6f, 0.9f, 0.6f, 1.0f); break;
					case UNCOMMON: rarity_color = vec4(0.6f, 0.6f, 0.9f, 1.0f); break;
					case RARE: rarity_color = vec4(0.9f, 0.9f, 0.6f, 1.0f); break;
					case LEGENDARY: rarity_color = vec4(1.0f, 0.75f, 0.6f, 1.0f); break;
					case MASTERPIECE: rarity_color = vec4(0.6f, 0.9f, 0.9f, 1.0f); break;
					}

					rarity_text = text->getTextArray(stringFromRarity((*it).second->getData()->getRarity()), context,
						false, rarity_color, transparent_color, true, title_text->getLowerLeft(), info_scale, text_box_width);

					string to_print = std::to_string((*it).second->getData()->getDate().getYear()) + "\n" + (*it).second->getData()->getArtistName();
					to_print += "\n$" + (*it).second->getValue().getNumberString(true, false, 2);

					info_text = text->getTextArray(to_print, context, false, info_color, transparent_color, 
						true, rarity_text->getLowerLeft(), info_scale, text_box_width);

					glUniform1f(context->getShaderGLint("dim_factor"), 0.5f);
				}
			}
			glUniform1f(context->getShaderGLint("dim_factor"), 1.0f);

			if (keys->checkPress(GLFW_KEY_ENTER, false))
			{
				alert_text = nullptr;
				string alert_string;

				//if painting is already on display, take it off. if it's not, add it to display
				switch (current_player->isOnDisplay((*current_selection).first))
				{
				case true: current_player->removePaintingFromDisplay(*current_selection); 
					alert_string = (*current_selection).second->getData()->getTitle() + " has been removed from the gallery"; 
					break;
				case false: current_player->addPaintingToDisplay(*current_selection);
					alert_string = (*current_selection).second->getData()->getTitle() + " has been added to the gallery";
					break;
				}

				alert_text = text->getTextArray(alert_string, context, false, alert_color, transparent_color,
					true, info_text->getLowerLeft() - vec2(0.0f, alert_buffer), alert_scale, text_box_width);

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

				//generate new matrix maps
				thumbnail_matrix_index = getThumbnailMatrixMap(
					context, paintings_to_display, thumbnail_margin, thumbnail_cell_size, thumbnail_cell_padding_factor);
				highlight_matrix_index = getHighlightMatrixMap(context, paintings_to_display, highlight_position, highlight_image_size);

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

				//generate new matrix maps
				thumbnail_matrix_index = getThumbnailMatrixMap(
					context, paintings_to_display, thumbnail_margin, thumbnail_cell_size, thumbnail_cell_padding_factor);
				highlight_matrix_index = getHighlightMatrixMap(context, paintings_to_display, highlight_position, highlight_image_size);

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

				//generate new matrix maps
				thumbnail_matrix_index = getThumbnailMatrixMap(
					context, paintings_to_display, thumbnail_margin, thumbnail_cell_size, thumbnail_cell_padding_factor);
				highlight_matrix_index = getHighlightMatrixMap(context, paintings_to_display, highlight_position, highlight_image_size);

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

				//generate new matrix maps
				thumbnail_matrix_index = getThumbnailMatrixMap(
					context, paintings_to_display, thumbnail_margin, thumbnail_cell_size, thumbnail_cell_padding_factor);
				highlight_matrix_index = getHighlightMatrixMap(context, paintings_to_display, highlight_position, highlight_image_size);

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

				//generate new matrix maps
				thumbnail_matrix_index = getThumbnailMatrixMap(
					context, paintings_to_display, thumbnail_margin, thumbnail_cell_size, thumbnail_cell_padding_factor);
				highlight_matrix_index = getHighlightMatrixMap(context, paintings_to_display, highlight_position, highlight_image_size);

				current_selection = paintings_to_display.begin();

				cout << endl << "-----Sorted by Title-----" << endl;

				for (auto i : paintings_to_display)
					printArtwork(i.second);
			}

			if (title_text != nullptr)
				title_text->draw(camera, context, "text", "text_color", "transparency_color");
			if (info_text != nullptr)
				info_text->draw(camera, context, "text", "text_color", "transparency_color");
			if (rarity_text != nullptr)
				rarity_text->draw(camera, context, "text", "text_color", "transparency_color");
			if (alert_text != nullptr)
				alert_text->draw(camera, context, "text", "text_color", "transparency_color");

			context->swapBuffers();

			if (keys->checkPress(GLFW_KEY_ESCAPE, false))
			{
				menu_return = mainMenu(data_path, context, keys, text);
				finished = (menu_return != 1);
			}

			glfwSetTime(0.0f);			
		}
	}

	context->setBackgroundColor(original_background);
	return menu_return;
}

int openCrate(string data_path, const shared_ptr<ogl_context> &context, const shared_ptr<key_handler> &keys, 
	const shared_ptr<player> &current_player, const shared_ptr<loot_generator> &lg, const shared_ptr<text_handler> &text)
{
	vec4 original_background = context->getBackgroundColor();
	context->setBackgroundColor(vec4(0.0f, 0.0f, 0.5f, 1.0f));
	
	int drop_count = 6;

	//TODO revise so function doesn't rely on so many containers created/copied per run
	//add copies of the artwork instances to the local vector, so position can be manipulated
	vector<pair<int, shared_ptr<artwork> > > paintings_to_display = lg->generateArtworks(drop_count, 1.0f);

	//add player's default frames to each
	for (auto i : paintings_to_display)
		i.second->applyFrameTemplate(context, *(current_player->getDefaultFrame()));

	//generate a maps of matrices
	float thumbnail_cell_size(0.3f);
	float thumbnail_margin(0.1f);
	float thumbnail_cell_padding_factor(0.05f);
	map<int, mat4> thumbnail_matrix_index = getThumbnailMatrixMap(
		context, paintings_to_display, thumbnail_margin, thumbnail_cell_size, thumbnail_cell_padding_factor);

	float top_margin(0.1f);
	float thumbnail_buffer(0.05f);
	float highlight_image_size(2.0f - thumbnail_cell_size - thumbnail_margin - thumbnail_buffer - top_margin);
	float left_margin(0.1f);
	//TODO text is placed properly in window, find out how aspect ratio is adjusted and make matrix maps work similarly
	//x_offset based on left margin/image size
	float x_offset(-0.4f);
	vec2 highlight_position(x_offset, 0.25f);
	map<int, mat4> highlight_matrix_index = getHighlightMatrixMap(context, paintings_to_display, highlight_position, highlight_image_size);

	//identify positions for text
	vec4 title_color(1.0f, 1.0f, 1.0f, 1.0f);
	vec4 rarity_color;
	vec4 info_color(0.6f, 0.6f, 0.6f, 1.0f);
	vec4 transparent_color(0.0f, 1.0f, 0.0f, 1.0f);
	float title_scale(0.06f);
	float info_scale(0.04f);
	float highlight_buffer(0.1f);
	vec2 title_screen_position(0.2f, 0.75f);
	vec2 rarity_screen_position(title_screen_position.x, title_screen_position.y - title_scale);
	vec2 info_screen_position(title_screen_position.x, rarity_screen_position.y - info_scale);
	float text_box_width(0.7f);

	shared_ptr<static_text> title_text(nullptr);
	shared_ptr<static_text> rarity_text(nullptr);
	shared_ptr<static_text> info_text(nullptr);

	vec4 alert_color(0.8f, 0.5f, 0.5f, 1.0f);
	float alert_scale(0.04f);
	float alert_buffer(0.1f);
	vec2 alert_screen_position(info_screen_position.x, info_screen_position.y - alert_buffer);
	shared_ptr<static_text> alert_text(nullptr);

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
			{
				alert_text = nullptr;
				current_selection--;
			}

			else if (keys->checkPress(GLFW_KEY_RIGHT, false) && current_selection != paintings_to_display.end() - 1)
			{
				alert_text = nullptr;
				current_selection++;
			}

			glUniform1f(context->getShaderGLint("dim_factor"), 0.5f);
			for (vector<pair<int, shared_ptr<artwork> > >::const_iterator it = paintings_to_display.cbegin(); it != paintings_to_display.cend(); it++)
			{
				int index = std::distance(paintings_to_display.cbegin(), it);

				if (current_selection != it)
					(*it).second->draw2D(context, camera, thumbnail_matrix_index.at(index));

				else
				{
					glUniform1f(context->getShaderGLint("dim_factor"), 1.0f);
					(*it).second->draw2D(context, camera, thumbnail_matrix_index.at(index));
					(*it).second->draw2D(context, camera, highlight_matrix_index.at(index));

					title_text = text->getTextArray((*it).second->getData()->getTitle(), context,
						true, title_color, transparent_color, true, title_screen_position, title_scale, text_box_width);

					switch ((*it).second->getData()->getRarity())
					{
					case COMMON: rarity_color = vec4(0.6f, 0.9f, 0.6f, 1.0f); break;
					case UNCOMMON: rarity_color = vec4(0.6f, 0.6f, 0.9f, 1.0f); break;
					case RARE: rarity_color = vec4(0.9f, 0.9f, 0.6f, 1.0f); break;
					case LEGENDARY: rarity_color = vec4(1.0f, 0.75f, 0.6f, 1.0f); break;
					case MASTERPIECE: rarity_color = vec4(0.6f, 0.9f, 0.9f, 1.0f); break;
					}

					rarity_text = text->getTextArray(stringFromRarity((*it).second->getData()->getRarity()), context,
						false, rarity_color, transparent_color, true, title_text->getLowerLeft(), info_scale, text_box_width);

					string to_print = std::to_string((*it).second->getData()->getDate().getYear()) + "\n" + (*it).second->getData()->getArtistName();
					to_print += "\n$" + (*it).second->getValue().getNumberString(true, false, 2);

					info_text = text->getTextArray(to_print, context, false, info_color, transparent_color,
						true, rarity_text->getLowerLeft(), info_scale, text_box_width);

					glUniform1f(context->getShaderGLint("dim_factor"), 0.5f);
				}
			}
			glUniform1f(context->getShaderGLint("dim_factor"), 1.0f);

			if (keys->checkPress(GLFW_KEY_ENTER, false))
			{
				int selected_index = (*current_selection).second->getData()->getID();
				bool already_owned = current_player->alreadyOwned(selected_index);

				string alert_string;

				switch (already_owned)
				{
				case true: alert_string = (*current_selection).second->getData()->getTitle() + " is already in your inventory"; break;
				case false: current_player->addWorkToInventory((*current_selection).second);
					alert_string = (*current_selection).second->getData()->getTitle() + " has been added to your inventory";
					alert_string += "\n\nCollection value: $" + current_player->getCollectionValue().getNumberString(true, false, 2);
					break;
				}

				alert_text = text->getTextArray(alert_string, context, false, alert_color, transparent_color,
					true, info_text->getLowerLeft() - vec2(0.0f, alert_buffer), alert_scale, text_box_width);
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

				alert_text = text->getTextArray("All paintings have been added to your inventory", context, false, alert_color, transparent_color,
					true, info_text->getLowerLeft() - vec2(0.0f, alert_buffer), alert_scale, text_box_width);

				cout << "Collection value: $" << current_player->getCollectionValue().getNumberString(true, false, 2) << endl;
			}

			if (keys->checkPress(GLFW_KEY_1, false))
			{
				current_selection = sortArtVec(paintings_to_display, ARTIST_NAME);
				
				//generate new matrix maps
				thumbnail_matrix_index = getThumbnailMatrixMap(
					context, paintings_to_display, thumbnail_margin, thumbnail_cell_size, thumbnail_cell_padding_factor);
				highlight_matrix_index = getHighlightMatrixMap(context, paintings_to_display, highlight_position, highlight_image_size);

				cout << endl << "-----Sorted by Artist-----" << endl;

				for (auto i : paintings_to_display)
					printArtwork(i.second);
			}

			if (keys->checkPress(GLFW_KEY_2, false))
			{
				current_selection = sortArtVec(paintings_to_display, VALUE);
				
				//generate new matrix maps
				thumbnail_matrix_index = getThumbnailMatrixMap(
					context, paintings_to_display, thumbnail_margin, thumbnail_cell_size, thumbnail_cell_padding_factor);
				highlight_matrix_index = getHighlightMatrixMap(context, paintings_to_display, highlight_position, highlight_image_size);

				cout << endl << "-----Sorted by Value-----" << endl;

				for (auto i : paintings_to_display)
					printArtwork(i.second);
			}

			if (keys->checkPress(GLFW_KEY_3, false))
			{
				current_selection = sortArtVec(paintings_to_display, DATE);
				
				//generate new matrix maps
				thumbnail_matrix_index = getThumbnailMatrixMap(
					context, paintings_to_display, thumbnail_margin, thumbnail_cell_size, thumbnail_cell_padding_factor);
				highlight_matrix_index = getHighlightMatrixMap(context, paintings_to_display, highlight_position, highlight_image_size);

				cout << endl << "-----Sorted by Date-----" << endl;

				for (auto i : paintings_to_display)
					printArtwork(i.second);
			}

			if (keys->checkPress(GLFW_KEY_4, false))
			{
				current_selection = sortArtVec(paintings_to_display, RARITY);
				
				//generate new matrix maps
				thumbnail_matrix_index = getThumbnailMatrixMap(
					context, paintings_to_display, thumbnail_margin, thumbnail_cell_size, thumbnail_cell_padding_factor);
				highlight_matrix_index = getHighlightMatrixMap(context, paintings_to_display, highlight_position, highlight_image_size);

				cout << endl << "-----Sorted by Rarity-----" << endl;

				for (auto i : paintings_to_display)
					printArtwork(i.second);
			}

			if (keys->checkPress(GLFW_KEY_5, false))
			{
				current_selection = sortArtVec(paintings_to_display, TITLE);
				
				//generate new matrix maps
				thumbnail_matrix_index = getThumbnailMatrixMap(
					context, paintings_to_display, thumbnail_margin, thumbnail_cell_size, thumbnail_cell_padding_factor);
				highlight_matrix_index = getHighlightMatrixMap(context, paintings_to_display, highlight_position, highlight_image_size);

				cout << endl << "-----Sorted by Title-----" << endl;

				for (auto i : paintings_to_display)
					printArtwork(i.second);
			}

			if (title_text != nullptr)
				title_text->draw(camera, context, "text", "text_color", "transparency_color");
			if (rarity_text != nullptr)
				rarity_text->draw(camera, context, "text", "text_color", "transparency_color");
			if (info_text != nullptr)
				info_text->draw(camera, context, "text", "text_color", "transparency_color");
			if (alert_text != nullptr)
				alert_text->draw(camera, context, "text", "text_color", "transparency_color");

			if (keys->checkPress(GLFW_KEY_ESCAPE, false))
			{
				menu_return = mainMenu(data_path, context, keys, text);
				finished = true;
			}

			context->swapBuffers();
			glfwSetTime(0.0f);
		}
	}

	context->setBackgroundColor(original_background);
	return menu_return;
}

int viewGallery(string data_path, const shared_ptr<ogl_context> &context, shared_ptr<key_handler> keys, 
	const shared_ptr<player> &current_player, const shared_ptr<text_handler> &text)
{
	float eye_level = 1.65f;
	shared_ptr<ogl_camera> camera(new ogl_camera_free(keys, context, vec3(0.0f, eye_level, 5.0f), 45.0f));

	vec4 original_background = context->getBackgroundColor();
	context->setBackgroundColor(vec4(0.5f, 0.5f, 0.5f, 1.0f));

	//GALLERY TEST CODE
	shared_ptr<gallery> test_gallery(new gallery(
		context, data_path + "model_data\\",
		data_path + "model_data\\",
		"test_gallery_02.obj",
		"test_filler.obj",
		"test_gallery_02.mtl",
		"test_filler.mtl"));
	//GALLERY TEST CODE

	//TODO why does this require a pair with int first?
	vector<pair<int, shared_ptr<artwork> > > paintings_to_display = current_player->getDisplayedCopy();
	offsetArtworks(paintings_to_display, eye_level);
	
	for (auto i : paintings_to_display)
		i.second->applyFrameTemplate(context, *(current_player->getDefaultFrame()));

	artwork_thumbnail thumbnail_test(context, camera);

	glfwSetTime(0);
	float render_fps = 60.0f;

	bool finished = false;
	int menu_return = 0;

	vector<shared_ptr<line> > lines;

	lines.push_back(shared_ptr<line>(new line(
		vec4(1.0f, 0.0f, 0.0f, 1.0f),
		vec4(-1.0f, 0.0f, 0.0f, 1.0f),
		vec4(0.0f, 0.0f, 0.0f, 1.0f)
		)));

	lines.push_back(shared_ptr<line>(new line(
		vec4(0.0f, 1.0f, 0.0f, 1.0f),
		vec4(0.0f, -1.0f, 0.0f, 1.0f),
		vec4(0.0f, 0.0f, 0.0f, 1.0f)
		)));

	lines.push_back(shared_ptr<line>(new line(
		vec4(0.0f, 0.0f, 1.0f, 1.0f),
		vec4(0.0f, 0.0f, -1.0f, 1.0f),
		vec4(0.0f, 0.0f, 0.0f, 1.0f)
		)));

	//identify positions for text
	vec4 title_color(1.0f, 1.0f, 1.0f, 1.0f);
	vec4 rarity_color;
	vec4 info_color(0.3f, 0.3f, 0.3f, 1.0f);
	vec4 transparent_color(0.0f, 1.0f, 0.0f, 1.0f);
	float title_scale(0.045f);
	float info_scale(0.035f);
	float highlight_buffer(0.1f);
	vec2 title_screen_position(0.5f, 0.75f);
	vec2 rarity_screen_position(title_screen_position.x, title_screen_position.y - title_scale);
	vec2 info_screen_position(title_screen_position.x, rarity_screen_position.y - info_scale);
	float text_box_width(0.45f);

	shared_ptr<static_text> title_text(nullptr);
	shared_ptr<static_text> rarity_text(nullptr);
	shared_ptr<static_text> info_text(nullptr);

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

			test_gallery->renderGallery(context, camera);

			if (title_text != nullptr)
				title_text->draw(camera, context, "text", "text_color", "transparency_color");
			if (info_text != nullptr)
				info_text->draw(camera, context, "text", "text_color", "transparency_color");
			if (rarity_text != nullptr)
				rarity_text->draw(camera, context, "text", "text_color", "transparency_color");

			if (keys->checkPress(GLFW_KEY_ESCAPE))
			{
				menu_return = mainMenu(data_path, context, keys, text);
				if (menu_return != 0)
					finished = true;
			}

			if (keys->checkMouse(GLFW_MOUSE_BUTTON_LEFT, false))
			{
				//first float indicates scale of the ray from bary coordinates (result.z)
				//the smaller the scale, the closer the object is. closest object indicates intended target
				pair<float, shared_ptr<artwork> > artwork_selected;
				pair<float, shared_ptr<display_wall> > wall_selected;

				wall_selected.second = test_gallery->checkWallClicks(keys, camera, wall_selected.first);
				artwork_selected.second = test_gallery->checkArtworkClicks(keys, camera, artwork_selected.first);

				cout << "artwork_selected distance: " << artwork_selected.first << endl;
				cout << "wall_selected distance: " << wall_selected.first << endl;

				//wall returned wasn't nullptr, and either no artwork was selected or the wall was closer
				bool wall_was_selected = (wall_selected.second != nullptr && 
					(artwork_selected.second == nullptr || artwork_selected.first > wall_selected.first));
				bool artwork_was_selected = artwork_selected.second != nullptr && !wall_was_selected;

				if (wall_selected.second == nullptr)
					cout << "wall_selected.second is nullptr" << endl;

				if (wall_was_selected)
				{
					cout << "wall was selected" << endl;
					vec2 point_clicked = wall_selected.second->getClickPositionWallspace();
					
					if (paintings_to_display.size() > 0)
					{
						shared_ptr<artwork> copy = paintings_to_display.at(0).second;

						//TODO validPlacement not working
						if (wall_selected.second->validPlacement(copy, point_clicked) || true)
							wall_selected.second->addPainting(point_clicked, *copy);

						else cout << "position not valid" << endl;
					}				
				}

				if (artwork_was_selected)
				{
					shared_ptr<artwork> selected = artwork_selected.second;
					title_text = text->getTextArray(selected->getData()->getTitle(), context,
						true, title_color, transparent_color, true, title_screen_position, title_scale, text_box_width);

					switch (selected->getData()->getRarity())
					{
					case COMMON: rarity_color = vec4(0.6f, 0.9f, 0.6f, 1.0f); break;
					case UNCOMMON: rarity_color = vec4(0.6f, 0.6f, 0.9f, 1.0f); break;
					case RARE: rarity_color = vec4(0.9f, 0.9f, 0.6f, 1.0f); break;
					case LEGENDARY: rarity_color = vec4(1.0f, 0.75f, 0.6f, 1.0f); break;
					case MASTERPIECE: rarity_color = vec4(0.6f, 0.9f, 0.9f, 1.0f); break;
					}

					rarity_text = text->getTextArray(stringFromRarity(selected->getData()->getRarity()), context,
						false, rarity_color, transparent_color, true, title_text->getLowerLeft(), info_scale, text_box_width);

					string to_print = std::to_string(selected->getData()->getDate().getYear()) + "\n" + selected->getData()->getArtistName();
					to_print += "\n$" + selected->getValue().getNumberString(true, false, 2);

					info_text = text->getTextArray(to_print, context, false, info_color, transparent_color,
						true, rarity_text->getLowerLeft(), info_scale, text_box_width);
				}

				else
				{
					title_text = nullptr;
					info_text = nullptr;
					rarity_text = nullptr;
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