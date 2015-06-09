#ifndef HUD_H
#define HUD_H

#include "header.h"
#include "artwork.h"

//TODO create typedef for horizontal/vertical justifications
//hud_element abstract class
class hud_element
{
public:
	hud_element(const string &hud_identifier, const vec2 &anchor_point, const justpair &anchor_location, 
		const vec2 &on_screen_dimensions, const hud_element_type &type);
	~hud_element(){};

	//TODO Phase out itemSelected
	virtual bool itemSelected(shared_ptr<key_handler> &keys, const vec2 &cursor_position, bool deselect_if_false=true);

	bool cursorIsOver(const vec2 &cursor_position) const;
	void select() { currently_selected = true; setBackgroundColor(vec4(1.0f, 1.0f, 1.0f, 0.4f)); }
	void deselect() { currently_selected = false; clearBackgroundColor(); }
	virtual void draw(const shared_ptr<ogl_context> &context, const shared_ptr<ogl_camera> &camera) const = 0;

	vec2 getUpperLeft() const { return upper_left; }
	vec2 getUpperRight() const { return upper_right; }
	vec2 getLowerLeft() const { return lower_left; }
	vec2 getLowerRight() const { return lower_right; }
	vec2 getCenterpoint() const { return centerpoint; }
	virtual void setX(float x_offset) { centerpoint.x = x_offset; updatePoints(); }
	virtual void setY(float y_offset) { centerpoint.y = y_offset; updatePoints(); }
	mat4 getTranslationMatrix() const { return glm::translate(mat4(1.0f), vec3(centerpoint.x, centerpoint.y, 0.0f)); }
	float getHeight() const { return height; }
	float getWidth() const { return width; }
	void setHeight(float f) { height = f; }
	void setWidth(float w) { width = w; }

	bool isCurrentlyHovered() const { return currently_hovered; }
	bool isCurrentlySelected() const { return currently_selected; }
	void setCurrentlyHovered(bool set) { currently_hovered = set; }
	void setCurrentlySelected(bool set) { currently_selected = set; }

	hud_element_type getType() const { return element_type; }
	void updatePoints();

	void drawLines(const shared_ptr<ogl_context> &context, const shared_ptr<ogl_camera> &camera) const {
		for (const auto &i : lines)
			i->draw(context, camera, true);
	}

	virtual shared_ptr<hud_element> getSelectedWithinArray(
		shared_ptr<key_handler> &keys, const vec2 &cursor_position, hud_element_type &type, string &identifier) 
	{
		cout << "null search" << endl;
		type = NO_TYPE;
		return nullptr;
	}

	//TODO templatize for future elements
	virtual shared_ptr<artwork> getStoredArt() const { return nullptr; }
	virtual shared_ptr<artwork> getStoredText() const { return nullptr; }

	void setDrawSelected(void(*preDS)(const shared_ptr<ogl_context> &context, const shared_ptr<ogl_camera> &camera),
		void(*postDS)(const shared_ptr<ogl_context> &context, const shared_ptr<ogl_camera> &camera)) {
		preDrawSelected = preDS; postDrawSelected = postDS;
	}
	void setDrawNotSelected(void(*preDNS)(const shared_ptr<ogl_context> &context, const shared_ptr<ogl_camera> &camera),
		void(*postDNS)(const shared_ptr<ogl_context> &context, const shared_ptr<ogl_camera> &camera)) {
		preDrawNotSelected = preDNS; postDrawNotSelected = postDNS;
	}
	void clearDrawSelected() { preDrawSelected = nullptr; postDrawSelected = nullptr; }
	void clearDrawNotSelected() { preDrawNotSelected = nullptr; postDrawNotSelected = nullptr; }

	void(*preDrawSelected)(const shared_ptr<ogl_context> &context, const shared_ptr<ogl_camera> &camera);
	void(*postDrawSelected)(const shared_ptr<ogl_context> &context, const shared_ptr<ogl_camera> &camera);
	void(*preDrawNotSelected)(const shared_ptr<ogl_context> &context, const shared_ptr<ogl_camera> &camera);
	void(*postDrawNotSelected)(const shared_ptr<ogl_context> &context, const shared_ptr<ogl_camera> &camera);
	void(*preDrawHovered)(const shared_ptr<ogl_context> &context, const shared_ptr<ogl_camera> &camera);
	void(*postDrawHovered)(const shared_ptr<ogl_context> &context, const shared_ptr<ogl_camera> &camera);

	void setBackgroundImage(const shared_ptr<ogl_context> &context, const char* image_path);
	void setBackgroundColor(vec4 color);
	void setBackgroundImage(const shared_ptr<image> &bi) { background_image = bi; }
	void setBackgroundRec(const shared_ptr<rectangle> &br) { background_rec = br; }
	void setSelectable(bool b) { selectable = b; }
	bool isSelectable() const { return selectable; }
	void setVisibility(bool b) { visible = b; }
	bool isVisible() const { return visible; }
	void setLineDraw(bool b) { draw_lines = b; }
	bool drawLinesEnabled() const { return draw_lines; }

