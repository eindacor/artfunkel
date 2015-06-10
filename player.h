#ifndef PLAYER_H
#define PLAYER_H

#include "header.h"
#include "loot.h"

class player
{
public:
	//TODO set last check time immediately
	player(string s, const shared_ptr<loot_generator> &lg, const shared_ptr<ogl_context> ogl_con, 
		shared_ptr<texture_handler> &textures, string data_path);
	player(string s, const shared_ptr<ogl_context> &context, shared_ptr<texture_handler> &textures, unsigned long player_xp, unsigned short player_level, string balance);
	~player(){};

	bool addWorkToInventory(const shared_ptr<artwork> &work);
	bool removeWorkFromInventory(const shared_ptr<artwork> &work);
	void setFrame(const shared_ptr<frame_model> &f) { default_frame = f; }
	shared_ptr<frame_model> getDefaultFrame() const { return default_frame; }

	//TODO rework inventory system for placing paintings in gallery

	//map<int, shared_ptr<artwork> > getInventory() { return inventory; }
	//copied inventory is used when the position will be modified, such as menus
	//returns a vector of pairs for sortability
	vector<shared_ptr<artwork> > getInventoryCopy() const;
	const map<unsigned int, shared_ptr<artwork> > getInventory() const { return inventory; }

	//returns a vector of pairs for sortability
	vector<shared_ptr<artwork> > getDisplayedCopy();
	vector<shared_ptr<artwork> > getNotDisplayedCopy();

	bignum getCollectionValue() const { return collection_value; }

	void addPaintingToDisplay(const shared_ptr<artwork> &to_add);
	void removePaintingFromDisplay(const shared_ptr<artwork> &to_remove);

	bool player::isOnDisplay(const shared_ptr<artwork> &to_find) const;

	bool inventoryIsFull() const { return inventory.size() >= inventory_max; }
	bool alreadyOwned(const shared_ptr<artwork> &to_find) const;
	shared_ptr<gallery> getGallery(int n) const;
	const map<int, shared_ptr<gallery> > getGalleries() const { return active_galleries; }
	void addGallery(const shared_ptr<gallery> &to_add);

	unsigned long getXP() const { return xp; }
	unsigned short getLevel() const { return level; }
	string getName() const { return name; }

	bignum getBankBalance() const { return bank; }
	string getBankBalanceString(bool commas) const { return bank.getNumberString(commas, false, 2); }
	void deductPayment(const bignum &bn) { bank -= bn; }
	void addFunds(const bignum &bn) { bank += bn; }

	void updateBank();
	void setLastBalanceCheck(time_t t) { last_balance_check = t; }
	time_t getLastBalanceCheck() const { return last_balance_check; }

	shared_ptr<artwork> getPaintingFromInventory(unsigned int artwork_id) {
		if (inventory.find(artwork_id) != inventory.end())
			return inventory.at(artwork_id);

		else return nullptr;
	}

	map<string, string> getAvailableWallFinishes() const { return available_wall_finishes; }
	void addAvailableWallFinish(string finish_name, string finish_filename) { available_wall_finishes[finish_name] = finish_filename; }

private:
	string name;
	//key corresponds to painting ID
	map<unsigned int, shared_ptr<artwork> > inventory;
	shared_ptr<frame_model> default_frame;
	vector<shared_ptr<artwork> > paintings_on_display;
	vector<shared_ptr<artwork> > paintings_not_on_display;

	bignum bank;
	bignum collection_value;

	time_t last_balance_check;

	int inventory_max = 64;

	unsigned long xp;
	unsigned short level;

	map<int, shared_ptr<gallery> > active_galleries;

	//texture name, texture filename
	map<string, string> available_wall_finishes;
};

#endif