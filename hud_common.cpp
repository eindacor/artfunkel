#include "hud_common.h"

void refreshThumbnails(const shared_ptr<ogl_context> &context, const shared_ptr<texture_handler> &textures, const shared_ptr<player> &current_player,
	const vector<shared_ptr<artwork> > &source, shared_ptr<dynamic_hud_array> &container, const vec2 thumbnail_size, float thumbnail_padding, bool addframe)
{
	int previous_page_location = (container->isEmpty() ? 0 : container->getCurrentPage());

	container->clearElements();
	for (int i = 0; i < source.size(); i++)
	{
		string identifier = std::to_string(i) + "_" + source.at(i)->getData()->getArtistName() + "_"
			+ source.at(i)->getData()->getTitle();

		if (addframe)
			source.at(i)->applyFrameTemplate2D(context, textures, *(current_player->getDefaultFrame()));

		shared_ptr<artwork_thumbnail> thumbnail(new artwork_thumbnail(identifier, source.at(i), context, thumbnail_size, thumbnail_padding));
		thumbnail->setDrawSelected(highlight, fullBrightness);
		container->addElement(thumbnail);
	}

	while (!container->isEmpty() && !container->setVisible(previous_page_location) && previous_page_location > 0)
		previous_page_location--;
}

void setWorkInfoFields(const shared_ptr<ogl_context> &context, const shared_ptr<text_handler> &text, shared_ptr<dynamic_hud_array> &container, float scale_modifier)
{
	float title_text_height(0.045f * scale_modifier);
	vec2 title_element_dimensions(container->getAllowableWidth(), 0.1f * scale_modifier);
	pair <horizontal_justification, vertical_justification> title_just(H_LEFT, V_MIDDLE);
	bool title_italics = true;
	vec2 title_element_padding(0.015f * scale_modifier, 0.0f);
	vec2 title_spacing_scale(0.8f, 1.1f);

	shared_ptr<text_area> title_text(new text_area("title_text", "not yet set",
		context, text, title_element_dimensions, title_text_height, title_just, title_italics, V4C_WHITE,
		"text", "text_color", title_element_padding, title_spacing_scale));

	float rarity_text_height(0.03f * scale_modifier);
	vec4 rarity_color(1.0f, 1.0f, 1.0f, 1.0f);
	vec2 rarity_element_dimensions(container->getAllowableWidth(), 0.032f * scale_modifier);
	pair <horizontal_justification, vertical_justification> rarity_just(H_LEFT, V_MIDDLE);
	bool rarity_italics = false;
	vec2 rarity_element_padding(0.025f * scale_modifier, 0.0f);
	vec2 rarity_spacing_scale(0.8f, 1.0f);

	shared_ptr<text_area> rarity_text(new text_area("rarity_text", "not yet set",
		context, text, rarity_element_dimensions, rarity_text_height, rarity_just, rarity_italics, rarity_color,
		"text", "text_color", rarity_element_padding, rarity_spacing_scale));

	float artist_text_height(0.03f * scale_modifier);
	vec2 artist_element_dimensions(container->getAllowableWidth(), 0.032f * scale_modifier);
	pair <horizontal_justification, vertical_justification> artist_just(H_LEFT, V_MIDDLE);
	bool artist_italics = false;
	vec2 artist_element_padding(0.025f * scale_modifier, 0.0f);
	vec2 artist_spacing_scale(0.8f, 1.0f);

	shared_ptr<text_area> artist_text(new text_area("artist_text", "not yet set",
		context, text, artist_element_dimensions, artist_text_height, artist_just, artist_italics, V4C_GRAY_LIGHT,
		"text", "text_color", artist_element_padding, artist_spacing_scale));

	float value_text_height(0.03f * scale_modifier);
	vec2 value_element_dimensions(container->getAllowableWidth(), 0.032f * scale_modifier);
	pair <horizontal_justification, vertical_justification> value_just(H_LEFT, V_MIDDLE);
	bool value_italics = false;
	vec2 value_element_padding(0.025f * scale_modifier, 0.0f);
	vec2 value_spacing_scale(0.8f, 1.0f);

	shared_ptr<text_area> value_text(new text_area("value_text", "not yet set",
		context, text, value_element_dimensions, value_text_height, value_just, value_italics, V4C_GRAY_LIGHT,
		"text", "text_color", value_element_padding, value_spacing_scale));

	shared_ptr<text_area> default_attributes(new text_area("default_attributes_text", "not yet set",
		context, text, vec2(container->getAllowableWidth() / 2, 0.6f * scale_modifier), value_text_height, justpair(H_LEFT, V_TOP), value_italics, V4C_GRAY_LIGHT,
		"text", "text_color", value_element_padding, value_spacing_scale));

	shared_ptr<dynamic_hud_array> primary_secondary_container(new dynamic_hud_array("primary_secondary_container", context, vec2(-1.0f, 0.0f),
		justpair(H_LEFT, V_TOP), vec2(container->getAllowableWidth() / 2, 0.6f * scale_modifier), justpair(H_LEFT, V_TOP)));

	shared_ptr<text_area> primary_attributes(new text_area("primary_attributes_text", "not yet set",
		context, text, vec2(container->getAllowableWidth() / 2, 0.3f * scale_modifier), value_text_height, justpair(H_LEFT, V_TOP), value_italics, V4C_GRAY_LIGHT,
		"text", "text_color", value_element_padding, value_spacing_scale));

	shared_ptr<text_area> secondary_attributes(new text_area("secondary_attributes_text", "not yet set",
		context, text, vec2(container->getAllowableWidth() / 2, 0.3f * scale_modifier), value_text_height, justpair(H_LEFT, V_TOP), value_italics, V4C_GRAY_LIGHT,
		"text", "text_color", value_element_padding, value_spacing_scale));

	container->addElement(title_text);
	container->addElement(rarity_text);
	container->addElement(artist_text);
	container->addElement(value_text);
	container->addElement(default_attributes);
	container->addElement(primary_secondary_container);
	primary_secondary_container->addElement(primary_attributes);
	primary_secondary_container->addElement(secondary_attributes);
}

