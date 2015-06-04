#include "header.h"
#include "player.h"
#include "artwork.h"
#include "utility_funcs.h"
#include "menus.h"
#include "gallery.h"
#include "hud.h"

int viewInventory(string data_path, const shared_ptr<ogl_context> &context,
	shared_ptr<key_handler> &keys, shared_ptr<player> &current_player, 
	const shared_ptr<text_handler> &text, shared_ptr<texture_handler> &textures)
{
	vec4 original_background = context->getBackgroundColor();
	context->setBackgroundColor(vec4(0.5f, 0.3f, 0.0f, 1.0f));

	int display_count = 10;

	//TODO remove inventory copy mechanic. use actual inventory container with active iterators
	//add copies of the artwork instances to the local vector, so position can be manipulated
	vector<shared_ptr<artwork> >inventory_copy = current_player->getInventoryCopy();
	shared_ptr<dynamic_hud_array> artwork_thumbnails(new dynamic_hud_array("thumbnails", context, vec2(0.4f, .15f), 1.2f, 1.7f ,
		pair<horizontal_justification, vertical_justification>(H_LEFT, V_TOP)));

	artwork_thumbnails->setBackgroundColor(vec4(0.0f, 0.0f, 0.0f, 0.4f));

	//TODO equip dynamic array with function that generates thumbnails from artwork
	//add player's default frames to each
	for (int i = 0; i < inventory_copy.size(); i++)
	{
		string identifier = std::to_string(i) + "_" + inventory_copy.at(i)->getData()->getArtistName() + "_"
			+ inventory_copy.at(i)->getData()->getTitle();
		inventory_copy.at(i)->applyFrameTemplate2D(context, textures, *(current_player->getDefaultFrame()));
		shared_ptr<artwork_thumbnail> thumbnail(new artwork_thumbnail(identifier, inventory_copy.at(i), context, vec2(0.2f, 0.3f), 0.04f));
		thumbnail->setDrawSelected(highlight, fullBrightness);
		artwork_thumbnails->addElement(thumbnail);
	}

	/////////////////////UPDATED HUD
	//identify positions for text
	shared_ptr<dynamic_hud_array> work_description(new dynamic_hud_array("description", context, vec2(-0.6f, -0.5f), 0.8f, 1.0f,
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


	shared_ptr<dynamic_hud_array> player_summary(new dynamic_hud_array("player_summary", context, vec2(0.4f, -0.85f), 1.2f, 0.3f,
		pair<horizontal_justification, vertical_justification>(H_LEFT, V_MIDDLE), vec2(0.02f, 0.1f)));

	player_summary->setBackgroundColor(vec4(0.0f, 0.0f, 0.0f, 0.7f));

	float username_text_height(0.045f);
	vec4 username_color(1.0f, 1.0f, 1.0f, 1.0f);
	vec2 username_centerpoint(0.0f, 0.0f);
	vec2 username_element_dimensions(0.76f, 0.1f);
	pair <horizontal_justification, vertical_justification> username_just(H_LEFT, V_MIDDLE);
	bool username_italics = true;
	vec2 username_element_padding(0.015f, 0.0f / context->getAspectRatio());
	vec2 username_spacing_scale(0.8f, 1.1f);

	shared_ptr<text_area> username_text(new text_area("username_text", current_player->getName(),
		context, text, username_centerpoint, username_element_dimensions, username_text_height, username_just, username_italics, username_color,
		"text", "text_color", username_element_padding, username_spacing_scale));

	float collection_text_height(0.03f);
	vec4 collection_color(0.7f, 0.7f, 0.7f, 1.0f);
	vec2 collection_centerpoint(0.0f, 0.0f);
	vec2 collection_element_dimensions(0.76f, 0.032f);
	pair <horizontal_justification, vertical_justification> collection_just(H_LEFT, V_MIDDLE);
	bool collection_italics = false;
	vec2 collection_element_padding(0.025f, 0.0f);
	vec2 collection_spacing_scale(0.8f, 1.0f);

	shared_ptr<text_area> collection_text(new text_area("collection_text", 
		"Collection Value: $" + current_player->getCollectionValue().getNumberString(true, false, 2), context, text, collection_centerpoint, 
		collection_element_dimensions, collection_text_height, collection_just, collection_italics, collection_color,
		"text", "text_color", collection_element_padding, collection_spacing_scale));

	float bank_text_height(0.03f);
	vec4 bank_color(0.7f, 0.7f, 0.7f, 1.0f);
	vec2 bank_centerpoint(0.0f, 0.0f);
	vec2 bank_element_dimensions(0.76f, 0.032f);
	pair <horizontal_justification, vertical_justification> bank_just(H_LEFT, V_MIDDLE);
	bool bank_italics = false;
	vec2 bank_element_padding(0.025f, 0.0f);
	vec2 bank_spacing_scale(0.8f, 1.0f);

	shared_ptr<text_area> bank_text(new text_area("bank_text",
		"Bank Balance: $" + current_player->getBankBalanceString(true), context, text, collection_centerpoint,
		collection_element_dimensions, collection_text_height, collection_just, collection_italics, collection_color,
		"text", "text_color", collection_element_padding, collection_spacing_scale));

	player_summary->addElement(username_text);
	player_summary->addElement(collection_text);
	player_summary->addElement(bank_text);

	////////////////////////////////

	shared_ptr<artwork_thumbnail> selected_painting = nullptr;

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
	
			player_summary->draw(context, camera);
			artwork_thumbnails->draw(context, camera);

			if (selected_painting != nullptr)
			{
				work_description->draw(context, camera);
				selected_painting->draw(context, camera);
			}

			else work_description->drawBackground(context, camera);

			context->swapBuffers();

			if (keys->checkPress(GLFW_KEY_ESCAPE, false))
			{
				menu_return = mainMenu(data_path, context, keys, current_player, text, textures);
				finished = (menu_return != 1);
			}

			if (keys->checkMouse(GLFW_MOUSE_BUTTON_LEFT, false))
			{
				vec2 cursor_position = keys->getCursorPosition();
				hud_element_type selected_type;
				string identifier;
				shared_ptr<hud_element> selected = artwork_thumbnails->getSelectedWithinArray(keys, cursor_position, selected_type, identifier);

				if (selected_type == THUMBNAIL)
				{
					selected_painting = shared_ptr<artwork_thumbnail>(new artwork_thumbnail("selected", selected->getStoredArt(), context, 
						vec2(-.6f, 0.5f), vec2(0.8f, 1.0f), 0.1f));
					
					title_text->setText(selected_painting->getStoredArt()->getData()->getTitle());
					artist_text->setText(selected_painting->getStoredArt()->getData()->getArtistName());
					value_text->setText("$" + selected_painting->getStoredArt()->getValue().getNumberString(true, false, 2));
					rarity_text->setText(stringFromRarity(selected_painting->getStoredArt()->getData()->getRarity()));

					switch (selected_painting->getStoredArt()->getData()->getRarity())
					{
					case COMMON: rarity_text->setColor(vec4(0.6f, 0.9f, 0.6f, 1.0f)); break;
					case UNCOMMON: rarity_text->setColor(vec4(0.6f, 0.6f, 0.9f, 1.0f)); break;
					case RARE: rarity_text->setColor(vec4(0.9f, 0.9f, 0.6f, 1.0f)); break;
					case LEGENDARY: rarity_text->setColor(vec4(1.0f, 0.75f, 0.6f, 1.0f)); break;
					case MASTERPIECE: rarity_text->setColor(vec4(0.6f, 0.9f, 0.9f, 1.0f)); break;
					}
				}

				else
				{
					selected_painting = nullptr;
				}
			}

			if (keys->checkPress(GLFW_KEY_COMMA, false))
			{
				artwork_thumbnails->pageDown();
				selected_painting = nullptr;
			}

			if (keys->checkPress(GLFW_KEY_PERIOD, false))
			{
				artwork_thumbnails->pageUp();
				selected_painting = nullptr;
			}

			if (keys->checkPress(GLFW_KEY_EQUAL, false))
			{
				current_player->addFunds(bignum("100000"));
				bank_text->setText("Bank Balance: $" + current_player->getBankBalanceString(true));
			}

			if ((keys->checkPress(GLFW_KEY_BACKSPACE, false) || keys->checkPress(GLFW_KEY_DELETE, false)) && selected_painting != nullptr)
			{
				current_player->removeWorkFromInventory(selected_painting->getStoredArt());

				bank_text->setText("Bank Balance: $" + current_player->getBankBalanceString(true));
				collection_text->setText("Collection Value: $" + current_player->getCollectionValue().getNumberString(true, false, 2));

				artwork_thumbnails->clearElements();

				inventory_copy.clear();
				inventory_copy = current_player->getInventoryCopy();
				selected_painting = nullptr;

				for (int i = 0; i < inventory_copy.size(); i++)
				{
					string identifier = std::to_string(i) + "_" + inventory_copy.at(i)->getData()->getArtistName() + "_"
						+ inventory_copy.at(i)->getData()->getTitle();
					inventory_copy.at(i)->applyFrameTemplate2D(context, textures, *(current_player->getDefaultFrame()));
					shared_ptr<artwork_thumbnail> thumbnail(new artwork_thumbnail(identifier, inventory_copy.at(i), context, vec2(0.2f, 0.3f), 0.04f));
					thumbnail->setDrawSelected(highlight, fullBrightness);
					artwork_thumbnails->addElement(thumbnail);
				}
			}

			glfwSetTime(0.0f);
		}
	}

	context->setBackgroundColor(original_background);
	return menu_return;
}