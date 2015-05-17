#include "header.h"
#include "player.h"
#include "artwork.h"
#include "menus.h"
#include "gallery.h"
#include "hud.h"
#include "utility_funcs.h"


int viewGallery(string data_path, const shared_ptr<ogl_context> &context, shared_ptr<key_handler> keys,
	const shared_ptr<player> &current_player, const shared_ptr<text_handler> &text)
{
	float eye_level = 1.65f;
	shared_ptr<ogl_camera> camera(new ogl_camera_free(keys, context, vec3(0.0f, eye_level, 5.0f), 45.0f));

	vec4 original_background = context->getBackgroundColor();
	context->setBackgroundColor(vec4(0.5f, 0.5f, 0.5f, 1.0f));

	shared_ptr<gallery> current_gallery;
	if (current_player->getGallery(0) == nullptr)
	{
		current_player->addGallery(shared_ptr<gallery>(new gallery(
			context, data_path + "model_data\\",
			data_path + "model_data\\",
			"test_gallery_02.obj",
			"test_filler.obj",
			"test_gallery_02.mtl",
			"test_filler.mtl")));
	}

	current_gallery = current_player->getGallery(0);

	vector<shared_ptr<artwork> > not_displayed_copy = current_player->getNotDisplayedCopy();
	vector<shared_ptr<artwork> > paintings_to_display;
	vector< shared_ptr<artwork_thumbnail> > thumbnails;
	vector<vec2> thumbnail_locations;
	int display_count = 10;
	vector< shared_ptr<artwork> >::const_iterator chunk_first;
	vector< shared_ptr<artwork> >::const_iterator chunk_last;
	vector< shared_ptr<artwork> >::const_iterator chunk_end;

	vec2 thumbnail_center(-0.75f, -0.75f);
	float thumbnail_height(0.3f);
	float thumbnail_width(0.3f);
	float thumbnail_padding(0.0f);
	float thumbnail_bottom_margin(0.02f);

	if (not_displayed_copy.size() > 0)
	{
		//add player's default frames to each
		for (auto i : not_displayed_copy)
			i->applyFrameTemplate(context, *(current_player->getDefaultFrame()));

		//take a chunk of the inventory paintings, using first, last, end iterators. update this vectore to go through pages of inventory
		chunk_first = not_displayed_copy.begin();
		chunk_last = findChunkLast(chunk_first, not_displayed_copy, display_count);
		chunk_end = findChunkEnd(chunk_first, not_displayed_copy, display_count);
		paintings_to_display.reserve(display_count);
		paintings_to_display.insert(paintings_to_display.begin(), chunk_first, chunk_end);

		thumbnail_locations = getThumbnailLocations(context, paintings_to_display.size(), thumbnail_bottom_margin, thumbnail_height);

		//thumbnail_matrix_index indices must match painting size, not very robust
		for (int i = 0; i < paintings_to_display.size(); i++)
		{
			vec2 centerpoint = thumbnail_locations.at(i);
			shared_ptr<artwork_thumbnail> thumbnail(new artwork_thumbnail(
				paintings_to_display.at(i),
				context,
				centerpoint,
				vec2(thumbnail_height, thumbnail_width),
				thumbnail_padding));

			thumbnails.push_back(thumbnail);
		}
	}

	glfwSetTime(0);
	float render_fps = 60.0f;

	bool finished = false;
	int menu_return = 0;

	//TODO replace all text code with HUD elements
	//identify positions for text
	vec4 title_color(1.0f, 1.0f, 1.0f, 1.0f);
	vec4 rarity_color;
	vec4 info_color(0.3f, 0.3f, 0.3f, 1.0f);
	vec4 transparent_color(0.0f, 1.0f, 0.0f, 1.0f);
	float title_scale(0.045f);
	float info_scale(0.035f);
	float highlight_buffer(0.1f);
	vec2 title_screen_position(0.5f, 0.75f);
	vec2 rarity_screen_position(title_screen_position.x, title_screen_position.y - title_scale);
	vec2 info_screen_position(title_screen_position.x, rarity_screen_position.y - info_scale);
	float text_box_width(0.45f);

	shared_ptr<static_text> title_text(nullptr);
	shared_ptr<static_text> rarity_text(nullptr);
	shared_ptr<static_text> info_text(nullptr);
	shared_ptr<artwork> painting_to_place(nullptr);
	pair<float, shared_ptr<artwork> > artwork_selected(0.0f, nullptr);
	pair<float, shared_ptr<display_wall> > wall_selected(0.0f, nullptr);

	while (!finished)
	{
		if (glfwGetTime() > 1.0f / render_fps)
		{
			glfwPollEvents();
			context->clearBuffers();

			camera->updateCamera();

			current_gallery->renderGallery(context, camera);

			if (painting_to_place != nullptr)
				glUniform1f(context->getShaderGLint("dim_factor"), 0.5f);

			for (auto i : thumbnails)
			{
				if (painting_to_place != nullptr && *(i->getStoredArt()) == *painting_to_place)
				{
					glUniform1f(context->getShaderGLint("dim_factor"), 1.0f);
					i->draw(context, camera);
					glUniform1f(context->getShaderGLint("dim_factor"), 0.5f);
				}

				else i->draw(context, camera);	
			}
			glUniform1f(context->getShaderGLint("dim_factor"), 1.0f);

			if (title_text != nullptr)
				title_text->draw(camera, context, "text", "text_color", "transparency_color");
			if (info_text != nullptr)
				info_text->draw(camera, context, "text", "text_color", "transparency_color");
			if (rarity_text != nullptr)
				rarity_text->draw(camera, context, "text", "text_color", "transparency_color");

			if (keys->checkPress(GLFW_KEY_ESCAPE))
			{
				menu_return = mainMenu(data_path, context, keys, text);
				if (menu_return != 0)
					finished = true;
			}

			bool new_selection = false;
			if (keys->checkMouse(GLFW_MOUSE_BUTTON_LEFT, false))
			{				
				for (auto i : thumbnails)
				{
					vec2 cursor_position = keys->getCursorPosition();
					if (i->isSelected(keys, cursor_position, painting_to_place))
					{
						new_selection = true;
						break;
					}
				}

				if (!new_selection)
				{
					//first float indicates scale of the ray from bary coordinates (result.z)
					//the smaller the scale, the closer the object is. closest object indicates intended target
					//pair<float, shared_ptr<artwork> > artwork_selected;
					//pair<float, shared_ptr<display_wall> > wall_selected;

					wall_selected.second = current_gallery->getClosestWallUnderCursor(keys, camera, wall_selected.first);
					artwork_selected.second = current_gallery->getClosestArtworkUnderCursor(keys, camera, artwork_selected.first);

					//wall returned wasn't nullptr, and either no artwork was selected or the wall was closer
					bool wall_was_selected = (wall_selected.second != nullptr &&
						(artwork_selected.second == nullptr || artwork_selected.first > wall_selected.first));
					bool artwork_was_selected = artwork_selected.second != nullptr && !wall_was_selected;

					if (wall_was_selected && painting_to_place != nullptr)
					{
						vec2 point_clicked = wall_selected.second->getCursorPositionWallspace();

						//TODO validPlacement not working
						if (wall_selected.second->validPlacement(painting_to_place, point_clicked) || true)
						{ 
							//TODO find a way to combine these to ensure players paintings are never added without updating player
							wall_selected.second->addArtwork(point_clicked, *painting_to_place);
							current_player->addPaintingToDisplay(painting_to_place);

							not_displayed_copy.clear();
							not_displayed_copy = current_player->getNotDisplayedCopy();
							paintings_to_display.clear();
							thumbnails.clear();

							if (not_displayed_copy.size() > 0)
							{
								//add player's default frames to each
								for (auto i : not_displayed_copy)
									i->applyFrameTemplate(context, *(current_player->getDefaultFrame()));

								//take a chunk of the inventory paintings, using first, last, end iterators. update this vectore to go through pages of inventory
								chunk_first = not_displayed_copy.begin();
								chunk_last = findChunkLast(chunk_first, not_displayed_copy, display_count);
								chunk_end = findChunkEnd(chunk_first, not_displayed_copy, display_count);

								paintings_to_display.reserve(display_count);
								paintings_to_display.insert(paintings_to_display.begin(), chunk_first, chunk_end);

								thumbnail_locations = getThumbnailLocations(context, paintings_to_display.size(), thumbnail_bottom_margin, thumbnail_height);

								for (int i = 0; i < paintings_to_display.size(); i++)
								{
									vec2 centerpoint = thumbnail_locations.at(i);
									shared_ptr<artwork_thumbnail> thumbnail(new artwork_thumbnail(
										paintings_to_display.at(i),
										context,
										centerpoint,
										vec2(thumbnail_height, thumbnail_width),
										thumbnail_padding));

									thumbnails.push_back(thumbnail);
								}
							}
						}

						else cout << "position not valid" << endl;

						painting_to_place = nullptr;
					}

					if (artwork_was_selected)
					{
						shared_ptr<artwork> selected = artwork_selected.second;
						title_text = text->getTextArray(selected->getData()->getTitle(), context,
							true, title_color, transparent_color, true, title_screen_position, title_scale, text_box_width);

						switch (selected->getData()->getRarity())
						{
						case COMMON: rarity_color = vec4(0.6f, 0.9f, 0.6f, 1.0f); break;
						case UNCOMMON: rarity_color = vec4(0.6f, 0.6f, 0.9f, 1.0f); break;
						case RARE: rarity_color = vec4(0.9f, 0.9f, 0.6f, 1.0f); break;
						case LEGENDARY: rarity_color = vec4(1.0f, 0.75f, 0.6f, 1.0f); break;
						case MASTERPIECE: rarity_color = vec4(0.6f, 0.9f, 0.9f, 1.0f); break;
						}

						rarity_text = text->getTextArray(stringFromRarity(selected->getData()->getRarity()), context,
							false, rarity_color, transparent_color, true, title_text->getLowerLeft(), info_scale, text_box_width);

						string to_print = std::to_string(selected->getData()->getDate().getYear()) + "\n" + selected->getData()->getArtistName();
						to_print += "\n$" + selected->getValue().getNumberString(true, false, 2);

						info_text = text->getTextArray(to_print, context, false, info_color, transparent_color,
							true, rarity_text->getLowerLeft(), info_scale, text_box_width);

						//remove from display
						painting_to_place = nullptr;
					}

					else
					{
						title_text = nullptr;
						info_text = nullptr;
						rarity_text = nullptr;
					}
				}
			}

			if ((keys->checkPress(GLFW_KEY_BACKSPACE, false) || keys->checkPress(GLFW_KEY_BACKSPACE, false)) && artwork_selected.second != nullptr)
			{
				current_player->removePaintingFromDisplay(artwork_selected.second);
				current_player->getGallery(0)->removeArtwork(artwork_selected.second);
				artwork_selected = pair<float, shared_ptr<artwork> >(0.0f, nullptr);

				not_displayed_copy.clear();
				not_displayed_copy = current_player->getNotDisplayedCopy();
				paintings_to_display.clear();
				thumbnails.clear();

				if (not_displayed_copy.size() > 0)
				{
					//add player's default frames to each
					for (auto i : not_displayed_copy)
						i->applyFrameTemplate(context, *(current_player->getDefaultFrame()));

					//take a chunk of the inventory paintings, using first, last, end iterators. update this vectore to go through pages of inventory
					chunk_first = not_displayed_copy.begin();
					chunk_last = findChunkLast(chunk_first, not_displayed_copy, display_count);
					chunk_end = findChunkEnd(chunk_first, not_displayed_copy, display_count);

					paintings_to_display.reserve(display_count);
					paintings_to_display.insert(paintings_to_display.begin(), chunk_first, chunk_end);

					thumbnail_locations = getThumbnailLocations(context, paintings_to_display.size(), thumbnail_bottom_margin, thumbnail_height);

					for (int i = 0; i < paintings_to_display.size(); i++)
					{
						vec2 centerpoint = thumbnail_locations.at(i);
						shared_ptr<artwork_thumbnail> thumbnail(new artwork_thumbnail(
							paintings_to_display.at(i),
							context,
							centerpoint,
							vec2(thumbnail_height, thumbnail_width),
							thumbnail_padding));

						thumbnails.push_back(thumbnail);
					}
				}
			}

			context->swapBuffers();
			glfwSetTime(0.0f);
		}
	}

	for (auto i : paintings_to_display)
		i->getData()->unloadData();

	context->setBackgroundColor(original_background);
	return menu_return;
}

