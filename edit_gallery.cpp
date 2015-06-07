#include "header.h"
#include "player.h"
#include "artwork.h"
#include "menus.h"
#include "gallery.h"
#include "hud.h"
#include "utility_funcs.h"
#include "hud_common.h"

int editGallerySelect()
{
	return 0;
}

int editGallery(string data_path, const shared_ptr<ogl_context> &context, shared_ptr<key_handler> keys,
	shared_ptr<player> &current_player, const shared_ptr<loot_generator> &lg, const shared_ptr<text_handler> &text, 
	shared_ptr<texture_handler> &textures)
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
			current_player->getName(),
			"created gallery"
			)));
	}

	current_gallery = current_player->getGallery(0);

	//TODO remove inventory copy mechanic. use actual inventory container with active iterators
	//add copies of the artwork instances to the local vector, so position can be manipulated
	vector<shared_ptr<artwork> >not_displayed_copy = current_player->getNotDisplayedCopy();
	shared_ptr<dynamic_hud_array> artwork_thumbnails(new dynamic_hud_array("thumbnails", context, vec2(1.0f, -1.0f), justpair(H_RIGHT, V_BOTTOM), vec2(1.5f, 0.25f),
		pair<horizontal_justification, vertical_justification>(H_LEFT, V_MIDDLE)));
	artwork_thumbnails->setBackgroundColor(vec4(0.0f, 0.0f, 0.0f, 0.4f));
	vec2 thumbsize(0.2f, 0.25f);
	float thumbpadding(0.01f);
	refreshThumbnails(context, textures, current_player, not_displayed_copy, artwork_thumbnails, thumbsize, thumbpadding);

	/////////////////////UPDATED HUD
	//identify positions for text
	shared_ptr<dynamic_hud_array> work_info(new dynamic_hud_array("description", context, vec2(1.0f, 1.0f), justpair(H_RIGHT, V_TOP), vec2(0.8f, 0.25f),
		pair<horizontal_justification, vertical_justification>(H_LEFT, V_MIDDLE), vec2(0.02f, 0.0f)));
	work_info->setBackgroundColor(vec4(0.0f, 0.0f, 0.0f, 0.5f));
	setWorkInfoFields(context, text, work_info);

	shared_ptr<dynamic_hud_array> player_summary = generatePlayerInfo(context, text, current_player);
	////////////////////////////////

	shared_ptr<artwork> painting_to_place(nullptr);
	pair<float, shared_ptr<artwork> > artwork_selected(0.0f, nullptr);
	pair<float, shared_ptr<display_wall> > wall_selected(0.0f, nullptr);

	bool inventory_displayed = true;

	shared_ptr<rectangle> placement_preview = nullptr;

	glfwSetTime(0);
	float render_fps = 60.0f;
	bool finished = false;
	int menu_return = 0;

	//TODO put this in player class, determined by level/rep
	bignum gallery_value_per_sec(".0001");

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
				work_info->draw(context, camera);

			player_summary->draw(context, camera);

			if (keys->checkPress(GLFW_KEY_I, false))
			{
				inventory_displayed = !inventory_displayed;

				//TEXT BELOW UNLOADS INVENTORY WHEN IT IS HIDDEN
				/*
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
							vec2(0.3f, 0.25f), 0.01f));
						thumbnail->setDrawSelected(highlight, fullBrightness);
						artwork_thumbnails->addElement(thumbnail);
					}
				}
				*/
			}

			bool new_selection = false;
			if (keys->checkMouse(GLFW_MOUSE_BUTTON_LEFT, false))
			{
				vec2 cursor_position = keys->getCursorPosition();
				//hud_element_type selected_type;
				//string identifier;
				//artwork_thumbnails->handleClick(cursor_position, identifier);
				shared_ptr<hud_element> selected_element = artwork_thumbnails->getMouseoverElement(cursor_position, true);
				//shared_ptr<hud_element> selected = artwork_thumbnails->getSelectedWithinArray(keys, cursor_position, selected_type, identifier);

				if (selected_element != nullptr && selected_element->getType() == THUMBNAIL)
				{
					painting_to_place = selected_element->getStoredArt();
					vec2 painting_dimensions = vec2(painting_to_place->getOverallDimensions().x, painting_to_place->getOverallDimensions().y);
					placement_preview = shared_ptr<rectangle>(new rectangle(vec2(0.0f, 0.0f), painting_dimensions, vec4(1.0f, 1.0f, 0.0f, 0.5f)));

					new_selection = true;
					setWorkInfoDescription(work_info, painting_to_place);
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
							current_player->updateBank();

							//TODO find a way to combine these to ensure players paintings are never added without updating player
							painting_to_place->applyFrameTemplate(context, textures, *(current_player->getDefaultFrame()));
							current_gallery->addArtwork(wall_selected.second->getIndex(), painting_to_place, point_clicked);
							//wall_selected.second->addArtwork(point_clicked, *painting_to_place);
							current_player->addPaintingToDisplay(painting_to_place);

							unsigned work_id = painting_to_place->getData()->getID();

							if (!current_player->getPaintingFromInventory(work_id)->getProfited())
							{
								//current_player->addFunds(lg->calcPlacementBonus(painting_to_place->getValue()));
								current_player->getPaintingFromInventory(work_id)->setProfitedTEMP(true);
							}

							not_displayed_copy = current_player->getNotDisplayedCopy();
							refreshThumbnails(context, textures, current_player, not_displayed_copy, artwork_thumbnails, thumbsize, thumbpadding);
						}

						else cout << "position not valid" << endl;

						painting_to_place = nullptr;
						placement_preview = nullptr;
					}

					if (artwork_was_selected)
					{
						shared_ptr<artwork> selected = artwork_selected.second;
						setWorkInfoDescription(work_info, selected);

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
				current_player->updateBank();
				
				current_player->removePaintingFromDisplay(artwork_selected.second);
				current_player->getGallery(0)->removeArtwork(artwork_selected.second);
				artwork_selected = pair<float, shared_ptr<artwork> >(0.0f, nullptr);

				not_displayed_copy = current_player->getNotDisplayedCopy();
				refreshThumbnails(context, textures, current_player, not_displayed_copy, artwork_thumbnails, thumbsize, thumbpadding);
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

			if (keys->checkPress(GLFW_KEY_T, false))
				current_player->updateBank();

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

	current_player->updateBank();

	context->setBackgroundColor(original_background);
	return menu_return;
}