	void clearBackgroundImage() { background_image = nullptr; }
	void clearBackgroundColor() { background_rec = nullptr; }

	void drawBackground(const shared_ptr<ogl_context> &context, const shared_ptr<ogl_camera> &camera) const;

	string getIdentifier() const { return identifier; }

private:

	//below are the binding points for click detection
	string identifier;
	vec2 upper_left, upper_right, lower_left, lower_right;
	float height, width;
	vec2 centerpoint;
	hud_element_type element_type;
	vector< shared_ptr<line> > lines;

	shared_ptr<rectangle> background_rec;
	shared_ptr<image> background_image;

	bool currently_hovered;
	bool currently_selected;
	bool currently_not_selected;
	bool selectable = true;
	bool visible = true;
	bool draw_lines = false;
};

//TODO create first and last getters, change to a thumbnail-specific class
//TODO create dynamic hud_element class that stores multiple hud_elements and spaces them equally, similar to the way text wrap works
class dynamic_hud_array : public hud_element
{
public:
	//TODO swap width with height in constructor
	dynamic_hud_array(string identifier, const shared_ptr<ogl_context> &ogl_con, const vec2 &anchor_point,
		const justpair &anchor_location, const vec2 &on_screen_dimensions,
		const justpair interior_justification, vec2 padding = vec2(0.0f, 0.0f))
		: hud_element(identifier, anchor_point, anchor_location, on_screen_dimensions, ELEMENT_ARRAY)
	{
		context = ogl_con;
		justification = interior_justification;
		array_padding = padding;
		setSelectable(false);
		deselect_on_miss = true;
		select_multiple = false;
	}

	dynamic_hud_array(string identifier, const shared_ptr<ogl_context> &ogl_con, const vec2 &on_screen_dimensions,
		const justpair interior_justification, vec2 padding = vec2(0.0f, 0.0f))
		: hud_element(identifier, vec2(0.0f, 0.0f), justpair(H_CENTER, V_MIDDLE), on_screen_dimensions, ELEMENT_ARRAY)
	{
		context = ogl_con;
		justification = interior_justification;
		array_padding = padding;
		setSelectable(false);
		deselect_on_miss = true;
		select_multiple = false;
	}
	~dynamic_hud_array(){};

	//TODO add methods for adding specific hud elements
	void addElement(const shared_ptr<hud_element> &to_add);
	void addElements(const vector< shared_ptr<hud_element> > &element_vec);
	void setPageData();
	bool setVisible(int page_number);
	void pageUp() 
	{ 
		if (!isLastPage())
		{
			deselectAllWithin();
			setVisible(current_page + 1);
		}
	}
		
	void pageDown()
	{
		if (!isFirstPage())
		{
			deselectAllWithin();
			setVisible(current_page - 1);
		}
	}

	int getCurrentPage() const { return current_page; }
	bool isLastPage() const { return page_map.find(current_page + 1) == page_map.end(); }
	bool isFirstPage() const { return page_map.find(current_page) == page_map.begin(); }
	bool isEmpty() const { return array_elements.size() == 0; }
	void clearElements() { array_elements.clear(); visible_lines.clear(); }
	shared_ptr<hud_element> getElementWithinByID(const string &ID) const;
	vector<string> getSelectedItemsWithin() const;

	void deselectAllWithin();
	void deselectAllOthersWithin(const string &exception);
	//bool handleClick(const vec2 &cursor_position, string &identifier);
	shared_ptr<hud_element> getMouseoverElement(const vec2 &cursor_position, bool select);

	void setSelectMultiple(bool b) { select_multiple = b; }
	void setDeselectOnMiss(bool b) { deselect_on_miss = b; }

	void draw(const shared_ptr<ogl_context> &context, const shared_ptr<ogl_camera> &camera) const;
	void setLines();

	float getAllowableWidth() const { return getWidth() - (array_padding.x * 2.0f); }
	float getAllowableHeight() const { return getHeight() - (array_padding.y * 2.0f); }

private:
	//used during spacing of array
	void setXForLine(vector< shared_ptr<hud_element> > &line_contents, float initial_x_offset);
	void setYForLine(vector< shared_ptr<hud_element> > &line_contents, float y_offset);

	//TODO add iterators taht denote "pages" for paging in menus
	vector< shared_ptr<hud_element> > array_elements;
	map <int, vector<shared_ptr<hud_element> > >visible_lines;
	vec2 array_padding;
	justpair justification;
	vector< shared_ptr<line> > lines;

	vector<string> selected_items_within;
	bool select_multiple;
	bool deselect_on_miss;

