#include "header.h"
#include "player.h"
#include "artwork.h"
#include "utility_funcs.h"
#include "menus.h"
#include "gallery.h"
#include "hud.h"
#include "hud_common.h"

shared_ptr<dynamic_hud_array> generateSortButtons(const shared_ptr<ogl_context> &context, const shared_ptr<text_handler> &text)
{
	shared_ptr<dynamic_hud_array> sort_buttons(new dynamic_hud_array("sort_buttons", context, vec2(1.0f, 1.0f),
		justpair(H_RIGHT, V_TOP), vec2(1.2f, 0.1f), justpair(H_CENTER, V_MIDDLE)));
	sort_buttons->setBackgroundColor(vec4(0.0f, 0.0f, 0.0f, 0.4f));

	sort_buttons->setDeselectOnMiss(false);

	float button_text_height(0.03f);
	vec4 button_color(0.7f, 0.7f, 0.7f, 1.0f);
	vec2 button_element_dimensions(0.24f, 0.075f);
	justpair button_just(H_CENTER, V_MIDDLE);
	bool button_italics = false;
	vec2 button_element_padding(0.015f, 0.0f);
	vec2 button_spacing_scale(0.8f, 1.0f);

	shared_ptr<text_area> value_sort_button(new text_area("value_sort",
		"by value", context, text, vec2(0.0f, 0.0f), justpair(H_CENTER, V_MIDDLE),
		button_element_dimensions, button_text_height, button_just, button_italics, button_color,
		"text", "text_color", button_element_padding, button_spacing_scale));
	sort_buttons->addElement(value_sort_button);

	shared_ptr<text_area> rarity_sort_button(new text_area("rarity_sort",
		"by rarity", context, text, vec2(0.0f, 0.0f), justpair(H_CENTER, V_MIDDLE),
		button_element_dimensions, button_text_height, button_just, button_italics, button_color,
		"text", "text_color", button_element_padding, button_spacing_scale));
	sort_buttons->addElement(rarity_sort_button);

	shared_ptr<text_area> artist_sort_button(new text_area("artist_sort",
		"by artist", context, text, vec2(0.0f, 0.0f), justpair(H_CENTER, V_MIDDLE),
		button_element_dimensions, button_text_height, button_just, button_italics, button_color,
		"text", "text_color", button_element_padding, button_spacing_scale));
	sort_buttons->addElement(artist_sort_button);

	shared_ptr<text_area> title_sort_button(new text_area("title_sort",
		"by title", context, text, vec2(0.0f, 0.0f), justpair(H_CENTER, V_MIDDLE),
		button_element_dimensions, button_text_height, button_just, button_italics, button_color,
		"text", "text_color", button_element_padding, button_spacing_scale));
	sort_buttons->addElement(title_sort_button);

	return sort_buttons;
}

