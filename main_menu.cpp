#include "header.h"
#include "geometry.h"
#include "loot.h"
#include "player.h"
#include "artwork.h"
#include "utility_funcs.h"
#include "menus.h"
#include "gallery.h"
#include "hud.h"
#include "fileman.h"
#include "hud_common.h"

vector< shared_ptr<text_area> > createMenuOptions(const vector<string> &options, const shared_ptr<ogl_context> &context,
	const shared_ptr<text_handler> &text)
{
	float option_text_height(0.05f);
	vec4 option_color(0.8f, 0.8f, 0.8f, 1.0f);
	vec2 option_element_dimensions(0.46f, 0.07f);
	justpair option_just(H_LEFT, V_MIDDLE);
	bool option_italics = false;
	vec2 option_element_padding(0.025f, 0.0f);
	vec2 option_spacing_scale(0.8f, 1.0f);

	vector< shared_ptr<text_area> > option_elements;
	for (const auto &str : options)
	{
		shared_ptr<text_area> option_text(new text_area(str, str,
			context, text, vec2(0.0f, 0.0f), justpair(H_CENTER, V_MIDDLE), option_element_dimensions, option_text_height, option_just, option_italics, option_color,
			"text", "text_color", option_element_padding, option_spacing_scale));
		option_text->setDrawSelected(highlightSubtle, fullBrightness);
		option_elements.push_back(option_text);
	}

	return option_elements;
}

int mainMenu(string data_path, const shared_ptr<ogl_context> &context, shared_ptr<key_handler> &keys,
	shared_ptr<player> &current_player, const shared_ptr<text_handler> &text, shared_ptr<texture_handler> &textures)
{
	savePlayer(data_path, current_player->getName(), current_player);

	shared_ptr<dynamic_hud_array> menu(new dynamic_hud_array("description", context, vec2(-1.0f, 1.0f), justpair(H_LEFT, V_TOP), vec2(0.5f, 1.75f),
		justpair(H_LEFT, V_MIDDLE), vec2(0.02f, 0.0f)));

	string file_path = data_path + "images\\paintings\\murakami_727.bmp";
	image background_image(vec2(0.0f, 0.0f), vec2(2.0f, 2.0f), context, file_path.c_str() );

	menu->setBackgroundColor(vec4(0.0f, 0.0f, 0.0f, 0.5f));

	float title_text_height(0.1f);
	vec4 title_color(1.0f, 1.0f, 1.0f, 1.0f);
	vec2 title_element_dimensions(0.48f, 0.4f);
	justpair title_just(H_LEFT, V_MIDDLE);
	bool title_italics = true;
	vec2 title_element_padding(0.015f, 0.0f / context->getAspectRatio());
	vec2 title_spacing_scale(0.8f, 1.0f);

	shared_ptr<text_area> title_text(new text_area("title_text", "Artfunkel",
		context, text, vec2(0.0f, 0.0f), justpair(H_CENTER, V_MIDDLE), title_element_dimensions, title_text_height, title_just, title_italics, title_color,
		"text", "text_color", title_element_padding, title_spacing_scale));

	title_text->setSelectable(false);

	shared_ptr<dynamic_hud_array> player_summary = generatePlayerInfo(context, text, current_player);

	vector<string> menu_items = { "edit gallery", "view inventory", "visit store", "options", "exit game" };
	vector< shared_ptr<text_area> > menu_elements = createMenuOptions(menu_items, context, text);

	menu->addElement(title_text);
	for (const auto &i : menu_elements)
		menu->addElement(i);

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

			background_image.draw(context, camera, true);
			menu->draw(context, camera);
			player_summary->draw(context, camera);

			vec2 cursor_position = keys->getCursorPosition();
			hud_element_type selected_type;
			string identifier;
			menu->handleClick(cursor_position, identifier);
			shared_ptr<hud_element> selected = menu->getElementWithinByID(identifier);
			//shared_ptr<hud_element> selected = menu->getSelectedWithinArray(keys, cursor_position, selected_type, identifier);

			if (keys->checkMouse(GLFW_MOUSE_BUTTON_LEFT, false))
			{
				if (identifier == "edit gallery")
					return 0;

				if (identifier == "view inventory")
					return 1;

				if (identifier == "visit store")
					return 2;

				//if (identifier == "options")
					//return 3;

				if (identifier == "exit game")
					return 4;
			}

			context->swapBuffers();
			glfwSetTime(0.0f);
		}
	}

	return menu_return;
}