	shared_ptr<ogl_context> context;

	int current_page;

	//int is page number (zero-indexed), iterator is first element of each page
	map<int, vector< shared_ptr<hud_element> >::iterator > page_map;
};

class text_area : public hud_element
{
public:
	text_area(string identifier, std::string s, const shared_ptr<ogl_context> &ogl_con, const shared_ptr<text_handler> &th,
		const vec2 &anchor_point, const justpair &anchor_location, const vec2 &on_screen_area_dimensions, float on_screen_height,
		const justpair interior_justification, bool italics, const glm::vec4 &color, GLchar* text_enable_ID,
		GLchar* text_color_ID, vec2 element_Padding = vec2(0.0f, 0.0f), vec2 character_spacing_scale=vec2(1.0f, 1.0f)) :
		hud_element(identifier, anchor_point, anchor_location, on_screen_area_dimensions, TEXT_AREA)
	{
		//character_dimensions = vec2(on_screen_height / ogl_con->getAspectRatio(), on_screen_height);
		character_dimensions = vec2(on_screen_height / ogl_con->getAspectRatio(), on_screen_height);
		context = ogl_con;
		justification = interior_justification;
		element_padding = element_Padding;
		xy_spacing_scale = character_spacing_scale;

		raw_text = s;
		text_enable_shader_ID = text_enable_ID;
		text_color_shader_ID = text_color_ID;
		text_color = color;

		text = th;
		ital = italics;

		setPageData();

		//TODO add code for setting text array
	};

	//disregards position(to be set in a dynamic array)
	text_area(string identifier, std::string s, const shared_ptr<ogl_context> &ogl_con, const shared_ptr<text_handler> &th, const vec2 &on_screen_area_dimensions, float on_screen_height,
		const justpair interior_justification, bool italics, const glm::vec4 &color, GLchar* text_enable_ID,
		GLchar* text_color_ID, vec2 element_Padding = vec2(0.0f, 0.0f), vec2 character_spacing_scale = vec2(1.0f, 1.0f)) :
		hud_element(identifier, vec2(0.0f, 0.0f), justpair(H_CENTER, V_MIDDLE), on_screen_area_dimensions, TEXT_AREA)
	{
		//character_dimensions = vec2(on_screen_height / ogl_con->getAspectRatio(), on_screen_height);
		character_dimensions = vec2(on_screen_height / ogl_con->getAspectRatio(), on_screen_height);
		context = ogl_con;
		justification = interior_justification;
		element_padding = element_Padding;
		xy_spacing_scale = character_spacing_scale;

		raw_text = s;
		text_enable_shader_ID = text_enable_ID;
		text_color_shader_ID = text_color_ID;
		text_color = color;

		text = th;
		ital = italics;

		setPageData();

		//TODO add code for setting text array
	};
	~text_area(){};

	void addText(string s) { raw_text += s; setPageData(); }
	void setText(string s) { raw_text = s; setPageData(); }
	void setColor(vec4 c) { text_color = c; }
	void backspace();
	void draw(const shared_ptr<ogl_context> &context, const shared_ptr<ogl_camera> &camera) const;

	void pageUp() { setVisible(current_first_line + lines_per_page); }
	void pageDown() { setVisible(current_first_line - lines_per_page); }
	int getCurrentFirstLine() const { return current_first_line; }

	bool setVisible(int first_line_index);

private:
	void setPageData();
	shared_ptr<text_handler> text;

	string raw_text;
	bool ital;
	shared_ptr<ogl_context> context;
	justpair justification;
	vec2 element_padding, xy_spacing_scale;
	int lines_per_page;

	vector<shared_ptr<text_character> > visible_characters;
	//int is page number (zero-indexed), iterator is first element of each page
	map<int, string> line_map;
	int current_first_line;

	vector< shared_ptr<line> > guidelines;

	vec4 text_color;

	GLchar* text_enable_shader_ID;
	GLchar* text_color_shader_ID;

	vec2 character_dimensions;
};

//TODO simplify constructors and purge unused
class artwork_thumbnail : public hud_element
{
public:
	//dimensions & position
	artwork_thumbnail(string identifier, const shared_ptr<artwork> &art,
		const shared_ptr<ogl_context> &context, 
		const vec2 &anchor_point, 
		const justpair &anchor_location,
		const vec2 &on_screen_dimensions,
		float padding)
		: hud_element(identifier, anchor_point, anchor_location, on_screen_dimensions, THUMBNAIL)
	{
		stored = art;
		thumbnail_padding = padding;  
		updateScale(context);
	}

	//square & position
	artwork_thumbnail(string identifier, const shared_ptr<artwork> &art,
		const shared_ptr<ogl_context> &context,
		const vec2 anchor_point,
		float square_height, 
		float padding)
		: hud_element(identifier, anchor_point, 
		justpair(H_CENTER, V_MIDDLE),
		vec2(square_height / context->getAspectRatio(), square_height), 
		THUMBNAIL)
	{
		stored = art;
		thumbnail_padding = padding;
		updateScale(context);
	}

