#ifndef PLAYER_H
#define PLAYER_H

#include "header.h"
#include "loot.h"

class player
{
public:
	player(string s, const shared_ptr<loot_generator> &lg, shared_ptr<ogl_context> ogl_con, string data_path);
	~player(){};

	void addWorkToInventory(const shared_ptr<artwork_instance> &work);
	void setFrame(const shared_ptr<frame_model> &f) { default_frame = f; }
	shared_ptr<frame_model> getDefaultFrame() const { return default_frame; }

	map<int, shared_ptr<artwork_instance> > getInventory() { return inventory; }
	//copied inventory is used when the position will be modified, like in menus
	map<int, shared_ptr<artwork_instance> > getInventoryCopy();

	map<int, shared_ptr<artwork_instance> > getDisplayed() { return paintings_on_display; }
	map<int, shared_ptr<artwork_instance> > getDisplayedCopy();

	void addPaintingToDisplay(const pair<int, shared_ptr<artwork_instance> > &toAdd);
	void removePaintingFromDisplay(const pair<int, shared_ptr<artwork_instance> > &toRemove);

	bool isOnDisplay(int index);

	bool alreadyOwned(int painting_index) const;

private:
	string name;
	map<int, shared_ptr<artwork_instance> > inventory;
	shared_ptr<frame_model> default_frame;
	map<int, shared_ptr<artwork_instance> > paintings_on_display;

	float currency;

	//map<int, gallery> active_galleries;
};

#endif