void setWorkInfoDescription(shared_ptr<dynamic_hud_array> &work_description, const shared_ptr<artwork> &work)
{
	shared_ptr<hud_element> title_element = work_description->getElementWithinByID("title_text");
	shared_ptr<text_area> title_text = boost::dynamic_pointer_cast<text_area>(title_element);
	title_text->setText(work->getData()->getTitle());

	shared_ptr<hud_element> artist_element = work_description->getElementWithinByID("artist_text");
	shared_ptr<text_area> artist_text = boost::dynamic_pointer_cast<text_area>(artist_element);
	artist_text->setText(work->getData()->getArtistName());

	shared_ptr<hud_element> value_element = work_description->getElementWithinByID("value_text");
	shared_ptr<text_area> value_text = boost::dynamic_pointer_cast<text_area>(value_element);
	value_text->setText("$" + work->getValue().getNumberString(true, false, 2));

	shared_ptr<hud_element> rarity_element = work_description->getElementWithinByID("rarity_text");
	shared_ptr<text_area> rarity_text = boost::dynamic_pointer_cast<text_area>(rarity_element);
	rarity_text->setText(stringFromRarity(work->getData()->getRarity()));

	shared_ptr<text_area> base_attribute_text = boost::dynamic_pointer_cast<text_area>(work_description->getElementWithinByID("default_attributes_text"));
	base_attribute_text->setText("\nBase Stats: \n" + work->getWorkAttributes().getBaseAttributesString());
	base_attribute_text->setColor(V4C_GREEN_LIGHT);

	shared_ptr<text_area> primary_attribute_text = boost::dynamic_pointer_cast<text_area>(work_description->getElementWithinByID("primary_attributes_text"));
	primary_attribute_text->setText("\nFeature Stats: \n" + work->getWorkAttributes().getPrimaryAttributesString());
	primary_attribute_text->setColor(V4C_YELLOW_LIGHT);

	shared_ptr<text_area> secondary_attribute_text = boost::dynamic_pointer_cast<text_area>(work_description->getElementWithinByID("secondary_attributes_text"));
	secondary_attribute_text->setText("\Non-feature Stats: \n" + work->getWorkAttributes().getSecondaryAttributesString());
	secondary_attribute_text->setColor(V4C_BLUE_LIGHT);

	rarity_text->setColor(getRarityColor(work->getData()->getRarity()));
}

