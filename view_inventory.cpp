#include "header.h"
#include "player.h"
#include "artwork.h"
#include "utility_funcs.h"
#include "menus.h"
#include "gallery.h"
#include "hud.h"

int viewInventory(string data_path, const shared_ptr<ogl_context> &context,
	shared_ptr<key_handler> &keys, shared_ptr<player> &current_player, 
	const shared_ptr<text_handler> &text, shared_ptr<texture_handler> &textures)
{
	vec4 original_background = context->getBackgroundColor();
	context->setBackgroundColor(vec4(0.5f, 0.3f, 0.0f, 1.0f));

	int display_count = 10;

	//TODO remove inventory copy mechanic. use actual inventory container with active iterators
	//add copies of the artwork instances to the local vector, so position can be manipulated
	vector<shared_ptr<artwork> >inventory_copy = current_player->getInventoryCopy();
	shared_ptr<dynamic_hud_array> artwork_thumbnails(new dynamic_hud_array("thumbnails", context, vec2(0.4f, .15f), 1.2f, 1.7f ,
		pair<horizontal_justification, vertical_justification>(H_LEFT, V_TOP)));

	artwork_thumbnails->setBackgroundColor(vec4(0.0f, 0.0f, 0.0f, 0.4f));

	//TODO equip dynamic array with function that generates thumbnails from artwork
	//add player's default frames to each
	for (int i = 0; i < inventory_copy.size(); i++)
	{
		string identifier = std::to_string(i) + "_" + inventory_copy.at(i)->getData()->getArtistName() + "_"
			+ inventory_copy.at(i)->getData()->getTitle();
		inventory_copy.at(i)->applyFrameTemplate2D(context, textures, *(current_player->getDefaultFrame()));
		shared_ptr<artwork_thumbnail> thumbnail(new artwork_thumbnail(identifier, inventory_copy.at(i), context, vec2(0.2f, 0.3f), 0.04f));
		thumbnail->setDrawSelected(highlight, fullBrightness);
		artwork_thumbnails->addElement(thumbnail);
	}

	//identify positions for text
	vec4 title_color(1.0f, 1.0f, 1.0f, 1.0f);
	vec4 rarity_color;
	vec4 info_color(0.6f, 0.6f, 0.6f, 1.0f);
	vec4 transparent_color(0.0f, 1.0f, 0.0f, 1.0f);
	float title_scale(0.05f);
	float info_scale(0.035f);
	vec2 title_screen_position(-0.95f, -0.1f);
	float text_box_width(0.7f);

	shared_ptr<static_text> title_text(nullptr);
	shared_ptr<static_text> rarity_text(nullptr);
	shared_ptr<static_text> info_text(nullptr);

	vec4 alert_color(0.8f, 0.5f, 0.5f, 1.0f);
	float alert_scale(0.04f);
	float alert_buffer(0.1f);
	shared_ptr<static_text> alert_text(nullptr);

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
	
			artwork_thumbnails->draw(context, camera);

			if (selected_painting != nullptr)
				selected_painting->draw(context, camera);

			if (title_text != nullptr)
				title_text->draw(camera, context);
			if (info_text != nullptr)
				info_text->draw(camera, context);
			if (rarity_text != nullptr)
				rarity_text->draw(camera, context);

			context->swapBuffers();

			if (keys->checkPress(GLFW_KEY_ESCAPE, false))
			{
				menu_return = mainMenu(data_path, context, keys, text, textures, current_player);
				finished = (menu_return != 1);
			}

			if (keys->checkMouse(GLFW_MOUSE_BUTTON_LEFT, false))
			{
				vec2 cursor_position = keys->getCursorPosition();
				hud_element_type selected_type;
				string identifier;
				shared_ptr<hud_element> selected = artwork_thumbnails->getSelectedWithinArray(keys, cursor_position, selected_type, identifier);

				if (selected_type == THUMBNAIL)
				{
					selected_painting = shared_ptr<artwork_thumbnail>(new artwork_thumbnail("selected", selected->getStoredArt(), context, 
						vec2(-.65f, 0.5f), vec2(0.7f, 1.0f), 0.1f));
					
					title_text = text->getTextArray(selected->getStoredArt()->getData()->getTitle(), context,
						true, title_color, transparent_color, "text", "text_color", "transparency_color", true, 
						title_screen_position, title_scale, text_box_width);

					switch (selected->getStoredArt()->getData()->getRarity())
					{
					case COMMON: rarity_color = vec4(0.6f, 0.9f, 0.6f, 1.0f); break;
					case UNCOMMON: rarity_color = vec4(0.6f, 0.6f, 0.9f, 1.0f); break;
					case RARE: rarity_color = vec4(0.9f, 0.9f, 0.6f, 1.0f); break;
					case LEGENDARY: rarity_color = vec4(1.0f, 0.75f, 0.6f, 1.0f); break;
					case MASTERPIECE: rarity_color = vec4(0.6f, 0.9f, 0.9f, 1.0f); break;
					}

					rarity_text = text->getTextArray(stringFromRarity(selected->getStoredArt()->getData()->getRarity()), context,
						false, rarity_color, transparent_color, "text", "text_color", "transparency_color", true, 
						title_text->getLowerLeft(), info_scale, text_box_width);

					string to_print = std::to_string(selected->getStoredArt()->getData()->getDate().getYear()) + "\n" +
						selected->getStoredArt()->getData()->getArtistName();
					to_print += "\n$" + selected->getStoredArt()->getValue().getNumberString(true, false, 2);

					info_text = text->getTextArray(to_print, context, false, info_color, transparent_color,
						"text", "text_color", "transparency_color", true, rarity_text->getLowerLeft(), info_scale, text_box_width);

					alert_text = nullptr;
				}

				else
				{
					selected_painting = nullptr;
					title_text = nullptr;
					info_text = nullptr;
					rarity_text = nullptr;
					alert_text = nullptr;
				}
			}

			if (keys->checkPress(GLFW_KEY_COMMA, false))
			{
				artwork_thumbnails->pageDown();
				selected_painting = nullptr;
				title_text = nullptr;
				info_text = nullptr;
				rarity_text = nullptr;
				alert_text = nullptr;
			}

			if (keys->checkPress(GLFW_KEY_PERIOD, false))
			{
				artwork_thumbnails->pageUp();
				selected_painting = nullptr;
				title_text = nullptr;
				info_text = nullptr;
				rarity_text = nullptr;
				alert_text = nullptr;
			}

			if ((keys->checkPress(GLFW_KEY_BACKSPACE, false) || keys->checkPress(GLFW_KEY_DELETE, false)) && selected_painting != nullptr)
			{
				current_player->removeWorkFromInventory(selected_painting->getStoredArt());

				artwork_thumbnails->clearElements();

				inventory_copy.clear();
				inventory_copy = current_player->getInventoryCopy();
				selected_painting = nullptr;
				title_text = nullptr;
				info_text = nullptr;
				rarity_text = nullptr;
				alert_text = nullptr;

				for (int i = 0; i < inventory_copy.size(); i++)
				{
					string identifier = std::to_string(i) + "_" + inventory_copy.at(i)->getData()->getArtistName() + "_"
						+ inventory_copy.at(i)->getData()->getTitle();
					inventory_copy.at(i)->applyFrameTemplate2D(context, textures, *(current_player->getDefaultFrame()));
					shared_ptr<artwork_thumbnail> thumbnail(new artwork_thumbnail(identifier, inventory_copy.at(i), context, vec2(0.2f, 0.3f), 0.04f));
					thumbnail->setDrawSelected(highlight, fullBrightness);
					artwork_thumbnails->addElement(thumbnail);
				}
			}

			glfwSetTime(0.0f);
		}
	}

	context->setBackgroundColor(original_background);
	return menu_return;
}