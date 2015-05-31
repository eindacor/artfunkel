#ifndef PLAYER_H
#define PLAYER_H

#include "header.h"
#include "loot.h"

class player
{
public:
	player(string s, const shared_ptr<loot_generator> &lg, const shared_ptr<ogl_context> ogl_con, 
		shared_ptr<texture_handler> &textures, string data_path, unsigned long player_xp = 0, unsigned short player_level = 0);
	~player(){};

	bool addWorkToInventory(const shared_ptr<artwork> &work);
	bool removeWorkFromInventory(const shared_ptr<artwork> &work);
	void setFrame(const shared_ptr<frame_model> &f) { default_frame = f; }
	shared_ptr<frame_model> getDefaultFrame() const { return default_frame; }

	//TODO rework inventory system for placing paintings in gallery

	//map<int, shared_ptr<artwork> > getInventory() { return inventory; }
	//copied inventory is used when the position will be modified, such as menus
	//returns a vector of pairs for sortability
	vector<shared_ptr<artwork> > getInventoryCopy();

	//returns a vector of pairs for sortability
	vector<shared_ptr<artwork> > getDisplayedCopy();
	vector<shared_ptr<artwork> > getNotDisplayedCopy();

	bignum getCollectionValue() const { return collection_value; }

	void addPaintingToDisplay(const shared_ptr<artwork> &to_add);
	void removePaintingFromDisplay(const shared_ptr<artwork> &to_remove);

	bool player::isOnDisplay(const shared_ptr<artwork> &to_find) const;

	bool alreadyOwned(const shared_ptr<artwork> &to_find) const;
	shared_ptr<gallery> getGallery(int n) const;
	void addGallery(const shared_ptr<gallery> &to_add);

	unsigned long getXP() const { return xp; }
	unsigned short getLevel() const { return level; }

	string getBankBalance() const;

private:
	string name;
	//key corresponds to painting ID
	map<int, shared_ptr<artwork> > inventory;
	shared_ptr<frame_model> default_frame;
	vector<shared_ptr<artwork> > paintings_on_display;
	vector<shared_ptr<artwork> > paintings_not_on_display;

	bignum bank;
	bignum collection_value;

	unsigned long xp;
	unsigned short level;

	map<int, shared_ptr<gallery> > active_galleries;
};

#endif