shared_ptr<dynamic_hud_array> generatePlayerInfo(const shared_ptr<ogl_context> &context, const shared_ptr<text_handler> &text, const shared_ptr<player> &current_player)
{
	shared_ptr<dynamic_hud_array> player_summary(new dynamic_hud_array("player_summary", context, vec2(-1.0f, -1.0f), justpair(H_LEFT, V_BOTTOM), vec2(0.5f, 0.25f),
		justpair(H_LEFT, V_MIDDLE), vec2(0.02f, 0.1f)));

	player_summary->setBackgroundColor(vec4(0.0f, 0.0f, 0.0f, 0.7f));

	float username_text_height(0.045f);
	vec2 username_element_dimensions(0.47f, 0.09f);
	justpair username_just(H_LEFT, V_MIDDLE);
	bool username_italics = true;
	vec2 username_element_padding(0.015f, 0.0f / context->getAspectRatio());
	vec2 username_spacing_scale(0.8f, 1.1f);

	shared_ptr<text_area> username_text(new text_area("username_text", current_player->getName(),
		context, text, vec2(0.0f, 0.0f), justpair(H_CENTER, V_MIDDLE), username_element_dimensions, 
		username_text_height, username_just, username_italics, V4C_WHITE,
		"text", "text_color", username_element_padding, username_spacing_scale));

	float collection_text_height(0.03f);
	vec2 collection_element_dimensions(0.47f, 0.032f);
	justpair collection_just(H_LEFT, V_MIDDLE);
	bool collection_italics = false;
	vec2 collection_element_padding(0.015f, 0.0f);
	vec2 collection_spacing_scale(0.8f, 1.0f);

	shared_ptr<text_area> collection_text(new text_area("collection_text",
		"Collection Value: $" + current_player->getCollectionValue().getNumberString(true, false, 2), context, text, vec2(0.0f, 0.0f), justpair(H_CENTER, V_MIDDLE),
		collection_element_dimensions, collection_text_height, collection_just, collection_italics, V4C_GRAY,
		"text", "text_color", collection_element_padding, collection_spacing_scale));

	float bank_text_height(0.03f);
	vec2 bank_element_dimensions(0.47f, 0.032f);
	justpair bank_just(H_LEFT, V_MIDDLE);
	bool bank_italics = false;
	vec2 bank_element_padding(0.015f, 0.0f);
	vec2 bank_spacing_scale(0.8f, 1.0f);

	shared_ptr<text_area> bank_text(new text_area("bank_text",
		"Bank Balance: $" + current_player->getBankBalanceString(true), context, text, vec2(0.0f, 0.0f), justpair(H_CENTER, V_MIDDLE),
		collection_element_dimensions, collection_text_height, collection_just, collection_italics, V4C_GRAY,
		"text", "text_color", collection_element_padding, collection_spacing_scale));

	player_summary->addElement(username_text);
	player_summary->addElement(collection_text);
	player_summary->addElement(bank_text);

	username_text->setText(current_player->getName());
	bank_text->setText("Bank Balance: $" + current_player->getBankBalanceString(true));
	collection_text->setText("Collection Value: $" + current_player->getCollectionValue().getNumberString(true, false, 2));

	return player_summary;
}

void refreshPlayerInfo(const shared_ptr<dynamic_hud_array> &player_summary, const shared_ptr<player> &current_player)
{
	shared_ptr<hud_element> bank_element = player_summary->getElementWithinByID("bank_text");
	shared_ptr<text_area> bank_text = boost::dynamic_pointer_cast<text_area>(bank_element);
	bank_text->setText("Bank Balance: $" + current_player->getBankBalanceString(true));

	shared_ptr<hud_element> collection_element = player_summary->getElementWithinByID("collection_text");
	shared_ptr<text_area> collection_text = boost::dynamic_pointer_cast<text_area>(collection_element);
	collection_text->setText("Collection Value: $" + current_player->getCollectionValue().getNumberString(true, false, 2));
}