	//dimensions & no position
	artwork_thumbnail(string identifier, const shared_ptr<artwork> &art,
		const shared_ptr<ogl_context> &context,
		const vec2 on_screen_dimensions, 
		float padding)
		: hud_element(identifier, vec2(0.0f, 0.0f), 
		justpair(H_CENTER, V_MIDDLE),
		on_screen_dimensions, 
		THUMBNAIL)
	{
		stored = art;
		thumbnail_padding = padding;
		updateScale(context);
	}

	//square & no position
	artwork_thumbnail(string identifier, const shared_ptr<artwork> &art,
		const shared_ptr<ogl_context> &context,
		float on_screen_height, 
		float padding = 0.0f)
		: hud_element(identifier, vec2(0.0f, 0.0f), 
		justpair(H_CENTER, V_MIDDLE),
		vec2(on_screen_height / context->getAspectRatio(), on_screen_height), 
		THUMBNAIL)
	{
		stored = art;
		thumbnail_padding = padding;
		updateScale(context);
	}
	~artwork_thumbnail(){};

	void updateScale(const shared_ptr<ogl_context> &context) { scale_matrix = calcScaleMatrix(context); }
	void draw(const shared_ptr<ogl_context> &context, const shared_ptr<ogl_camera> &camera) const;
	virtual bool isSelected(shared_ptr<key_handler> &keys, const vec2 &cursor_position, shared_ptr<artwork> &selected);

	virtual shared_ptr<artwork> getStoredArt() const { return stored; }

private:
	mat4 calcScaleMatrix(const shared_ptr<ogl_context> &context) const;
	mat4 scale_matrix;
	shared_ptr<artwork> stored;
	float thumbnail_padding;
};

class finish_thumbnail : public hud_element
{
public:
	finish_thumbnail(string identifier, string tex_filename, string fin_name, const shared_ptr<GLuint> &TEX,
		const shared_ptr<ogl_context> &context, const vec2 &anchor_point, const justpair &anchor_location,
		const vec2 &on_screen_dimensions, vec2 padding)
		: hud_element(identifier, anchor_point, anchor_location, on_screen_dimensions, FINISH_THUMBNAIL)
	{
		finish_name = fin_name;
		stored_TEX = TEX;
		texture_filename = tex_filename;

		vec2 actual_on_screen_image_dimensions(getWidth() - (padding.x * 2.0f), getHeight() - (padding.y * 2.0f));
		
		stored_image = shared_ptr<image>(new image(anchor_point, actual_on_screen_image_dimensions, context, TEX));
	}

	finish_thumbnail(string identifier, string tex_filename, string fin_name, const shared_ptr<GLuint> &TEX,
		const shared_ptr<ogl_context> &context, const vec2 &on_screen_dimensions, vec2 padding)
		: hud_element(identifier, vec2(0.0f, 0.0f), justpair(H_CENTER, V_MIDDLE), on_screen_dimensions, FINISH_THUMBNAIL)
	{
		finish_name = fin_name;
		stored_TEX = TEX;
		texture_filename = tex_filename;

		vec2 actual_on_screen_image_dimensions(getWidth() - (padding.x * 2.0f), getHeight() - (padding.y * 2.0f));
		stored_image = shared_ptr<image>(new image(vec2(0.0f, 0.0f), actual_on_screen_image_dimensions, context, TEX));
	}
	~finish_thumbnail(){};

	shared_ptr<GLuint> getStoredTEX() const { return stored_TEX; }
	virtual void draw(const shared_ptr<ogl_context> &context, const shared_ptr<ogl_camera> &camera) const;
	string getFinishName() const { return finish_name; }
	string getTextureFilename() const { return texture_filename; }

private:
	shared_ptr<GLuint> stored_TEX;
	string finish_name;
	string texture_filename;
	shared_ptr<image> stored_image;
};

/*

class button : public hud_element
{
public:
	button(){};
	~button(){};

private:


};

class text_field : public hud_element
{
public:
	text_field(){};
	~text_field(){};

private:


};
*/

//TODO add HUD element container class, where elements can be assigned names for click detection
class hud
{
public: 
	hud(){};
	~hud(){};

	//returns true if element added successfully
	bool addElement(string identifier, const shared_ptr<hud_element> &to_add);
	shared_ptr<hud_element> getElement(string identifier) const;
	shared_ptr<hud_element> getSelected(shared_ptr<key_handler> &keys, hud_element_type &type, string &identifier) const;

	void draw(const shared_ptr<ogl_context> &context, const shared_ptr<ogl_camera> &camera) const;

private:
	map<string, shared_ptr<hud_element> > element_map;
};

#endif