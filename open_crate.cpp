#include "header.h"
#include "loot.h"
#include "player.h"
#include "artwork.h"
#include "utility_funcs.h"
#include "menus.h"
#include "gallery.h"
#include "hud.h"
#include "hud_common.h"

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

	shared_ptr<dynamic_hud_array> player_summary = generatePlayerInfo(context, text, current_player);

	shared_ptr<dynamic_hud_array> artwork_thumbnails(new dynamic_hud_array("thumbnails", context, vec2(0.0f, -.75f), justpair(H_CENTER, V_BOTTOM), 
		vec2(2.0f, 0.3f), justpair(H_CENTER, V_MIDDLE)));
	artwork_thumbnails->setBackgroundColor(vec4(0.0f, 0.0f, 0.0f, 0.4f));
	vec2 thumbsize(0.3f, 0.3f);
	float thumbpadding(.01f);
	refreshThumbnails(context, textures, current_player, crate_contents, artwork_thumbnails, thumbsize, thumbpadding);

	shared_ptr<artwork_thumbnail> painting_selected = nullptr;

	/////////////////////UPDATED HUD
	//identify positions for text
	shared_ptr<dynamic_hud_array> work_info(new dynamic_hud_array("description", context, vec2(1.0f, 1.0f), justpair(H_RIGHT, V_TOP), vec2(0.8f, 1.0f),
		justpair(H_LEFT, V_MIDDLE), vec2(0.02f, 0.1f)));
	work_info->setBackgroundColor(vec4(0.0f, 0.0f, 0.0f, 0.5f));
	setWorkInfoFields(context, text, work_info, 1.2f);

	shared_ptr<dynamic_hud_array> alert_info(new dynamic_hud_array("alerts", context, vec2(1.0f, 0.0f), justpair(H_RIGHT, V_TOP), vec2(0.8f, 0.45f),
		justpair(H_LEFT, V_TOP), vec2(0.02f, 0.1f)));
	alert_info->setBackgroundColor(vec4(0.0f, 0.0f, 0.0f, 0.5f));

	float alert_text_height(0.03f);
	vec4 alert_color(0.7f, 0.7f, 0.7f, 1.0f);
	vec2 alert_element_dimensions(work_info->getAllowableWidth(), 0.12f);
	pair <horizontal_justification, vertical_justification> alert_just(H_LEFT, V_BOTTOM);
	bool alert_italics = false;
	vec2 alert_element_padding(0.025f, 0.0f);
	vec2 alert_spacing_scale(0.8f, 1.0f);

	shared_ptr<text_area> alert_text(new text_area("alert_text", "not yet set",
		context, text, alert_element_dimensions, alert_text_height, alert_just, alert_italics, alert_color,
		"text", "text_color", alert_element_padding, alert_spacing_scale));

	alert_text->setVisibility(false);
	alert_info->addElement(alert_text);

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
				work_info->draw(context, camera);
				painting_selected->draw(context, camera);
			}

			else work_info->drawBackground(context, camera);

			player_summary->draw(context, camera);
			alert_info->draw(context, camera);

			if (keys->checkPress(GLFW_KEY_ENTER, false) && painting_selected != nullptr)
			{
				shared_ptr<artwork> current_selection = painting_selected->getStoredArt();
				bool already_owned = current_player->alreadyOwned(current_selection);

				string alert_string;

				if (already_owned)
				{
					alert_string = current_selection->getData()->getTitle() + " is already in your inventory";
					alert_text->setColor(vec4(1.0f, 0.0f, 0.0f, 1.0f));
					alert_text->setText(alert_string);
					alert_text->setVisibility(true);
				}

				else
				{
					if (current_player->addWorkToInventory(current_selection))
					{
						alert_string = (current_selection)->getData()->getTitle() + " has been added to your inventory";
						alert_text->setColor(vec4(0.0f, 1.0f, 0.0f, 1.0f));
						alert_text->setText(alert_string);
						alert_text->setVisibility(true);

						refreshPlayerInfo(player_summary, current_player);

						for (vector<shared_ptr<artwork> >::iterator it = crate_contents.begin(); it != crate_contents.end(); it++)
						{
							if (**it == *current_selection)
							{
								crate_contents.erase(it);
								painting_selected = nullptr;
								break;
							}
						}

						refreshThumbnails(context, textures, current_player, crate_contents, artwork_thumbnails, thumbsize, thumbpadding);
					}

					else
					{
						alert_string = "Your inventory has reached the limit";
						alert_text->setColor(vec4(1.0f, 0.0f, 0.0f, 1.0f));
						alert_text->setText(alert_string);
						alert_text->setVisibility(true);
					}

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
						refreshPlayerInfo(player_summary, current_player);
						painting_selected = nullptr;

						string alert_string = current_selection->getData()->getTitle() + " has been sold for $" + current_selection->getValue().getNumberString(true, false, 2);

						alert_text->setColor(vec4(0.0f, 1.0f, 0.0f, 1.0f));
						alert_text->setText(alert_string);
						alert_text->setVisibility(true);

						break;
					}
				}

				refreshThumbnails(context, textures, current_player, crate_contents, artwork_thumbnails, thumbsize, thumbpadding);
			}

			//temporarily disable
			if (keys->checkPress(GLFW_KEY_A, false) && false)
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
							refreshPlayerInfo(player_summary, current_player);
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
				shared_ptr<hud_element> selected_element = artwork_thumbnails->getMouseoverElement(cursor_position, true);

				if (selected_element != nullptr && selected_element->getType() == THUMBNAIL)
				{
					painting_selected = shared_ptr<artwork_thumbnail>(new artwork_thumbnail("painting_selected", selected_element->getStoredArt(),
						context, vec2(-1.0f, 1.0f), justpair(H_LEFT, V_TOP), vec2(1.2f, 1.45f), 0.1f));

					setWorkInfoDescription(work_info, painting_selected->getStoredArt());

					alert_text->setVisibility(false);
				}

				else
				{
					painting_selected = nullptr;
					alert_text->setVisibility(false);
					//title_text = nullptr;
					//info_text = nullptr;
					//rarity_text = nullptr;
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

			if (keys->checkPress(GLFW_KEY_ESCAPE, false) && crate_contents.size() == 0)
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