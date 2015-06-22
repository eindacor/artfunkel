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

	cout << "-----Gallery Attributes-----" << endl;
	current_gallery->printAttributeTotals();

	//TODO remove inventory copy mechanic. use actual inventory container with active iterators
	//add copies of the artwork instances to the local vector, so position can be manipulated
	vector<shared_ptr<artwork> >not_displayed_copy = current_player->getNotDisplayedCopy();
	shared_ptr<dynamic_hud_array> artwork_thumbnails(new dynamic_hud_array("thumbnails", context, vec2(1.0f, -1.0f), justpair(H_RIGHT, V_BOTTOM), vec2(1.5f, 0.25f),
		pair<horizontal_justification, vertical_justification>(H_LEFT, V_MIDDLE)));
	artwork_thumbnails->setBackgroundColor(vec4(0.0f, 0.0f, 0.0f, 0.4f));
	vec2 thumbsize(0.2f, 0.25f);
	float thumbpadding(0.01f);
	refreshThumbnails(context, textures, current_player, not_displayed_copy, artwork_thumbnails, thumbsize, thumbpadding);

	shared_ptr<dynamic_hud_array> finish_thumbnails(new dynamic_hud_array("thumbnails", context, vec2(1.0f, -1.0f), justpair(H_RIGHT, V_BOTTOM), vec2(1.5f, 0.25f),
		pair<horizontal_justification, vertical_justification>(H_LEFT, V_MIDDLE)));
	finish_thumbnails->setBackgroundColor(vec4(0.0f, 0.0f, 0.0f, 0.4f));
	finish_thumbnails->setVisibility(false);

	map<string, string> finish_map = current_player->getAvailableWallFinishes();
	for (const auto &finish_data : finish_map)
	{
		string identifier = finish_data.first + "_thumb";
		string texture_filename = finish_data.second;
		string finish_name = finish_data.first;
		shared_ptr<finish_thumbnail> finish_thumb(new finish_thumbnail(identifier, texture_filename, finish_name,
			textures->getTexture(texture_filename), context, vec2(0.2f / context->getAspectRatio(), 0.2f), vec2(thumbpadding)));

		finish_thumbnails->addElement(finish_thumb);
	}

	shared_ptr<dynamic_hud_array> work_info(new dynamic_hud_array("description", context, vec2(1.0f, 1.0f), justpair(H_RIGHT, V_TOP), vec2(0.8f, 0.25f),
		pair<horizontal_justification, vertical_justification>(H_LEFT, V_MIDDLE), vec2(0.02f, 0.0f)));
	work_info->setBackgroundColor(vec4(0.0f, 0.0f, 0.0f, 0.5f));
	setWorkInfoFields(context, text, work_info);

	shared_ptr<dynamic_hud_array> finish_info(new dynamic_hud_array("finish_info", context, vec2(1.0f, 1.0f), justpair(H_RIGHT, V_TOP), vec2(0.8f, 0.25f),
		pair<horizontal_justification, vertical_justification>(H_LEFT, V_MIDDLE), vec2(0.02f, 0.0f)));
	finish_info->setBackgroundColor(vec4(0.0f, 0.0f, 0.0f, 0.5f));

	float finish_text_height(0.045f);
	vec4 finish_color(1.0f, 1.0f, 1.0f, 1.0f);
	vec2 finish_element_dimensions(finish_info->getAllowableWidth(), 0.1f);
	pair <horizontal_justification, vertical_justification> finish_just(H_LEFT, V_MIDDLE);
	bool finish_italics = true;
	vec2 finish_element_padding(0.015f, 0.0f);
	vec2 finish_spacing_scale(0.8f, 1.1f);

	shared_ptr<text_area> finish_text(new text_area("title_text", "not yet set",
		context, text, finish_element_dimensions, finish_text_height, finish_just, finish_italics, finish_color,
		"text", "text_color", finish_element_padding, finish_spacing_scale));

	finish_info->addElement(finish_text);
	finish_info->setVisibility(false);

	shared_ptr<dynamic_hud_array> player_summary = generatePlayerInfo(context, text, current_player);

	shared_ptr<artwork> painting_to_place(nullptr);
	shared_ptr<finish_thumbnail> selected_finish = nullptr;
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

	//test
	bool brick = false;

	clock_t start = clock();
	while (!finished)
	{
		if (glfwGetTime() > 1.0f / render_fps)
		{
			if ((clock() - start) / CLOCKS_PER_SEC > 2.0f)
			{
				current_player->updateBank();
				refreshPlayerInfo(player_summary, current_player);
				start = clock();
			}

			glfwPollEvents();
			context->clearBuffers();

			camera->updateCamera();

			current_gallery->renderGallery(context, camera);

			artwork_thumbnails->draw(context, camera);
			finish_thumbnails->draw(context, camera);

			finish_info->setVisibility(selected_finish != nullptr);
			finish_info->draw(context, camera);

			if (painting_to_place != nullptr || artwork_selected.second != nullptr)
				work_info->draw(context, camera);

			player_summary->draw(context, camera);

			if (keys->checkPress(GLFW_KEY_I, false))
			{
				artwork_thumbnails->setVisibility(!artwork_thumbnails->isVisible());
				finish_thumbnails->setVisibility(!finish_thumbnails->isVisible());

				painting_to_place = nullptr;
				selected_finish = nullptr;
			}

			if (keys->checkMouse(GLFW_MOUSE_BUTTON_LEFT, false))
			{
				vec2 cursor_position = keys->getCursorPosition();
				shared_ptr<hud_element> selected_element = nullptr;
				
				if (artwork_thumbnails->isVisible())
				{
					selected_element = artwork_thumbnails->getMouseoverElement(cursor_position, true);

					if (selected_element != nullptr && selected_element->getType() == THUMBNAIL)
					{
						shared_ptr<artwork_thumbnail> thumbnail_selected = boost::dynamic_pointer_cast<artwork_thumbnail>(selected_element);

						painting_to_place = thumbnail_selected->getStoredArt();
						vec2 painting_dimensions = vec2(painting_to_place->getOverallDimensions().x, painting_to_place->getOverallDimensions().y);
						placement_preview = shared_ptr<rectangle>(new rectangle(vec2(0.0f, 0.0f), painting_dimensions, vec4(1.0f, 1.0f, 0.0f, 0.5f)));
						setWorkInfoDescription(work_info, painting_to_place);
					}
				}

				else if (finish_thumbnails->isVisible())
				{
					selected_element = finish_thumbnails->getMouseoverElement(cursor_position, true);

					if (selected_element != nullptr && selected_element->getType() == FINISH_THUMBNAIL)
					{
						selected_finish = boost::dynamic_pointer_cast<finish_thumbnail>(selected_element);
						finish_text->setText(selected_finish->getFinishName());
					}
				}

				//new_selection being false means a click was made that is not in either of the inventory options, no new thumbnail was selected
				if (selected_element == nullptr)
				{
					//first float indicates scale of the ray from bary coordinates (result.z)
					//the smaller the scale, the closer the object is. closest object indicates intended target
					wall_selected.second = current_gallery->getClosestWallUnderCursor(keys, camera, wall_selected.first);
					artwork_selected.second = current_gallery->getClosestArtworkUnderCursor(keys, camera, artwork_selected.first);

					//wall returned wasn't nullptr, and either no artwork was selected or the wall was closer
					bool wall_was_selected = (wall_selected.second != nullptr &&
						(artwork_selected.second == nullptr || artwork_selected.first > wall_selected.first));
					bool artwork_was_selected = artwork_selected.second != nullptr && !wall_was_selected;

					if (wall_was_selected)
					{
						if (painting_to_place != nullptr)
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

							painting_to_place = nullptr;
							placement_preview = nullptr;
						}

						if (selected_finish != nullptr)
							current_gallery->getWall(wall_selected.second->getIndex())->setTexture(selected_finish->getTextureFilename(), textures);
					}			

					else if (artwork_was_selected)
					{
						shared_ptr<artwork> selected = artwork_selected.second;
						setWorkInfoDescription(work_info, selected);

						//remove from display
						selected_finish = nullptr;
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

			if (keys->checkPress(GLFW_KEY_COMMA, false))
			{
				if (artwork_thumbnails->isVisible())
					artwork_thumbnails->pageDown();
				if (finish_thumbnails->isVisible())
					finish_thumbnails->pageDown();
				painting_to_place = nullptr;
				placement_preview = nullptr;
			}

			if (keys->checkPress(GLFW_KEY_PERIOD, false))
			{
				if (artwork_thumbnails->isVisible())
					artwork_thumbnails->pageUp();
				if (finish_thumbnails->isVisible())
					finish_thumbnails->pageUp();
				painting_to_place = nullptr;
				placement_preview = nullptr;
			}

			if (keys->checkMouse(GLFW_MOUSE_BUTTON_RIGHT, false))
			{
				painting_to_place = nullptr;
				placement_preview = nullptr;
				selected_finish = nullptr;
				artwork_selected.second = nullptr;
				artwork_thumbnails->deselectAllWithin();
			}

			//TODO see why this only works when include_hold is enabled
			if (keys->checkPress(GLFW_KEY_ESCAPE))
			{
				menu_return = mainMenu(data_path, context, keys, current_player, text, textures);
				if (menu_return != 0)
					finished = true;
				refreshPlayerInfo(player_summary, current_player);
			}

			context->swapBuffers();
			glfwSetTime(0.0f);
		}
	}

	current_player->updateBank();

	context->setBackgroundColor(original_background);
	return menu_return;
}