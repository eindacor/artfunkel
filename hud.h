#ifndef HUD_H
#define HUD_H

#include "header.h"

class hud_item
{
public:
	hud_item(const vec2 centerpoint, float height, float width){};
	~hud_item(){};

private:
	vec2 upper_left;
	vec2 upper_right;
	vec2 lower_left;
	vec2 lower_right;
	vec2 centerpoint;

};

class artwork_thumbnail : public hud_item
{
public:
	artwork_thumbnail();
	artwork_thumbnail(){};
	~artwork_thumbnail(){};

private:


};

class text_box : public hud_item
{
public:
	text_box(){};
	~text_box(){};

private:


};

#endif