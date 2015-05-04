#ifndef PLAYER_H
#define PLAYER_H

#include "header.h"
#include "loot.h"

class player
{
public:
	player(string s, const shared_ptr<loot_generator> &lg);
	~player(){};

	void addWorkToInventory(const shared_ptr<artwork_instance> &work) { inventory.push_back(work); }
	void setFrame(const shared_ptr<frame_model> &f) { default_frame = f; }
	shared_ptr<frame_model> getDefaultFrame() const { return default_frame; }

	vector< shared_ptr<artwork_instance> > getInventory() { return inventory; }
	//copied inventory is used when the position will be modified, like in menus
	vector< shared_ptr<artwork_instance> > getInventoryCopy();

private:
	string name;
	vector< shared_ptr<artwork_instance> > inventory;
	shared_ptr<frame_model> default_frame;

	//map<int, gallery> active_galleries;
};

#endif