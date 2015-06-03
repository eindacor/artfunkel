#include "header.h"
#include "player.h"
#include "artwork.h"
#include "menus.h"
#include "gallery.h"
#include "hud.h"
#include "utility_funcs.h"

int editGallery(string data_path, const shared_ptr<ogl_context> &context, shared_ptr<key_handler> keys,
	shared_ptr<player> &current_player, const shared_ptr<text_handler> &text, shared_ptr<texture_handler> &textures)
{
	float eye_level = 1.65f;
	shared_ptr<ogl_camera> camera(new ogl_camera_free(keys, context, vec3(0.0f, eye_level, 5.0f), 45.0f));

	vec4 original_background = context->getBackgroundColor();
	context->setBackgroundColor(vec4(0.5f, 0.5f, 0.5f, 1.0f));

	shared_ptr<gallery> current_gallery(nullptr);
	if (current_player->getGallery(0) == nullptr)
	{
		current_player->addGallery(shared_ptr<gallery>(new gallery(
			context, textures,
			data_path + "model_data\\",
			data_path + "model_data\\",
			"gallery_template_01",
			current_player->getName()
			)));
	}

	current_gallery = current_player->getGallery(0);

	//TODO remove inventory copy mechanic. use actual inventory container with active iterators
	//add copies of the artwork instances to the local vector, so position can be manipulated
	vector<shared_ptr<artwork> >not_displayed_copy = current_player->getNotDisplayedCopy();
	shared_ptr<dynamic_hud_array> artwork_thumbnails(new dynamic_hud_array("thumbnails", context, vec2(0.0f, -0.85f), 2.0f, 0.3f,
		pair<horizontal_justification, vertical_justification>(H_LEFT, V_MIDDLE)));

	artwork_thumbnails->setBackgroundColor(vec4(0.0f, 0.0f, 0.0f, 0.4f));

	//add player's default frames to each
	for (int i = 0; i < not_displayed_copy.size(); i++)
	{
		string identifier = std::to_string(i) + "_" + not_displayed_copy.at(i)->getData()->getArtistName() + "_"
			+ not_displayed_copy.at(i)->getData()->getTitle();
		not_displayed_copy.at(i)->applyFrameTemplate2D(context, textures, *(current_player->getDefaultFrame()));
		shared_ptr<artwork_thumbnail> thumbnail(new artwork_thumbnail(identifier, not_displayed_copy.at(i), context,
			vec2(0.3f, 0.3f), 0.01f));
		thumbnail->setDrawSelected(highlight, fullBrightness);
		artwork_thumbnails->addElement(thumbnail);
	}

	glfwSetTime(0);
	float render_fps = 60.0f;

	bool finished = false;
	int menu_return = 0;

	/////////////////////UPDATED HUD
	//identify positions for text
	shared_ptr<dynamic_hud_array> work_description(new dynamic_hud_array("description", context, vec2(0.6f, 0.9f), 0.8f, 0.25f,
		pair<horizontal_justification, vertical_justification>(H_LEFT, V_MIDDLE), vec2(0.02f, 0.0f)));

	work_description->setBackgroundColor(vec4(0.0f, 0.0f, 0.0f, 0.5f));

	float title_text_height(0.045f);
	vec4 title_color(1.0f, 1.0f, 1.0f, 1.0f);
	vec2 title_centerpoint(0.0f, 0.0f);
	vec2 title_element_dimensions(0.8f, 0.1f);
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
	vec2 rarity_element_dimensions(0.8f, 0.032f);
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
	vec2 artist_element_dimensions(0.8f, 0.032f);
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
	vec2 value_element_dimensions(0.8f, 0.032f);
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

	shared_ptr<artwork> painting_to_place(nullptr);
	pair<float, shared_ptr<artwork> > artwork_selected(0.0f, nullptr);
	pair<float, shared_ptr<display_wall> > wall_selected(0.0f, nullptr);

	bool inventory_displayed = true;

	shared_ptr<rectangle> placement_preview = nullptr;

	while (!finished)
	{
		if (glfwGetTime() > 1.0f / render_fps)
		{
			glfwPollEvents();
			context->clearBuffers();

			camera->updateCamera();

			current_gallery->renderGallery(context, camera);

			if (inventory_displayed)
				artwork_thumbnails->draw(context, camera);

			if (painting_to_place != nullptr || artwork_selected.second != nullptr)
				work_description->draw(context, camera);

			if (keys->checkPress(GLFW_KEY_I, false))
			{
				if (inventory_displayed)
				{
					artwork_thumbnails->clearElements();
					inventory_displayed = false;
					artwork_selected = pair<float, shared_ptr<artwork> >(0.0f, nullptr);
				}

				else
				{
					inventory_displayed = true;
					artwork_thumbnails->clearElements();
					for (int i = 0; i < not_displayed_copy.size(); i++)
					{
						string identifier = std::to_string(i) + "_" + not_displayed_copy.at(i)->getData()->getArtistName() + "_"
							+ not_displayed_copy.at(i)->getData()->getTitle();
						not_displayed_copy.at(i)->applyFrameTemplate2D(context, textures, *(current_player->getDefaultFrame()));
						shared_ptr<artwork_thumbnail> thumbnail(new artwork_thumbnail(identifier, not_displayed_copy.at(i), context,
							vec2(0.3f, 0.3f), 0.01f));
						thumbnail->setDrawSelected(highlight, fullBrightness);
						artwork_thumbnails->addElement(thumbnail);
					}
				}
			}

			bool new_selection = false;
			if (keys->checkMouse(GLFW_MOUSE_BUTTON_LEFT, false))
			{
				vec2 cursor_position = keys->getCursorPosition();
				hud_element_type selected_type;
				string identifier;
				shared_ptr<hud_element> selected = artwork_thumbnails->getSelectedWithinArray(keys, cursor_position, selected_type, identifier);

				if (selected_type == THUMBNAIL)
				{
					painting_to_place = selected->getStoredArt();
					vec2 painting_dimensions = vec2(painting_to_place->getOverallDimensions().x, painting_to_place->getOverallDimensions().y);
					placement_preview = shared_ptr<rectangle>(new rectangle(vec2(0.0f, 0.0f), painting_dimensions, vec4(1.0f, 1.0f, 0.0f, 0.5f)));

					new_selection = true;

					title_text->setText(painting_to_place->getData()->getTitle());
					artist_text->setText(painting_to_place->getData()->getArtistName());
					value_text->setText("$" + painting_to_place->getValue().getNumberString(true, false, 2));
					rarity_text->setText(stringFromRarity(painting_to_place->getData()->getRarity()));

					switch (painting_to_place->getData()->getRarity())
					{
					case COMMON: rarity_text->setColor(vec4(0.6f, 0.9f, 0.6f, 1.0f)); break;
					case UNCOMMON: rarity_text->setColor(vec4(0.6f, 0.6f, 0.9f, 1.0f)); break;
					case RARE: rarity_text->setColor(vec4(0.9f, 0.9f, 0.6f, 1.0f)); break;
					case LEGENDARY: rarity_text->setColor(vec4(1.0f, 0.75f, 0.6f, 1.0f)); break;
					case MASTERPIECE: rarity_text->setColor(vec4(0.6f, 0.9f, 0.9f, 1.0f)); break;
					}
				}

				if (!new_selection)
				{
					//first float indicates scale of the ray from bary coordinates (result.z)
					//the smaller the scale, the closer the object is. closest object indicates intended target
					wall_selected.second = current_gallery->getClosestWallUnderCursor(keys, camera, wall_selected.first);
					artwork_selected.second = current_gallery->getClosestArtworkUnderCursor(keys, camera, artwork_selected.first);

					//wall returned wasn't nullptr, and either no artwork was selected or the wall was closer
					bool wall_was_selected = (wall_selected.second != nullptr &&
						(artwork_selected.second == nullptr || artwork_selected.first > wall_selected.first));
					bool artwork_was_selected = artwork_selected.second != nullptr && !wall_was_selected;

					if (wall_was_selected && painting_to_place != nullptr)
					{
						vec2 point_clicked = wall_selected.second->getCursorPositionWallspace();

						if (wall_selected.second->validPlacement(painting_to_place, point_clicked))
						{
							//TODO find a way to combine these to ensure players paintings are never added without updating player
							painting_to_place->applyFrameTemplate(context, textures, *(current_player->getDefaultFrame()));
							wall_selected.second->addArtwork(point_clicked, *painting_to_place);
							current_player->addPaintingToDisplay(painting_to_place);

							not_displayed_copy = current_player->getNotDisplayedCopy();
							artwork_thumbnails->clearElements();
							for (int i = 0; i < not_displayed_copy.size(); i++)
							{
								string identifier = std::to_string(i) + "_" + not_displayed_copy.at(i)->getData()->getArtistName() + "_"
									+ not_displayed_copy.at(i)->getData()->getTitle();
								not_displayed_copy.at(i)->applyFrameTemplate2D(context, textures, *(current_player->getDefaultFrame()));
								shared_ptr<artwork_thumbnail> thumbnail(new artwork_thumbnail(identifier, not_displayed_copy.at(i), context,
									vec2(0.3f, 0.3f), 0.01f));
								thumbnail->setDrawSelected(highlight, fullBrightness);
								artwork_thumbnails->addElement(thumbnail);
							}
						}

						else cout << "position not valid" << endl;

						painting_to_place = nullptr;
						placement_preview = nullptr;
					}

					if (artwork_was_selected)
					{
						shared_ptr<artwork> selected = artwork_selected.second;
						title_text->setText(selected->getData()->getTitle());
						artist_text->setText(selected->getData()->getArtistName());
						value_text->setText("$" + selected->getValue().getNumberString(true, false, 2));
						rarity_text->setText(stringFromRarity(selected->getData()->getRarity()));

						switch (selected->getData()->getRarity())
						{
						case COMMON: rarity_text->setColor(vec4(0.6f, 0.9f, 0.6f, 1.0f)); break;
						case UNCOMMON: rarity_text->setColor(vec4(0.6f, 0.6f, 0.9f, 1.0f)); break;
						case RARE: rarity_text->setColor(vec4(0.9f, 0.9f, 0.6f, 1.0f)); break;
						case LEGENDARY: rarity_text->setColor(vec4(1.0f, 0.75f, 0.6f, 1.0f)); break;
						case MASTERPIECE: rarity_text->setColor(vec4(0.6f, 0.9f, 0.9f, 1.0f)); break;
						}

						//remove from display
						painting_to_place = nullptr;
						placement_preview = nullptr;
					}
				}
			}

			if (painting_to_place != nullptr)
			{
				float distance;
				shared_ptr<display_wall> wall_hovered = current_gallery->getClosestWallUnderCursor(keys, camera, distance);
				if (wall_hovered != nullptr)
				{
					vec2 position_on_wall = wall_hovered->getCursorPositionWallspace();
					mat4 wall_position_matrix(glm::translate(mat4(1.0f), vec3(position_on_wall.x, position_on_wall.y, 0.0f)));
					mat4 rectangle_matrix = wall_hovered->getWallModelMatrix() * wall_position_matrix;

					if (wall_hovered->validPlacement(painting_to_place, position_on_wall))
						placement_preview->setColor(vec4(0.0f, 1.0f, 0.0f, 0.5f));

					else placement_preview->setColor(vec4(1.0f, 0.0f, 0.0f, 0.5f));

					glDisable(GL_DEPTH_TEST);
					placement_preview->draw(context, camera, rectangle_matrix, false);
					glEnable(GL_DEPTH_TEST);
				}
			}

			if ((keys->checkPress(GLFW_KEY_BACKSPACE, false) || keys->checkPress(GLFW_KEY_DELETE, false)) && artwork_selected.second != nullptr)
			{
				current_player->removePaintingFromDisplay(artwork_selected.second);
				current_player->getGallery(0)->removeArtwork(artwork_selected.second);
				artwork_selected = pair<float, shared_ptr<artwork> >(0.0f, nullptr);

				not_displayed_copy = current_player->getNotDisplayedCopy();
				artwork_thumbnails->clearElements();
				for (int i = 0; i < not_displayed_copy.size(); i++)
				{
					string identifier = std::to_string(i) + "_" + not_displayed_copy.at(i)->getData()->getArtistName() + "_"
						+ not_displayed_copy.at(i)->getData()->getTitle();
					not_displayed_copy.at(i)->applyFrameTemplate2D(context, textures, *(current_player->getDefaultFrame()));
					shared_ptr<artwork_thumbnail> thumbnail(new artwork_thumbnail(identifier, not_displayed_copy.at(i), context,
						vec2(0.3f, 0.3f), 0.01f));
					thumbnail->setDrawSelected(highlight, fullBrightness);
					artwork_thumbnails->addElement(thumbnail);
				}
			}

			if (keys->checkPress(GLFW_KEY_COMMA, false) && inventory_displayed)
			{
				artwork_thumbnails->pageDown();
				painting_to_place = nullptr;
				placement_preview = nullptr;
			}

			if (keys->checkPress(GLFW_KEY_PERIOD, false) && inventory_displayed)
			{
				artwork_thumbnails->pageUp();
				painting_to_place = nullptr;
				placement_preview = nullptr;
			}

			if (keys->checkMouse(GLFW_MOUSE_BUTTON_RIGHT, false))
			{
				painting_to_place = nullptr;
				placement_preview = nullptr;
				artwork_selected.second = nullptr;
				artwork_thumbnails->deselectAllWithin();
			}

			//TODO see why this only works when include_hold is enabled
			if (keys->checkPress(GLFW_KEY_ESCAPE))
			{
				menu_return = mainMenu(data_path, context, keys, current_player, text, textures);
				if (menu_return != 0)
					finished = true;
			}

			context->swapBuffers();
			glfwSetTime(0.0f);
		}
	}

	context->setBackgroundColor(original_background);
	return menu_return;
}