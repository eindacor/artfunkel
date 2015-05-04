#ifndef PLAYER_H
#define PLAYER_H

#include "header.h"
#include "loot.h"

class player
{
public:
	player(string name, const shared_ptr<loot_generator> &lg) { inventory = lg->generateArtworks(10, 1.0f); }
	~player(){};

	void addWorkToInventory(const shared_ptr<artwork_data> &work) { inventory.push_back(work); }
	void setFrame(const shared_ptr<frame_model> &f) { default_frame = f; }
	shared_ptr<frame_model> getDefaultFrame() const { return default_frame; }

	vector< shared_ptr<artwork_instance> > getInventory() { return &inventory; }

private:
	string name;
	vector<artwork_instance> inventory;
	shared_ptr<frame_model> default_frame;

	map<int, gallery> active_galleries;
}

#endif