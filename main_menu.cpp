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

int menuTest(string data_path, const shared_ptr<ogl_context> &context, shared_ptr<key_handler> &keys,
	shared_ptr<player> &current_player, const shared_ptr<loot_generator> &lg,
	const shared_ptr<text_handler> &text, shared_ptr<texture_handler> &textures)
{
	cout << "opening crate" << endl;
	vec4 original_background = context->getBackgroundColor();
	context->setBackgroundColor(vec4(0.0f, 0.5f, 0.5f, 1.0f));

	int drop_count = 6;

	//TODO revise so function doesn't rely on so many containers created/copied per run
	//add copies of the artwork instances to the local vector, so position can be manipulated
	vector<shared_ptr<artwork> > crate_contents = lg->generateArtworks(drop_count, 1.0f);

	shared_ptr<dynamic_hud_array> large_container(new dynamic_hud_array("large container", context, vec2(0.0f, 0.0f), 2.0f, 2.0f,
		pair<horizontal_justification, vertical_justification>(H_CENTER, V_MIDDLE)));

	large_container->setBackgroundColor(vec4(0.0f, 0.0f, 0.0f, 0.4f));

	shared_ptr<dynamic_hud_array> horizontal_container(new dynamic_hud_array("horizontal container", context, vec2(0.0f, 0.0f), 1.0f, 0.3f,
		pair<horizontal_justification, vertical_justification>(H_CENTER, V_MIDDLE)));

	shared_ptr<dynamic_hud_array> vertical_container(new dynamic_hud_array("vertical container", context, vec2(0.0f, 0.0f), 0.3f, 1.0f,
		pair<horizontal_justification, vertical_justification>(H_CENTER, V_MIDDLE)));

	large_container->addElement(horizontal_container);
	large_container->addElement(vertical_container);

	for (int i = 0; i < crate_contents.size(); i++)
	{
		string identifier = std::to_string(i) + "_" + crate_contents.at(i)->getData()->getArtistName() + "_"
			+ crate_contents.at(i)->getData()->getTitle();

		shared_ptr<artwork_thumbnail> thumbnail(new artwork_thumbnail(identifier, crate_contents.at(i), context, vec2(0.3f, 0.3f), 0.01f));
		thumbnail->setDrawSelected(highlight, fullBrightness);

		if (i % 2 == 0)
			horizontal_container->addElement(thumbnail);
	
		else vertical_container->addElement(thumbnail);
	}

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

			large_container->draw(context, camera);

			if (keys->checkPress(GLFW_KEY_ENTER, false) && highlight != nullptr)
			{
				;
			}

			if (keys->checkMouse(GLFW_MOUSE_BUTTON_LEFT, false))
			{
				vec2 cursor_position = keys->getCursorPosition();
				hud_element_type selected_type;
				string identifier;
				shared_ptr<hud_element> selected = large_container->getSelectedWithinArray(keys, cursor_position, selected_type, identifier);

				if (selected_type == THUMBNAIL)
				{
					title_text = text->getTextArray(selected->getStoredArt()->getData()->getTitle(), context,
						true, title_color, transparent_color, "text", "text_color", "transparency_color", true, title_screen_position, title_scale, text_box_width);

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
				if (menu_return != -1)
					finished = true;
			}

			context->swapBuffers();
			glfwSetTime(0.0f);
		}
	}

	context->setBackgroundColor(original_background);
	return menu_return;
}