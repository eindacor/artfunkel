#include "header.h"
#include "player.h"
#include "artwork.h"
#include "utility_funcs.h"
#include "menus.h"
#include "gallery.h"
#include "hud.h"

int viewInventory(string data_path, const shared_ptr<ogl_context> &context,
	const shared_ptr<key_handler> &keys, const shared_ptr<player> &current_player, const shared_ptr<text_handler> &text)
{
	vec4 original_background = context->getBackgroundColor();
	context->setBackgroundColor(vec4(0.5f, 0.0f, 0.0f, 1.0f));

	int display_count = 10;

	//remove inventory copy mechanic. use actual inventory container with active iterators
	//add copies of the artwork instances to the local vector, so position can be manipulated
	vector<shared_ptr<artwork> >inventory_copy = current_player->getInventoryCopy();

	//add player's default frames to each
	for (auto i : inventory_copy)
		i->applyFrameTemplate(context, *(current_player->getDefaultFrame()));

	//take a chunk of the inventory paintings, using first, last, end iterators. update this vectore to go through pages of inventory
	vector<shared_ptr<artwork> >::const_iterator chunk_first = inventory_copy.begin();
	vector<shared_ptr<artwork> >::const_iterator chunk_last =
		findChunkLast(chunk_first, inventory_copy, display_count);
	vector<shared_ptr<artwork> >::const_iterator chunk_end =
		findChunkEnd(chunk_first, inventory_copy, display_count);

	vector<shared_ptr<artwork> > paintings_to_display;

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

	vector<shared_ptr<artwork> >::iterator current_selection = paintings_to_display.begin();
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
			for (vector<shared_ptr<artwork> >::const_iterator it = paintings_to_display.cbegin(); it != paintings_to_display.cend(); it++)
			{
				int index = std::distance(paintings_to_display.cbegin(), it);

				/*
				if (current_selection != it)
					(*it)->draw2D(context, camera, thumbnail_matrix_index.at(index));

				else
				{
					glUniform1f(context->getShaderGLint("dim_factor"), 1.0f);
					(*it)->draw2D(context, camera, thumbnail_matrix_index.at(index));
					(*it)->draw2D(context, camera, highlight_matrix_index.at(index));

					title_text = text->getTextArray((*it)->getData()->getTitle(), context,
						true, title_color, transparent_color, true, title_screen_position, title_scale, text_box_width);

					switch ((*it)->getData()->getRarity())
					{
					case COMMON: rarity_color = vec4(0.6f, 0.9f, 0.6f, 1.0f); break;
					case UNCOMMON: rarity_color = vec4(0.6f, 0.6f, 0.9f, 1.0f); break;
					case RARE: rarity_color = vec4(0.9f, 0.9f, 0.6f, 1.0f); break;
					case LEGENDARY: rarity_color = vec4(1.0f, 0.75f, 0.6f, 1.0f); break;
					case MASTERPIECE: rarity_color = vec4(0.6f, 0.9f, 0.9f, 1.0f); break;
					}

					rarity_text = text->getTextArray(stringFromRarity((*it)->getData()->getRarity()), context,
						false, rarity_color, transparent_color, true, title_text->getLowerLeft(), info_scale, text_box_width);

					string to_print = std::to_string((*it)->getData()->getDate().getYear()) + "\n" + (*it)->getData()->getArtistName();
					to_print += "\n$" + (*it)->getValue().getNumberString(true, false, 2);

					info_text = text->getTextArray(to_print, context, false, info_color, transparent_color,
						true, rarity_text->getLowerLeft(), info_scale, text_box_width);

					glUniform1f(context->getShaderGLint("dim_factor"), 0.5f);
				}
				*/
			}
			glUniform1f(context->getShaderGLint("dim_factor"), 1.0f);

			if (keys->checkPress(GLFW_KEY_ENTER, false))
			{
				alert_text = nullptr;
				string alert_string;

				//if painting is already on display, take it off. if it's not, add it to display
				switch (current_player->isOnDisplay(*current_selection))
				{
				case true: current_player->removePaintingFromDisplay(*current_selection);
					alert_string = (*current_selection)->getData()->getTitle() + " has been removed from the gallery";
					break;
				case false: current_player->addPaintingToDisplay(*current_selection);
					alert_string = (*current_selection)->getData()->getTitle() + " has been added to the gallery";
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
					printArtwork(i);
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
					printArtwork(i);
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
					printArtwork(i);
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
					printArtwork(i);
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
					printArtwork(i);
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

int viewInventory_HUD(string data_path, const shared_ptr<ogl_context> &context,
	shared_ptr<key_handler> &keys, const shared_ptr<player> &current_player, const shared_ptr<text_handler> &text)
{
	vec4 original_background = context->getBackgroundColor();
	context->setBackgroundColor(vec4(0.5f, 0.3f, 0.0f, 1.0f));

	int display_count = 10;

	//TODO remove inventory copy mechanic. use actual inventory container with active iterators
	//add copies of the artwork instances to the local vector, so position can be manipulated
	vector<shared_ptr<artwork> >inventory_copy = current_player->getInventoryCopy();
	shared_ptr<dynamic_hud_array> artwork_thumbnails(new dynamic_hud_array(context, vec2(0.85f, -0.0f), 0.3f, 2.0f ,
		pair<horizontal_justification, vertical_justification>(H_CENTER, V_MIDDLE)));

	//TODO equip dynamic array with function that generates thumbnails from artwork
	//add player's default frames to each
	for (auto i : inventory_copy)
	{
		i->applyFrameTemplate(context, *(current_player->getDefaultFrame()));
		shared_ptr<artwork_thumbnail> thumbnail(new artwork_thumbnail(i, context, vec2(0.3f, 0.3f), 0.01f));
		thumbnail->setDrawSelected(highlight, fullBrightness);
		artwork_thumbnails->addElement(thumbnail);
	}

	shared_ptr<artwork_thumbnail> highlight = nullptr;

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
	
			artwork_thumbnails->draw(context, camera);

			if (highlight != nullptr)
				highlight->draw(context, camera);

			context->swapBuffers();

			if (keys->checkPress(GLFW_KEY_ESCAPE, false))
			{
				menu_return = mainMenu(data_path, context, keys, text);
				finished = (menu_return != 1);
			}

			if (keys->checkMouse(GLFW_MOUSE_BUTTON_LEFT, false))
			{
				vec2 cursor_position = keys->getCursorPosition();
				hud_element_type selected_type;
				string identifier;
				shared_ptr<hud_element> selected = artwork_thumbnails->getSelectedWithinArray(keys, cursor_position, selected_type, identifier);

				if (selected_type == THUMBNAIL)
					highlight = shared_ptr<artwork_thumbnail>(new artwork_thumbnail(selected->getStoredArt(), context, vec2(-0.15f, 0.15f), vec2(1.7f, 1.7f), 0.1f));

				else highlight = nullptr;
			}

			glfwSetTime(0.0f);
		}
	}

	context->setBackgroundColor(original_background);
	return menu_return;
}