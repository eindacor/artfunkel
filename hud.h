#ifndef HUD_H
#define HUD_H

#include "header.h"
#include "artwork.h"

//hud_element abstract class
class hud_element
{
public:
	hud_element(const vec2 &item_centerpoint, float on_screen_width, float on_screen_height, hud_element_type type);
	~hud_element(){};

	virtual bool itemSelected(shared_ptr<key_handler> &keys, const vec2 &cursor_position);
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

	bool isCurrentlyHovered() const { return currently_hovered; }
	bool isCurrentlySelected() const { return currently_selected; }
	void setCurrentlyHovered(bool set) { currently_hovered = set; }
	void setCurrentlySelected(bool set) { currently_selected = set; }

	hud_element_type getType() const { return element_type; }
	void updatePoints();

	void drawLines(const shared_ptr<ogl_context> &context, const shared_ptr<ogl_camera> &camera) const {
		for (auto i : lines)
			i->draw(context, camera, true);
	}

	virtual shared_ptr<hud_element> getSelectedWithinArray(
		shared_ptr<key_handler> &keys, const vec2 &cursor_position, hud_element_type &type, string &identifier) 
	{
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

	void clearBackgroundImage() { background_image = nullptr; }
	void clearBackgroundColor() { background_rec = nullptr; }

	void drawBackground(const shared_ptr<ogl_context> &context, const shared_ptr<ogl_camera> &camera) const;

private:

	//below are the binding points for click detection
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
};

//TODO create first and last getters, change to a thumbnail-specific class
//TODO create dynamic hud_element class that stores multiple hud_elements and spaces them equally, similar to the way text wrap works
class dynamic_hud_array : public hud_element
{
public:
	//TODO swap width with height in constructor
	dynamic_hud_array(const shared_ptr<ogl_context> &ogl_con, const vec2 centerpoint,
		float on_screen_width, float on_screen_height, pair<horizontal_justification, vertical_justification> j, float padding = 0.0f)
		: hud_element(centerpoint, on_screen_width, on_screen_height, ELEMENT_ARRAY)
	{
		context = ogl_con;
		justification = j;
		array_padding = padding;
	}
	~dynamic_hud_array(){};

	//TODO add methods for adding specific hud elements
	void addElement(const shared_ptr<hud_element> &to_add);
	void addElements(const vector< shared_ptr<hud_element> > &element_vec);
	void setElementPositions();
	void clearElements() { array_elements.clear(); visible_lines.clear(); }
	//getSelectedWithinArray is design to recurse if it detects nested arrays
	virtual shared_ptr<hud_element> getSelectedWithinArray(
		shared_ptr<key_handler> &keys, const vec2 &cursor_position, hud_element_type &type, string &identifier) const;

	void draw(const shared_ptr<ogl_context> &context, const shared_ptr<ogl_camera> &camera) const;
	void setLines();

private:
	//used during spacing of array
	void setXForLine(vector< shared_ptr<hud_element> > &line_contents, float initial_x_offset);
	void setYForLine(vector< shared_ptr<hud_element> > &line_contents, float y_offset);

	//TODO add iterators taht denote "pages" for paging in menus
	vector< shared_ptr<hud_element> > array_elements;
	map <int, vector<shared_ptr<hud_element> > >visible_lines;
	float array_padding;
	pair<horizontal_justification, vertical_justification> justification;
	vector< shared_ptr<line> > lines;

	shared_ptr<ogl_context> context;
};

class artwork_thumbnail : public hud_element
{
public:
	//dimensions & position
	artwork_thumbnail(const shared_ptr<artwork> &art, 
		const shared_ptr<ogl_context> &context, 
		const vec2 centerpoint, 
		const vec2 on_screen_dimensions,
		float padding)
		: hud_element(centerpoint, on_screen_dimensions.x, on_screen_dimensions.y, THUMBNAIL)
	{
		stored = art;
		thumbnail_padding = padding;  
		updateScale(context);
	}

	//square & position
	artwork_thumbnail(const shared_ptr<artwork> &art,
		const shared_ptr<ogl_context> &context,
		const vec2 centerpoint,
		float square_height, 
		float padding)
		: hud_element(centerpoint, square_height / context->getAspectRatio(), square_height, THUMBNAIL)
	{
		stored = art;
		thumbnail_padding = padding;
		updateScale(context);
	}

	//dimensions & no position
	artwork_thumbnail(const shared_ptr<artwork> &art,
		const shared_ptr<ogl_context> &context,
		const vec2 on_screen_dimensions, 
		float padding)
		: hud_element(vec2(0.0f, 0.0f), on_screen_dimensions.x, on_screen_dimensions.y, THUMBNAIL)
	{
		stored = art;
		thumbnail_padding = padding;
		updateScale(context);
	}

	//square & no position
	artwork_thumbnail(const shared_ptr<artwork> &art,
		const shared_ptr<ogl_context> &context,
		float on_screen_height, 
		float padding = 0.0f)
		: hud_element(vec2(0.0f, 0.0f), on_screen_height / context->getAspectRatio(), on_screen_height, THUMBNAIL)
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

/*
class text_box : public hud_element
{
public:
	text_box(){};
	~text_box(){};

private:


};

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