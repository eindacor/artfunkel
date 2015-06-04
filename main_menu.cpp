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

/*
int mainMenu(string data_path, const shared_ptr<ogl_context> &context, const shared_ptr<key_handler> &keys, const shared_ptr<text_handler> &text,
	shared_ptr<texture_handler> &textures, shared_ptr<player> &current_player)
{
	savePlayer(data_path, current_player->getName(), current_player);

	bool item_selected = false;
	float camera_distance_from_items = 10.0f;
	float menu_item_offset = 3.0f;
	float menu_item_width = 2.0f, menu_item_height = 2.0f;

	vec4 original_background = context->getBackgroundColor();
	context->setBackgroundColor(vec4(0.0f, 0.2f, 0.1f, 1.0f));

	map<int, pair<painting_surface, frame_model> > options;

	shared_ptr<ogl_camera> camera(new ogl_camera(keys, context, vec3(0.0f, 0.0f, camera_distance_from_items), vec3(0.0f, 0.0f, 0.0f), 45.0f));
	//TODO CLEAN UP THIS MESS 
	options.insert(pair<int, pair<painting_surface, frame_model> > (0, pair<painting_surface, frame_model>(
		painting_surface(menu_item_width, menu_item_height, context, (data_path + "images\\menu_art\\enter_gallery.bmp").c_str()),
		frame_model(menu_item_width, menu_item_height, context, "frame_black.bmp", "white_matte.bmp", textures)
		)));

	options.insert(pair<int, pair<painting_surface, frame_model> >(1, pair<painting_surface, frame_model>(
		painting_surface(menu_item_width, menu_item_height, context, (data_path + "images\\menu_art\\view_inventory.bmp").c_str()),
		frame_model(menu_item_width, menu_item_height, context, "frame_black.bmp", "white_matte.bmp", textures)
		)));

	options.insert(pair<int, pair<painting_surface, frame_model> >(2, pair<painting_surface, frame_model>(
		painting_surface(menu_item_width, menu_item_height, context, (data_path + "images\\menu_art\\open_crate.bmp").c_str()),
		frame_model(menu_item_width, menu_item_height, context, "frame_black.bmp", "white_matte.bmp", textures)
		)));

	options.insert(pair<int, pair<painting_surface, frame_model> >(3, pair<painting_surface, frame_model>(
		painting_surface(menu_item_width, menu_item_height, context, (data_path + "images\\menu_art\\options.bmp").c_str()),
		frame_model(menu_item_width, menu_item_height, context, "frame_black.bmp", "white_matte.bmp", textures)
		)));

	options.insert(pair<int, pair<painting_surface, frame_model> >(4, pair<painting_surface, frame_model>(
		painting_surface(menu_item_width, menu_item_height, context, (data_path + "images\\menu_art\\exit.bmp").c_str()),
		frame_model(menu_item_width, menu_item_height, context, "frame_black.bmp", "white_matte.bmp", textures)
		)));

	int current_selection = 0;
	glfwSetTime(0);
	float render_fps = 60.0f;
	while (!item_selected)
	{
		if (glfwGetTime() > 1.0f / render_fps)
		{
			glfwPollEvents();
			context->clearBuffers();

			if (keys->checkPress(GLFW_KEY_LEFT, false) && current_selection != 0)
				current_selection--;

			else if (keys->checkPress(GLFW_KEY_RIGHT, false) && current_selection != (options.size() - 1))
				current_selection++;

			for (const auto &i : options)
			{
				float x_offset = (float(current_selection) - float(i.first)) * menu_item_offset * -1.0f;
				mat4 model_matrix(glm::translate(mat4(1.0f), vec3(x_offset, 0.0f, 0.0f)));

				if (i.first == current_selection)
				{
					model_matrix = glm::scale(mat4(1.0f), vec3(1.3f, 1.3f, 1.3f));
					i.second.first.draw(context, model_matrix, camera);
					i.second.second.draw(context, model_matrix, camera);			
				}

				else
				{
					glUniform1f(context->getShaderGLint("dim_factor"), 0.5f);
					i.second.first.draw(context, model_matrix, camera);
					i.second.second.draw(context, model_matrix, camera);
					glUniform1f(context->getShaderGLint("dim_factor"), 1.0f);
				}
			}

			if (keys->checkPress(GLFW_KEY_Y, false))
				return -1;

			item_selected = keys->checkPress(GLFW_KEY_ENTER, false);

			context->swapBuffers();
			glfwSetTime(0.0f);
		}
	}

	context->setBackgroundColor(original_background);
	return current_selection;
}
*/

vector< shared_ptr<text_area> > createMenuOptions(const vector<string> &options, const shared_ptr<ogl_context> &context,
	const shared_ptr<text_handler> &text)
{
	float option_text_height(0.05f);
	vec4 option_color(0.8f, 0.8f, 0.8f, 1.0f);
	vec2 option_centerpoint(0.0f, 0.0f);
	vec2 option_element_dimensions(0.46f, 0.07f);
	pair <horizontal_justification, vertical_justification> option_just(H_LEFT, V_MIDDLE);
	bool option_italics = false;
	vec2 option_element_padding(0.025f, 0.0f);
	vec2 option_spacing_scale(0.8f, 1.0f);

	vector< shared_ptr<text_area> > option_elements;
	for (const auto &str : options)
	{
		shared_ptr<text_area> option_text(new text_area(str, str,
			context, text, option_centerpoint, option_element_dimensions, option_text_height, option_just, option_italics, option_color,
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

	shared_ptr<dynamic_hud_array> menu(new dynamic_hud_array("description", context, vec2(-.75f,0.0f), 0.5f, 2.0f,
		pair<horizontal_justification, vertical_justification>(H_LEFT, V_MIDDLE), vec2(0.02f, 0.0f)));

	string file_path = data_path + "images\\paintings\\murakami_727.bmp";
	image background_image(vec2(0.0f, 0.0f), vec2(2.0f, 2.0f), context, file_path.c_str() );

	menu->setBackgroundColor(vec4(0.0f, 0.0f, 0.0f, 0.5f));

	float title_text_height(0.1f);
	vec4 title_color(1.0f, 1.0f, 1.0f, 1.0f);
	vec2 title_centerpoint(0.0f, 0.0f);
	vec2 title_element_dimensions(0.48f, 0.4f);
	pair <horizontal_justification, vertical_justification> title_just(H_LEFT, V_MIDDLE);
	bool title_italics = true;
	vec2 title_element_padding(0.015f, 0.0f / context->getAspectRatio());
	vec2 title_spacing_scale(0.8f, 1.0f);

	shared_ptr<text_area> title_text(new text_area("title_text", "Artfunkel",
		context, text, title_centerpoint, title_element_dimensions, title_text_height, title_just, title_italics, title_color,
		"text", "text_color", title_element_padding, title_spacing_scale));

	title_text->setSelectable(false);

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

			vec2 cursor_position = keys->getCursorPosition();
			hud_element_type selected_type;
			string identifier;
			shared_ptr<hud_element> selected = menu->getSelectedWithinArray(keys, cursor_position, selected_type, identifier);

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