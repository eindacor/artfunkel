#ifndef PLAYER_H
#define PLAYER_H

#include "header.h"
#include "loot.h"

class player
{
public:
	player(string s, const shared_ptr<loot_generator> &lg, shared_ptr<ogl_context> ogl_con, string data_path);
	~player(){};

	void addWorkToInventory(const shared_ptr<artwork> &work);
	void setFrame(const shared_ptr<frame_model> &f) { default_frame = f; }
	shared_ptr<frame_model> getDefaultFrame() const { return default_frame; }

	map<int, shared_ptr<artwork> > getInventory() { return inventory; }
	//copied inventory is used when the position will be modified, such as menus
	//returns a vector of pairs for sortability
	vector<pair<int, shared_ptr<artwork> > > getInventoryCopy();

	map<int, shared_ptr<artwork> > getDisplayed() { return paintings_on_display; }
	//returns a vector of pairs for sortability
	vector<pair<int, shared_ptr<artwork> > > getDisplayedCopy();

	bignum getCollectionValue() const { return collection_value; }

	void addPaintingToDisplay(const pair<int, shared_ptr<artwork> > &toAdd);
	void removePaintingFromDisplay(const pair<int, shared_ptr<artwork> > &toRemove);

	bool isOnDisplay(int index);

	bool alreadyOwned(int painting_index) const;

private:
	string name;
	//key corresponds to painting ID
	map<int, shared_ptr<artwork> > inventory;
	shared_ptr<frame_model> default_frame;
	map<int, shared_ptr<artwork> > paintings_on_display;

	bignum currency;
	bignum collection_value;

	//map<int, gallery> active_galleries;
};

#endif