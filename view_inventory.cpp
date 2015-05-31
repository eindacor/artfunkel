#include "header.h"
#include "player.h"
#include "artwork.h"
#include "utility_funcs.h"
#include "menus.h"
#include "gallery.h"
#include "hud.h"

int viewInventory_HUD(string data_path, const shared_ptr<ogl_context> &context,
	shared_ptr<key_handler> &keys, const shared_ptr<player> &current_player, 
	const shared_ptr<text_handler> &text, shared_ptr<texture_handler> &textures)
{
	vec4 original_background = context->getBackgroundColor();
	context->setBackgroundColor(vec4(0.5f, 0.3f, 0.0f, 1.0f));

	int display_count = 10;

	//TODO remove inventory copy mechanic. use actual inventory container with active iterators
	//add copies of the artwork instances to the local vector, so position can be manipulated
	vector<shared_ptr<artwork> >inventory_copy = current_player->getInventoryCopy();
	shared_ptr<dynamic_hud_array> artwork_thumbnails(new dynamic_hud_array(context, vec2(0.4f, .15f), 1.2f, 1.7f ,
		pair<horizontal_justification, vertical_justification>(H_LEFT, V_TOP)));

	artwork_thumbnails->setBackgroundColor(vec4(0.0f, 0.0f, 0.0f, 0.4f));

	//TODO equip dynamic array with function that generates thumbnails from artwork
	//add player's default frames to each
	for (const auto &i : inventory_copy)
	{
		i->applyFrameTemplate2D(context, textures, *(current_player->getDefaultFrame()));
		shared_ptr<artwork_thumbnail> thumbnail(new artwork_thumbnail(i, context, vec2(0.2f, 0.3f), 0.04f));
		thumbnail->setDrawSelected(highlight, fullBrightness);
		artwork_thumbnails->addElement(thumbnail);
	}

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

			context->swapBuffers();

			if (keys->checkPress(GLFW_KEY_ESCAPE, false))
			{
				menu_return = mainMenu(data_path, context, keys, text, textures);
				finished = (menu_return != 1);
			}

			if (keys->checkMouse(GLFW_MOUSE_BUTTON_LEFT, false))
			{
				vec2 cursor_position = keys->getCursorPosition();
				hud_element_type selected_type;
				string identifier;
				shared_ptr<hud_element> selected = artwork_thumbnails->getSelectedWithinArray(keys, cursor_position, selected_type, identifier);

				if (selected_type == THUMBNAIL)
					selected_painting = shared_ptr<artwork_thumbnail>(new artwork_thumbnail(selected->getStoredArt(), context, vec2(-.65f, 0.5f), vec2(0.7f, 1.0f), 0.1f));

				else selected_painting = nullptr;
			}

			if (keys->checkPress(GLFW_KEY_COMMA, false))
			{
				artwork_thumbnails->pageDown();
				selected_painting = nullptr;
				//title_text = nullptr;
				//info_text = nullptr;
				//rarity_text = nullptr;
			}

			if (keys->checkPress(GLFW_KEY_PERIOD, false))
			{
				artwork_thumbnails->pageUp();
				selected_painting = nullptr;
				//title_text = nullptr;
				//info_text = nullptr;
				//rarity_text = nullptr;
			}

			if ((keys->checkPress(GLFW_KEY_BACKSPACE, false) || keys->checkPress(GLFW_KEY_DELETE, false)) && selected_painting != nullptr)
			{
				current_player->removeWorkFromInventory(selected_painting->getStoredArt());

				artwork_thumbnails->clearElements();

				inventory_copy.clear();
				inventory_copy = current_player->getInventoryCopy();
				selected_painting = nullptr;

				for (const auto &i : inventory_copy)
				{
					i->applyFrameTemplate2D(context, textures, *(current_player->getDefaultFrame()));
					shared_ptr<artwork_thumbnail> thumbnail(new artwork_thumbnail(i, context, vec2(0.2f, 0.3f), 0.04f));
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