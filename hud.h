#ifndef HUD_H
#define HUD_H

#include "header.h"
#include "artwork.h"

//hud_element abstract class
class hud_element
{
public:
	hud_element(const vec2 &item_centerpoint, float item_height, float item_width, hud_element_type type);
	~hud_element(){};

	virtual bool itemSelected(shared_ptr<key_handler> &keys, const vec2 &cursor_position) const;
	virtual void draw(const shared_ptr<ogl_context> &context, const shared_ptr<ogl_camera> &camera) const = 0;
	vec2 getUpperLeft() const { return upper_left; }
	vec2 getUpperRight() const { return upper_right; }
	vec2 getLowerLeft() const { return lower_left; }
	vec2 getLowerRight() const { return lower_right; }
	vec2 getCenterpoint() const { return centerpoint; }
	virtual void setX(float x_offset) { centerpoint.x = x_offset; setTranslationMatrix(); }
	virtual void setY(float y_offset) { centerpoint.y = y_offset; setTranslationMatrix(); }
	void setTranslationMatrix() { translation_matrix = glm::translate(mat4(1.0f), vec3(centerpoint.x, centerpoint.y, 0.0f)); }
	mat4 getTranslationMatrix() const { return translation_matrix; }
	float getHeight() const { return height; }
	float getWidth() const { return width; }
	hud_element_type getType() const { return element_type; }

	virtual shared_ptr<hud_element> getSelectedWithinArray(
		shared_ptr<key_handler> &keys, const vec2 &cursor_position, hud_element_type &type, string &identifier) 
	{
		return nullptr;
	}

private:
	//below are the binding points for click detection
	vec2 upper_left, upper_right, lower_left, lower_right;
	float height, width;
	vec2 centerpoint;
	hud_element_type element_type;
	mat4 translation_matrix;
};

//TODO create dynamic hud_element class that stores multiple hud_elements and spaces them equally, similar to the way text wrap works
class dynamic_hud_array : public hud_element
{
public:
	dynamic_hud_array(const shared_ptr<ogl_context> &ogl_con, const vec2 centerpoint,
		float screen_height, float screen_width, pair<horizontal_justification, vertical_justification> j, float padding = 0.0f)
		: hud_element(centerpoint, screen_height, screen_width, ELEMENT_ARRAY)
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
	void clearElements() { array_elements.clear(); }
	//TODO update stub
	virtual shared_ptr<hud_element> getSelectedWithinArray(
		shared_ptr<key_handler> &keys, const vec2 &cursor_position, hud_element_type &type, string &identifier) {
		return nullptr;
	}

	void draw(const shared_ptr<ogl_context> &context, const shared_ptr<ogl_camera> &camera) const;

private:
	//used during spacing of array
	void setXForLine(vector< shared_ptr<hud_element> > &line_contents, float initial_x_offset);
	void setYForLine(vector< shared_ptr<hud_element> > &line_contents, float initial_y_offset,
		int line_number, const map<int, vec2> &line_dimensions);

	//TODO add iterators taht denote "pages" for paging in menus
	vector< shared_ptr<hud_element> > array_elements;
	map <int, vector<shared_ptr<hud_element> > >visible_lines;
	float array_padding;
	pair<horizontal_justification, vertical_justification> justification;

	shared_ptr<ogl_context> context;
};

class artwork_thumbnail : public hud_element
{
public:
	artwork_thumbnail(const shared_ptr<artwork> &art, 
		const shared_ptr<ogl_context> &context, 
		const vec2 centerpoint, 
		float screen_height, float screen_width, float padding = 0.0f)
		: hud_element(centerpoint, screen_height, screen_width, THUMBNAIL)
	{
		stored = art;
		thumbnail_padding = padding;  
		setStored(context);		
	}
	artwork_thumbnail(const shared_ptr<artwork> &art,
		const shared_ptr<ogl_context> &context,
		float on_screen_height, float on_screen_width, float padding = 0.0f)
		: hud_element(vec2(0.0f, 0.0f), on_screen_height, on_screen_width, THUMBNAIL)
	{
		stored = art;
		thumbnail_padding = padding;
		setStored(context);
	}

	~artwork_thumbnail(){};

	void setStored(const shared_ptr<ogl_context> &context);
	shared_ptr<artwork> getStored() const { return stored; }
	void draw(const shared_ptr<ogl_context> &context, const shared_ptr<ogl_camera> &camera) const { 
		stored->draw2D(context, camera, scale_matrix * getTranslationMatrix()); }

	virtual bool isSelected(shared_ptr<key_handler> &keys, const vec2 &cursor_position, shared_ptr<artwork> &selected) const
	{
		if (itemSelected(keys, cursor_position))
		{
			selected = stored;
			return true;
		}

		else return false;
	}

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