int viewInventory(string data_path, const shared_ptr<ogl_context> &context,
	shared_ptr<key_handler> &keys, shared_ptr<player> &current_player, 
	const shared_ptr<text_handler> &text, shared_ptr<texture_handler> &textures)
{
	vec4 original_background = context->getBackgroundColor();
	context->setBackgroundColor(vec4(0.5f, 0.3f, 0.0f, 1.0f));

	int display_count = 10;

	shared_ptr<dynamic_hud_array> sort_buttons = generateSortButtons(context, text);
	sort_option current_sort = NO_SORT;
	bool ascending = false;

	//TODO remove inventory copy mechanic. use actual inventory container with active iterators
	//add copies of the artwork instances to the local vector, so position can be manipulated
	vector<shared_ptr<artwork> >inventory_copy = current_player->getInventoryCopy();
	shared_ptr<dynamic_hud_array> artwork_thumbnails(new dynamic_hud_array("thumbnails", context, vec2(1.0f, 0.9f), 
		justpair(H_RIGHT, V_TOP), vec2(1.2f, 1.65f), justpair(H_LEFT, V_TOP)));
	artwork_thumbnails->setBackgroundColor(vec4(0.0f, 0.0f, 0.0f, 0.4f));
	artwork_thumbnails->setDeselectOnMiss(true);
	vec2 thumbsize(0.2f, 0.2f);
	float thumbpadding(0.04f);
	refreshThumbnails(context, textures, current_player, inventory_copy, artwork_thumbnails, thumbsize, thumbpadding);

	//TODO equip dynamic array with function that generates thumbnails from artwork
	//add player's default frames to each

	/////////////////////UPDATED HUD
	//identify positions for text
	shared_ptr<dynamic_hud_array> work_info(new dynamic_hud_array("description", context, vec2(-1.0f, -.75f),
		justpair(H_LEFT, V_BOTTOM), vec2(0.8f, 0.75f), justpair(H_LEFT, V_MIDDLE), vec2(0.02f, 0.1f)));

	work_info->setBackgroundColor(vec4(0.0f, 0.0f, 0.0f, 0.5f));
	setWorkInfoFields(context, text, work_info);
	
	shared_ptr<dynamic_hud_array> player_summary = generatePlayerInfo(context, text, current_player);

	shared_ptr<dynamic_hud_array> blank_element(new dynamic_hud_array("placeholder", context, vec2(1.0f, -1.0f), justpair(H_RIGHT, V_BOTTOM), vec2(1.0f, 0.25f),
		justpair(H_LEFT, V_MIDDLE), vec2(0.02f, 0.1f)));

	blank_element->setBackgroundColor(vec4(0.0f, 0.0f, 0.0f, 0.7f));

	////////////////////////////////

	shared_ptr<artwork_thumbnail> selected_painting = nullptr;

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
	
			player_summary->draw(context, camera);
			artwork_thumbnails->draw(context, camera);
			sort_buttons->draw(context, camera);

			if (selected_painting != nullptr)
			{
				work_info->draw(context, camera);
				selected_painting->draw(context, camera);
			}

			else work_info->drawBackground(context, camera);

			context->swapBuffers();

			if (keys->checkPress(GLFW_KEY_ESCAPE, false))
			{
				menu_return = mainMenu(data_path, context, keys, current_player, text, textures);
				finished = (menu_return != 1);
			}
			
			vec2 cursor_position = keys->getCursorPosition();
			shared_ptr<hud_element> selected_element = artwork_thumbnails->getMouseoverElement(cursor_position, true);

			if (keys->checkMouse(GLFW_MOUSE_BUTTON_LEFT, false))
			{
				if (selected_element != nullptr && selected_element->getType() == THUMBNAIL)
				{				
					selected_painting = shared_ptr<artwork_thumbnail>(new artwork_thumbnail("selected", selected_element->getStoredArt(), context,
						vec2(-1.0f, 1.0f), justpair(H_LEFT, V_TOP), vec2(0.8f, 1.0f), 0.1f));
					
					setWorkInfoDescription(work_info, selected_painting->getStoredArt());
				}

				else
				{
					selected_element = sort_buttons->getMouseoverElement(cursor_position, true);

					if (selected_element != nullptr)
					{
						string identifier = selected_element->getIdentifier();

						if (identifier == "value_sort")
						{
							if (current_sort == VALUE)
								ascending = !ascending;

							else ascending = false;

							current_sort = VALUE;
							sortArtVec(inventory_copy, VALUE, ascending);
							refreshThumbnails(context, textures, current_player, inventory_copy, artwork_thumbnails, thumbsize, thumbpadding);
							selected_painting = nullptr;
						}

						else if (identifier == "rarity_sort")
						{
							if (current_sort == RARITY)
								ascending = !ascending;

							else ascending = false;

							current_sort = RARITY;
							sortArtVec(inventory_copy, RARITY, ascending);
							refreshThumbnails(context, textures, current_player, inventory_copy, artwork_thumbnails, thumbsize, thumbpadding);
							selected_painting = nullptr;
						}

						else if (identifier == "title_sort")
						{
							if (current_sort == TITLE)
								ascending = !ascending;

							else ascending = true;

							current_sort = TITLE;
							sortArtVec(inventory_copy, TITLE, ascending);
							refreshThumbnails(context, textures, current_player, inventory_copy, artwork_thumbnails, thumbsize, thumbpadding);
							selected_painting = nullptr;
						}

						else if (identifier == "artist_sort")
						{
							if (current_sort == ARTIST_NAME)
								ascending = !ascending;

							else ascending = true;

							current_sort = ARTIST_NAME;
							sortArtVec(inventory_copy, ARTIST_NAME, ascending);
							refreshThumbnails(context, textures, current_player, inventory_copy, artwork_thumbnails, thumbsize, thumbpadding);
							selected_painting = nullptr;
						}
					}
				}
			}

			if (keys->checkPress(GLFW_KEY_COMMA, false))
			{
				artwork_thumbnails->pageDown();
				selected_painting = nullptr;
			}

			if (keys->checkPress(GLFW_KEY_PERIOD, false))
			{
				artwork_thumbnails->pageUp();
				selected_painting = nullptr;
			}

			if (keys->checkPress(GLFW_KEY_EQUAL, false) && current_player->getName() == "EindacorDS")
			{
				current_player->addFunds(bignum("100000"));
				refreshPlayerInfo(player_summary, current_player);
			}

			if ((keys->checkPress(GLFW_KEY_BACKSPACE, false) || keys->checkPress(GLFW_KEY_DELETE, false)) && selected_painting != nullptr)
			{
				current_player->removeWorkFromInventory(selected_painting->getStoredArt());
				refreshPlayerInfo(player_summary, current_player);

				inventory_copy.clear();
				inventory_copy = current_player->getInventoryCopy();
				sortArtVec(inventory_copy, current_sort, ascending);
				refreshThumbnails(context, textures, current_player, inventory_copy, artwork_thumbnails, thumbsize, thumbpadding);
				selected_painting = nullptr;
			}

			glfwSetTime(0.0f);
		}
	}

	context->setBackgroundColor(original_background);
	return menu_return;
}