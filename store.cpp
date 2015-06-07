#include "header.h"
#include "loot.h"
#include "player.h"
#include "artwork.h"
#include "utility_funcs.h"
#include "menus.h"
#include "gallery.h"
#include "hud.h"
#include "hud_common.h"

//returns pair to add to crate map
pair<string, pair<rarity, int> > addCrateButton(const shared_ptr<ogl_context> &context, const shared_ptr<text_handler> &text,
	const shared_ptr<player> &current_player, const shared_ptr<loot_generator> &lg, rarity r, int count, shared_ptr<dynamic_hud_array> &crates)
{
	string crate_size;
	if (count / 5 > 2)
		crate_size = "Mega";

	else if (count / 5 > 1)
		crate_size = "Large";

	else crate_size = "Small";

	string button_id = crate_size + " " + stringFromRarity(r) + " Crate";

	//overall container of title and value elements
	pair <horizontal_justification, vertical_justification> button_just(H_LEFT, V_MIDDLE);
	shared_ptr<dynamic_hud_array> button(new dynamic_hud_array(button_id, context, vec2(0.0f), justpair(H_CENTER, V_MIDDLE), vec2(0.5f, .14f), button_just, vec2(0.05f, 0.0f)));

	float crate_title_text_height(0.042f);
	vec4 crate_title_color(0.8f, 0.8f, 0.8f, 1.0f);
	vec2 crate_title_element_dimensions(0.46f, 0.052f);
	pair <horizontal_justification, vertical_justification> option_title_just(H_LEFT, V_MIDDLE);
	bool crate_title_italics = false;
	vec2 crate_title_element_padding(0.025f, 0.0f);
	vec2 crate_title_spacing_scale(0.8f, 1.0f);

	shared_ptr<text_area> crate_title_button(new text_area(button_id, button_id,
		context, text, vec2(0.0f, 0.0f), justpair(H_CENTER, V_MIDDLE), crate_title_element_dimensions, crate_title_text_height, option_title_just,
		crate_title_italics, crate_title_color, "text", "text_color", crate_title_element_padding, crate_title_spacing_scale));

	switch (r)
	{
	case COMMON: crate_title_button->setColor(vec4(0.6f, 0.9f, 0.6f, 1.0f)); break;
	case UNCOMMON: crate_title_button->setColor(vec4(0.6f, 0.6f, 0.9f, 1.0f)); break;
	case RARE: crate_title_button->setColor(vec4(0.9f, 0.9f, 0.6f, 1.0f)); break;
	case LEGENDARY: crate_title_button->setColor(vec4(1.0f, 0.75f, 0.6f, 1.0f)); break;
	}

	float crate_value_text_height(0.04f);
	vec4 crate_value_color(0.8f, 0.8f, 0.8f, 1.0f);
	vec2 crate_value_element_dimensions(0.46f, 0.045f);
	pair <horizontal_justification, vertical_justification> crate_value_just(H_LEFT, V_MIDDLE);
	bool crate_value_italics = false;
	vec2 crate_value_element_padding(0.025f, 0.0f);
	vec2 crate_value_spacing_scale(0.8f, 1.0f);

	shared_ptr<text_area> crate_value_button(new text_area(button_id, "$" + lg->getCrateCost(r, count).getNumberString(true, false, 0),
		context, text, vec2(0.0f, 0.0f), justpair(H_CENTER, V_MIDDLE), crate_value_element_dimensions, crate_value_text_height, crate_value_just,
		crate_value_italics, crate_value_color, "text", "text_color", crate_value_element_padding, crate_value_spacing_scale));

	if (current_player->getBankBalance() < lg->getCrateCost(r, count))
	{
		button->setSelectable(false);
		crate_title_button->setSelectable(false);
		crate_value_button->setSelectable(false);
	}

	else button->setSelectable(true);

	button->addElement(crate_title_button);
	button->addElement(crate_value_button);
	crates->addElement(button);

	return pair<string, pair<rarity, int> >(button_id, pair<rarity, int>(r, count));
}

int visitStore(string data_path, const shared_ptr<ogl_context> &context, shared_ptr<key_handler> &keys,
	shared_ptr<player> &current_player, const shared_ptr<loot_generator> &lg, const shared_ptr<text_handler> &text, shared_ptr<texture_handler> &textures)
{
	shared_ptr<dynamic_hud_array> crate_menu(new dynamic_hud_array("description", context, vec2(-1.0f, 1.0f), justpair(H_LEFT, V_TOP), vec2(0.5f, 1.75f),
		justpair(H_LEFT, V_MIDDLE), vec2(0.02f, 0.0f)));

	crate_menu->setBackgroundColor(vec4(0.0f, 0.0f, 0.0f, 0.5f));
	crate_menu->setDeselectOnMiss(true);

	shared_ptr<dynamic_hud_array> player_summary = generatePlayerInfo(context, text, current_player);

	map<string, pair<rarity, int> >crate_map;

	crate_map.insert(addCrateButton(context, text, current_player, lg, COMMON, 5, crate_menu));
	crate_map.insert(addCrateButton(context, text, current_player, lg, COMMON, 10, crate_menu));
	crate_map.insert(addCrateButton(context, text, current_player, lg, COMMON, 15, crate_menu));
	crate_map.insert(addCrateButton(context, text, current_player, lg, UNCOMMON, 5, crate_menu));
	crate_map.insert(addCrateButton(context, text, current_player, lg, UNCOMMON, 10, crate_menu));
	crate_map.insert(addCrateButton(context, text, current_player, lg, UNCOMMON, 15, crate_menu));
	crate_map.insert(addCrateButton(context, text, current_player, lg, RARE, 5, crate_menu));
	crate_map.insert(addCrateButton(context, text, current_player, lg, RARE, 10, crate_menu));
	crate_map.insert(addCrateButton(context, text, current_player, lg, RARE, 15, crate_menu));
	crate_map.insert(addCrateButton(context, text, current_player, lg, LEGENDARY, 5, crate_menu));
	crate_map.insert(addCrateButton(context, text, current_player, lg, LEGENDARY, 10, crate_menu));
	crate_map.insert(addCrateButton(context, text, current_player, lg, LEGENDARY, 15, crate_menu));

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

			//background_image.draw(context, camera, true);
			crate_menu->draw(context, camera);
			player_summary->draw(context, camera);

			vec2 cursor_position = keys->getCursorPosition();
			shared_ptr<hud_element> selected_element = crate_menu->getMouseoverElement(cursor_position, true);

			if (keys->checkMouse(GLFW_MOUSE_BUTTON_LEFT, false))
			{
				if (selected_element != nullptr)
				{
					pair<rarity, int> crate_selected = crate_map.at(selected_element->getIdentifier());

					if (lg->getCrateCost(crate_selected.first, crate_selected.second) <= current_player->getBankBalance())
					{
						refreshPlayerInfo(player_summary, current_player);
						menu_return = openCrate(data_path, context, keys, current_player, lg, text, textures,
							crate_selected.first, crate_selected.second);
						finished = (menu_return != 2);
					}
				}
			}

			if (keys->checkPress(GLFW_KEY_ESCAPE, false))
			{
				menu_return = mainMenu(data_path, context, keys, current_player, text, textures);
				finished = (menu_return != 2);
			}

			context->swapBuffers();
			glfwSetTime(0.0f);
		}
	}

	return menu_return;
}