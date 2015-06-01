#include "header.h"
#include "loot.h"
#include "player.h"
#include "artwork.h"
#include "utility_funcs.h"
#include "menus.h"
#include "gallery.h"
#include "hud.h"

int openCrate(string data_path, const shared_ptr<ogl_context> &context, shared_ptr<key_handler> &keys,
	shared_ptr<player> &current_player, const shared_ptr<loot_generator> &lg, 
	const shared_ptr<text_handler> &text, shared_ptr<texture_handler> &textures)
{
	cout << "opening crate" << endl;
	vec4 original_background = context->getBackgroundColor();
	context->setBackgroundColor(vec4(0.0f, 0.0f, 0.5f, 1.0f));

	int drop_count = 6;

	//TODO revise so function doesn't rely on so many containers created/copied per run
	//add copies of the artwork instances to the local vector, so position can be manipulated
	vector<shared_ptr<artwork> > crate_contents = lg->generateArtworks(drop_count, 1.0f);

	shared_ptr<dynamic_hud_array> artwork_thumbnails(new dynamic_hud_array("thumbnails", context, vec2(0.0f, -0.85f), 2.0f, 0.3f,
		pair<horizontal_justification, vertical_justification>(H_CENTER, V_MIDDLE)));

	artwork_thumbnails->setBackgroundColor(vec4(0.0f, 0.0f, 0.0f, 0.4f));

	for (int i = 0; i < crate_contents.size(); i++)
	{
		string identifier = std::to_string(i) + "_" + crate_contents.at(i)->getData()->getArtistName() + "_"
			+ crate_contents.at(i)->getData()->getTitle();
		crate_contents.at(i)->applyFrameTemplate2D(context, textures, *(current_player->getDefaultFrame()));
		shared_ptr<artwork_thumbnail> thumbnail(new artwork_thumbnail(identifier, crate_contents.at(i), context, vec2(0.3f, 0.3f), 0.01f));
		thumbnail->setDrawSelected(highlight, fullBrightness);
		artwork_thumbnails->addElement(thumbnail);
	}

	shared_ptr<artwork_thumbnail> highlight = nullptr;

	//identify positions for text
	vec4 title_color(1.0f, 1.0f, 1.0f, 1.0f);
	vec4 rarity_color;
	vec4 info_color(0.6f, 0.6f, 0.6f, 1.0f);
	vec4 transparent_color(0.0f, 1.0f, 0.0f, 1.0f);
	float title_scale(0.05f);
	float info_scale(0.035f);
	vec2 title_screen_position(0.25f, 0.8f);
	float text_box_width(0.8f);

	shared_ptr<static_text> title_text(nullptr);
	shared_ptr<static_text> rarity_text(nullptr);
	shared_ptr<static_text> info_text(nullptr);

	vec4 alert_color(0.8f, 0.5f, 0.5f, 1.0f);
	float alert_scale(0.04f);
	float alert_buffer(0.1f);
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

				vec2 alert_position = (highlight != nullptr ? info_text->getLowerLeft() - vec2(0.0f, 0.1f) : title_screen_position);

				alert_text = text->getTextArray(alert_string, context, false, alert_color, transparent_color,
					"text", "text_color", "transparency_color",
					true, alert_position, alert_scale, text_box_width);
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
				{
					highlight = shared_ptr<artwork_thumbnail>(new artwork_thumbnail("highlight", selected->getStoredArt(), 
						context, vec2(-0.4f, 0.15f), vec2(1.2f, 1.7f), 0.1f));

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
						false, rarity_color, transparent_color, "text", "text_color", "transparency_color", true, title_text->getLowerLeft(), info_scale, text_box_width);

					string to_print = std::to_string(selected->getStoredArt()->getData()->getDate().getYear()) + "\n" + 
						selected->getStoredArt()->getData()->getArtistName();
					to_print += "\n$" + selected->getStoredArt()->getValue().getNumberString(true, false, 2);

					info_text = text->getTextArray(to_print, context, false, info_color, transparent_color, 
						"text", "text_color", "transparency_color", true, rarity_text->getLowerLeft(), info_scale, text_box_width);

					alert_text = nullptr;
				}

				else
				{
					highlight = nullptr;
					title_text = nullptr;
					info_text = nullptr;
					rarity_text = nullptr;
					alert_text = nullptr;
				}
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
				menu_return = mainMenu(data_path, context, keys, text, textures, current_player);
				finished = true;
			}

			context->swapBuffers();
			glfwSetTime(0.0f);
		}
	}

	context->setBackgroundColor(original_background);
	return menu_return;
}