int viewGallery_HUD(string data_path, const shared_ptr<ogl_context> &context, shared_ptr<key_handler> keys,
	const shared_ptr<player> &current_player, const shared_ptr<text_handler> &text)
{
	float eye_level = 1.65f;
	shared_ptr<ogl_camera> camera(new ogl_camera_free(keys, context, vec3(0.0f, eye_level, 5.0f), 45.0f));

	vec4 original_background = context->getBackgroundColor();
	context->setBackgroundColor(vec4(0.5f, 0.5f, 0.5f, 1.0f));

	shared_ptr<gallery> current_gallery;
	if (current_player->getGallery(0) == nullptr)
	{
		current_player->addGallery(shared_ptr<gallery>(new gallery(
			context, data_path + "model_data\\",
			data_path + "model_data\\",
			"test_gallery_02.obj",
			"test_filler.obj",
			"test_gallery_02.mtl",
			"test_filler.mtl")));
	}

	current_gallery = current_player->getGallery(0);

	//TODO remove inventory copy mechanic. use actual inventory container with active iterators
	//add copies of the artwork instances to the local vector, so position can be manipulated
	vector<shared_ptr<artwork> >not_displayed_copy = current_player->getNotDisplayedCopy();
	shared_ptr<dynamic_hud_array> artwork_thumbnails(new dynamic_hud_array(context, vec2(0.0f, -0.85f), 1.7f, 0.3f,
		pair<horizontal_justification, vertical_justification>(H_LEFT, V_MIDDLE)));

	artwork_thumbnails->setBackgroundColor(vec4(0.0f, 0.0f, 0.0f, 0.4f));

	//add player's default frames to each
	for (auto i : not_displayed_copy)
	{
		i->applyFrameTemplate(context, *(current_player->getDefaultFrame()));
		shared_ptr<artwork_thumbnail> thumbnail(new artwork_thumbnail(i, context, 0.3f, 0.01f));
		thumbnail->setDrawSelected(highlight, fullBrightness);
		artwork_thumbnails->addElement(thumbnail);
	}

	glfwSetTime(0);
	float render_fps = 60.0f;

	bool finished = false;
	int menu_return = 0;

	//TODO replace all text code with HUD elements
	//identify positions for text
	vec4 title_color(1.0f, 1.0f, 1.0f, 1.0f);
	vec4 rarity_color;
	vec4 info_color(0.3f, 0.3f, 0.3f, 1.0f);
	vec4 transparent_color(0.0f, 1.0f, 0.0f, 1.0f);
	float title_scale(0.045f);
	float info_scale(0.035f);
	float highlight_buffer(0.1f);
	vec2 title_screen_position(0.5f, 0.75f);
	vec2 rarity_screen_position(title_screen_position.x, title_screen_position.y - title_scale);
	vec2 info_screen_position(title_screen_position.x, rarity_screen_position.y - info_scale);
	float text_box_width(0.45f);

	shared_ptr<static_text> title_text(nullptr);
	shared_ptr<static_text> rarity_text(nullptr);
	shared_ptr<static_text> info_text(nullptr);
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

			if (title_text != nullptr)
				title_text->draw(camera, context, "text", "text_color", "transparency_color");
			if (info_text != nullptr)
				info_text->draw(camera, context, "text", "text_color", "transparency_color");
			if (rarity_text != nullptr)
				rarity_text->draw(camera, context, "text", "text_color", "transparency_color");

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
					for (auto i : not_displayed_copy)
					{
						shared_ptr<artwork_thumbnail> thumbnail(new artwork_thumbnail(i, context, 0.3f, 0.01f));
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
					printArtwork(selected->getStoredArt());

					title_text = text->getTextArray(painting_to_place->getData()->getTitle(), context,
						true, title_color, transparent_color, true, title_screen_position, title_scale, text_box_width);

					switch (painting_to_place->getData()->getRarity())
					{
					case COMMON: rarity_color = vec4(0.6f, 0.9f, 0.6f, 1.0f); break;
					case UNCOMMON: rarity_color = vec4(0.6f, 0.6f, 0.9f, 1.0f); break;
					case RARE: rarity_color = vec4(0.9f, 0.9f, 0.6f, 1.0f); break;
					case LEGENDARY: rarity_color = vec4(1.0f, 0.75f, 0.6f, 1.0f); break;
					case MASTERPIECE: rarity_color = vec4(0.6f, 0.9f, 0.9f, 1.0f); break;
					}

					rarity_text = text->getTextArray(stringFromRarity(painting_to_place->getData()->getRarity()), context,
						false, rarity_color, transparent_color, true, title_text->getLowerLeft(), info_scale, text_box_width);

					string to_print = std::to_string(painting_to_place->getData()->getDate().getYear()) + "\n" + painting_to_place->getData()->getArtistName();
					to_print += "\n$" + painting_to_place->getValue().getNumberString(true, false, 2);

					info_text = text->getTextArray(to_print, context, false, info_color, transparent_color,
						true, rarity_text->getLowerLeft(), info_scale, text_box_width);
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
							wall_selected.second->addArtwork(point_clicked, *painting_to_place);
							current_player->addPaintingToDisplay(painting_to_place);

							not_displayed_copy = current_player->getNotDisplayedCopy();
							artwork_thumbnails->clearElements();
							for (auto i : not_displayed_copy)
							{
								i->applyFrameTemplate(context, *(current_player->getDefaultFrame()));
								shared_ptr<artwork_thumbnail> thumbnail(new artwork_thumbnail(i, context, 0.3f, 0.01f));
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
						title_text = text->getTextArray(selected->getData()->getTitle(), context,
							true, title_color, transparent_color, true, title_screen_position, title_scale, text_box_width);

						switch (selected->getData()->getRarity())
						{
						case COMMON: rarity_color = vec4(0.6f, 0.9f, 0.6f, 1.0f); break;
						case UNCOMMON: rarity_color = vec4(0.6f, 0.6f, 0.9f, 1.0f); break;
						case RARE: rarity_color = vec4(0.9f, 0.9f, 0.6f, 1.0f); break;
						case LEGENDARY: rarity_color = vec4(1.0f, 0.75f, 0.6f, 1.0f); break;
						case MASTERPIECE: rarity_color = vec4(0.6f, 0.9f, 0.9f, 1.0f); break;
						}

						rarity_text = text->getTextArray(stringFromRarity(selected->getData()->getRarity()), context,
							false, rarity_color, transparent_color, true, title_text->getLowerLeft(), info_scale, text_box_width);

						string to_print = std::to_string(selected->getData()->getDate().getYear()) + "\n" + selected->getData()->getArtistName();
						to_print += "\n$" + selected->getValue().getNumberString(true, false, 2);

						info_text = text->getTextArray(to_print, context, false, info_color, transparent_color,
							true, rarity_text->getLowerLeft(), info_scale, text_box_width);

						//remove from display
						painting_to_place = nullptr;
						placement_preview = nullptr;
					}

					else
					{
						title_text = nullptr;
						info_text = nullptr;
						rarity_text = nullptr;
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
				for (auto i : not_displayed_copy)
				{
					i->applyFrameTemplate(context, *(current_player->getDefaultFrame()));
					shared_ptr<artwork_thumbnail> thumbnail(new artwork_thumbnail(i, context, 0.3f, 0.01f));
					thumbnail->setDrawSelected(highlight, fullBrightness);
					artwork_thumbnails->addElement(thumbnail);
				}

				title_text = nullptr;
				info_text = nullptr;
				rarity_text = nullptr;
			}

			if (keys->checkPress(GLFW_KEY_COMMA, false) && inventory_displayed)
			{
				artwork_thumbnails->pageDown();
				painting_to_place = nullptr;
				title_text = nullptr;
				info_text = nullptr;
				rarity_text = nullptr;
			}

			if (keys->checkPress(GLFW_KEY_PERIOD, false) && inventory_displayed)
			{
				artwork_thumbnails->pageUp();
				painting_to_place = nullptr;
				title_text = nullptr;
				info_text = nullptr;
				rarity_text = nullptr;
			}

			if (keys->checkPress(GLFW_KEY_ESCAPE))
			{
				menu_return = mainMenu(data_path, context, keys, text);
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