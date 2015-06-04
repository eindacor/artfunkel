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
	const shared_ptr<text_handler> &text, shared_ptr<texture_handler> &textures, rarity r, int count)
{
	cout << "opening crate" << endl;
	vec4 original_background = context->getBackgroundColor();
	context->setBackgroundColor(vec4(0.0f, 0.0f, 0.5f, 1.0f));

	int drop_count = 6;

	//TODO revise so function doesn't rely on so many containers created/copied per run
	//add copies of the artwork instances to the local vector, so position can be manipulated
	vector<shared_ptr<artwork> > crate_contents = lg->generateArtworks(count, r);
	current_player->deductPayment(lg->getCrateCost(r, count));
	cout << "charged: $" << lg->getCrateCost(r, count).getNumberString(true, false, 0) << endl;

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

	shared_ptr<artwork_thumbnail> painting_selected = nullptr;

	/////////////////////UPDATED HUD
	//identify positions for text
	shared_ptr<dynamic_hud_array> work_description(new dynamic_hud_array("description", context, vec2(0.6f, 0.15f), 0.8f, 1.7f,
		pair<horizontal_justification, vertical_justification>(H_LEFT, V_MIDDLE), vec2(0.02f, 0.1f)));

	work_description->setBackgroundColor(vec4(0.0f, 0.0f, 0.0f, 0.5f));

	float title_text_height(0.045f);
	vec4 title_color(1.0f, 1.0f, 1.0f, 1.0f);
	vec2 title_centerpoint(0.0f, 0.0f);
	vec2 title_element_dimensions(0.76f, 0.1f);
	pair <horizontal_justification, vertical_justification> title_just(H_LEFT, V_MIDDLE);
	bool title_italics = true;
	vec2 title_element_padding(0.015f, 0.0f / context->getAspectRatio());
	vec2 title_spacing_scale(0.8f, 1.1f);

	shared_ptr<text_area> title_text(new text_area("title_text", "not yet set",
		context, text, title_centerpoint, title_element_dimensions, title_text_height, title_just, title_italics, title_color,
		"text", "text_color", title_element_padding, title_spacing_scale));

	float rarity_text_height(0.03f);
	vec4 rarity_color(1.0f, 1.0f, 1.0f, 1.0f);
	vec2 rarity_centerpoint(0.0f, 0.0f);
	vec2 rarity_element_dimensions(0.76f, 0.032f);
	pair <horizontal_justification, vertical_justification> rarity_just(H_LEFT, V_MIDDLE);
	bool rarity_italics = false;
	vec2 rarity_element_padding(0.025f, 0.0f);
	vec2 rarity_spacing_scale(0.8f, 1.0f);

	shared_ptr<text_area> rarity_text(new text_area("rarity_text", "not yet set",
		context, text, rarity_centerpoint, rarity_element_dimensions, rarity_text_height, rarity_just, rarity_italics, rarity_color,
		"text", "text_color", rarity_element_padding, rarity_spacing_scale));

	float artist_text_height(0.03f);
	vec4 artist_color(0.7f, 0.7f, 0.7f, 1.0f);
	vec2 artist_centerpoint(0.0f, 0.0f);
	vec2 artist_element_dimensions(0.76f, 0.032f);
	pair <horizontal_justification, vertical_justification> artist_just(H_LEFT, V_MIDDLE);
	bool artist_italics = false;
	vec2 artist_element_padding(0.025f, 0.0f);
	vec2 artist_spacing_scale(0.8f, 1.0f);

	shared_ptr<text_area> artist_text(new text_area("artist_text", "not yet set",
		context, text, artist_centerpoint, artist_element_dimensions, artist_text_height, artist_just, artist_italics, artist_color,
		"text", "text_color", artist_element_padding, artist_spacing_scale));

	float value_text_height(0.03f);
	vec4 value_color(0.7f, 0.7f, 0.7f, 1.0f);
	vec2 value_centerpoint(0.0f, 0.0f);
	vec2 value_element_dimensions(0.76f, 0.032f);
	pair <horizontal_justification, vertical_justification> value_just(H_LEFT, V_MIDDLE);
	bool value_italics = false;
	vec2 value_element_padding(0.025f, 0.0f);
	vec2 value_spacing_scale(0.8f, 1.0f);

	shared_ptr<text_area> value_text(new text_area("value_text", "not yet set",
		context, text, value_centerpoint, value_element_dimensions, value_text_height, value_just, value_italics, value_color,
		"text", "text_color", value_element_padding, value_spacing_scale));

	work_description->addElement(title_text);
	work_description->addElement(rarity_text);
	work_description->addElement(artist_text);
	work_description->addElement(value_text);

	////////////////////////////////

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

			if (painting_selected != nullptr)
			{
				work_description->draw(context, camera);
				painting_selected->draw(context, camera);
			}

			else work_description->drawBackground(context, camera);

			if (keys->checkPress(GLFW_KEY_ENTER, false) && painting_selected != nullptr)
			{
				shared_ptr<artwork> current_selection = painting_selected->getStoredArt();
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
			}

			if (keys->checkPress(GLFW_KEY_BACKSPACE, false) && painting_selected != nullptr)
			{
				shared_ptr<artwork> current_selection = painting_selected->getStoredArt();

				for (vector<shared_ptr<artwork> >::iterator it = crate_contents.begin(); it != crate_contents.end(); it++)
				{
					if (**it == *current_selection)
					{
						crate_contents.erase(it);
						current_player->addFunds(current_selection->getValue());		
						painting_selected = nullptr;
						break;
					}
				}

				artwork_thumbnails->clearElements();
				for (int i = 0; i < crate_contents.size(); i++)
				{
					string identifier = std::to_string(i) + "_" + crate_contents.at(i)->getData()->getArtistName() + "_"
						+ crate_contents.at(i)->getData()->getTitle();
					crate_contents.at(i)->applyFrameTemplate2D(context, textures, *(current_player->getDefaultFrame()));
					shared_ptr<artwork_thumbnail> thumbnail(new artwork_thumbnail(identifier, crate_contents.at(i), context, vec2(0.3f, 0.3f), 0.01f));
					thumbnail->setDrawSelected(highlight, fullBrightness);
					artwork_thumbnails->addElement(thumbnail);
				}
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

				//alert_text = text->getTextArray(alert_string, context, false, alert_color, transparent_color,
					//"text", "text_color", "transparency_color",
					//true, vec2(-0.75f, -0.75f), alert_scale, text_box_width);
			}

			if (keys->checkMouse(GLFW_MOUSE_BUTTON_LEFT, false))
			{
				vec2 cursor_position = keys->getCursorPosition();
				hud_element_type selected_type;
				string identifier;
				shared_ptr<hud_element> selected = artwork_thumbnails->getSelectedWithinArray(keys, cursor_position, selected_type, identifier);

				if (selected_type == THUMBNAIL)
				{
					painting_selected = shared_ptr<artwork_thumbnail>(new artwork_thumbnail("painting_selected", selected->getStoredArt(),
						context, vec2(-0.4f, 0.15f), vec2(1.2f, 1.7f), 0.1f));

					title_text->setText(painting_selected->getStoredArt()->getData()->getTitle());
					artist_text->setText(painting_selected->getStoredArt()->getData()->getArtistName());
					value_text->setText("$" + painting_selected->getStoredArt()->getValue().getNumberString(true, false, 2));
					rarity_text->setText(stringFromRarity(painting_selected->getStoredArt()->getData()->getRarity()));

					switch (painting_selected->getStoredArt()->getData()->getRarity())
					{
					case COMMON: rarity_text->setColor(vec4(0.6f, 0.9f, 0.6f, 1.0f)); break;
					case UNCOMMON: rarity_text->setColor(vec4(0.6f, 0.6f, 0.9f, 1.0f)); break;
					case RARE: rarity_text->setColor(vec4(0.9f, 0.9f, 0.6f, 1.0f)); break;
					case LEGENDARY: rarity_text->setColor(vec4(1.0f, 0.75f, 0.6f, 1.0f)); break;
					case MASTERPIECE: rarity_text->setColor(vec4(0.6f, 0.9f, 0.9f, 1.0f)); break;
					}

					//alert_text = nullptr;
				}

				else
				{
					painting_selected = nullptr;
					//title_text = nullptr;
					//info_text = nullptr;
					//rarity_text = nullptr;
					//alert_text = nullptr;
				}
			}

			//if (title_text != nullptr)
				//title_text->draw(camera, context);
			//if (rarity_text != nullptr)
				//rarity_text->draw(camera, context);
			//if (info_text != nullptr)
				//info_text->draw(camera, context);
			//if (alert_text != nullptr)
				//alert_text->draw(camera, context);

			if (keys->checkPress(GLFW_KEY_ESCAPE, false))
			{
				menu_return = mainMenu(data_path, context, keys, current_player, text, textures);
				finished = true;
			}

			if (keys->checkPress(GLFW_KEY_COMMA, false))
			{
				artwork_thumbnails->pageDown();
				painting_selected = nullptr;
			}

			if (keys->checkPress(GLFW_KEY_PERIOD, false))
			{
				artwork_thumbnails->pageUp();
				painting_selected = nullptr;
			}

			context->swapBuffers();
			glfwSetTime(0.0f);
		}
	}

	context->setBackgroundColor(original_background);
	return menu_return;
}