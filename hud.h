#ifndef HUD_H
#define HUD_H

#include "header.h"
#include "artwork.h"

class hud_item
{
public:
	hud_item(const vec2 centerpoint, float item_height, float item_width) : height(item_height), width(item_width) {};
	~hud_item(){};

	virtual bool itemSelected(shared_ptr<key_handler> &keys, const shared_ptr<ogl_camera> &camera, const vec2 &cursor_position) const;
	virtual void draw(const shared_ptr<ogl_context> &context, const shared_ptr<ogl_camera> &camera) const = 0;
	vec2 getUpperLeft() const { return upper_left; }
	vec2 getUpperRight() const { return upper_right; }
	vec2 getLowerLeft() const { return lower_left; }
	vec2 getLowerRight() const { return lower_right; }
	vec2 getCenterpoint() const { return centerpoint; }
	float getHeight() const { return height; }
	float getWidth() const { return width; }

private:
	//below are the binding points for click detection
	vec2 upper_left, upper_right, lower_left, lower_right;
	float height, width;
	vec2 centerpoint;
};

class artwork_thumbnail : public hud_item
{
public:
	artwork_thumbnail(const shared_ptr<artwork> &art, 
		const shared_ptr<ogl_context> &context, 
		const vec2 centerpoint, 
		float screen_height, float screen_width, float padding = 0.0f)
		: hud_item(centerpoint, screen_height, screen_width)
	{
		thumbnail_padding = padding;  setStored(context);
		stored = art;
	}
	~artwork_thumbnail(){};

	void setStored(const shared_ptr<ogl_context> &context);
	void draw(const shared_ptr<ogl_context> &context, const shared_ptr<ogl_camera> &camera) const { 
		stored->draw2D(context, camera, model_matrix); }

	virtual bool isSelected(shared_ptr<key_handler> &keys, const shared_ptr<ogl_camera> &camera, const vec2 &cursor_position, shared_ptr<artwork> &selected) const
	{
		if (itemSelected(keys, camera, cursor_position)) selected = stored;
	}

private:
	mat4 calcScaleMatrix(const shared_ptr<ogl_context> &context) const;
	mat4 model_matrix;
	shared_ptr<artwork> stored;
	float thumbnail_padding;
};

/*
class text_box : public hud_item
{
public:
	text_box(){};
	~text_box(){};

private:


};

class button : public hud_item
{
public:
	button(){};
	~button(){};

private:


};

class text_field : public hud_item
{
public:
	text_field(){};
	~text_field(){};

private:


};
*/

#endif