#include "header.h"
#include "loot.h"
#include "player.h"
#include "artwork.h"
#include "utility_funcs.h"
#include "menus.h"
#include "gallery.h"
#include "hud.h"

int openCrate_HUD(string data_path, const shared_ptr<ogl_context> &context, shared_ptr<key_handler> &keys,
	const shared_ptr<player> &current_player, const shared_ptr<loot_generator> &lg, 
	const shared_ptr<text_handler> &text, shared_ptr<texture_handler> &textures)
{
	cout << "opening crate" << endl;
	vec4 original_background = context->getBackgroundColor();
	context->setBackgroundColor(vec4(0.0f, 0.0f, 0.5f, 1.0f));

	int drop_count = 6;

	//TODO revise so function doesn't rely on so many containers created/copied per run
	//add copies of the artwork instances to the local vector, so position can be manipulated
	vector<shared_ptr<artwork> > crate_contents = lg->generateArtworks(drop_count, 1.0f);

	shared_ptr<dynamic_hud_array> artwork_thumbnails(new dynamic_hud_array(context, vec2(0.85f, -0.0f), 0.3f, 2.0f,
		pair<horizontal_justification, vertical_justification>(H_CENTER, V_MIDDLE)));

	artwork_thumbnails->setBackgroundColor(vec4(0.0f, 0.0f, 0.0f, 0.4f));

	for (const auto &i : crate_contents)
	{
		i->applyFrameTemplate2D(context, textures, *(current_player->getDefaultFrame()));
		shared_ptr<artwork_thumbnail> thumbnail(new artwork_thumbnail(i, context, vec2(0.3f, 0.3f), 0.01f));
		thumbnail->setDrawSelected(highlight, fullBrightness);
		artwork_thumbnails->addElement(thumbnail);
	}

	shared_ptr<artwork_thumbnail> highlight = nullptr;

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

			if (keys->checkPress(GLFW_KEY_ENTER, false) && highlight != nullptr)
			{
				shared_ptr<artwork> current_selection = highlight->getStoredArt();
				bool already_owned = current_player->alreadyOwned(current_selection);

				string alert_string;

				if (already_owned)
					alert_string = current_selection->getData()->getTitle() + " is already in your inventory";

				else
				{
					if (current_player->addWorkToInventory(current_selection))
					{
						alert_string = (current_selection)->getData()->getTitle() + " has been added to your inventory";
						alert_string += "\n\nCollection value: $" + current_player->getCollectionValue().getNumberString(true, false, 2);
					}

					else alert_string = "Your inventory has reached the limit";
				}

				alert_text = text->getTextArray(alert_string, context, false, alert_color, transparent_color,
					"text", "text_color", "transparency_color",
					true, vec2(-0.75f, -0.75f), alert_scale, text_box_width);
			}

			if (keys->checkPress(GLFW_KEY_A, false))
			{
				string alert_string;
				for (const auto &i : crate_contents)
				{
					if (!current_player->alreadyOwned(i))
					{			
						if (current_player->addWorkToInventory(i))
						{
							alert_string = (i)->getData()->getTitle() + " has been added to your inventory";
							alert_string += "\n\nCollection value: $" + current_player->getCollectionValue().getNumberString(true, false, 2);
						}

						else alert_string = "Your inventory has reached the limit";
					}
				}

				alert_text = text->getTextArray(alert_string, context, false, alert_color, transparent_color,
					"text", "text_color", "transparency_color",
					true, vec2(-0.75f, -0.75f), alert_scale, text_box_width);
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

			if (title_text != nullptr)
				title_text->draw(camera, context);
			if (rarity_text != nullptr)
				rarity_text->draw(camera, context);
			if (info_text != nullptr)
				info_text->draw(camera, context);
			if (alert_text != nullptr)
				alert_text->draw(camera, context);

			if (keys->checkPress(GLFW_KEY_ESCAPE, false))
			{
				menu_return = mainMenu(data_path, context, keys, text, textures);
				finished = true;
			}

			context->swapBuffers();
			glfwSetTime(0.0f);
		}
	}

	context->setBackgroundColor(original_background);
	return menu_return;
}