#include "player.h"
#include "artwork.h"
#include "gallery.h"

player::player(string s, const shared_ptr<loot_generator> &lg, const shared_ptr<ogl_context> ogl_con, 
	shared_ptr<texture_handler> &textures, string data_path)
{
	name = s;
	xp = 0;
	level = 0;
	vector< shared_ptr<artwork> > generated_works = lg->generateArtworks(5, 1.0f);
	for (auto i : generated_works)
		addWorkToInventory(i);

	default_frame = shared_ptr<frame_model>(new frame_model(2.0f, 2.0f, ogl_con, "frame_black.bmp", "white_matte.bmp", textures));
	bank = bignum("500000");
	time(&last_balance_check);
}

player::player(string s, const shared_ptr<ogl_context> &context, shared_ptr<texture_handler> &textures, unsigned long player_xp, unsigned short player_level, string balance)
{
	name = s;
	xp = player_xp;
	level = player_level;
	bank = bignum(balance);
	time(&last_balance_check);

	default_frame = shared_ptr<frame_model>(new frame_model(2.0f, 2.0f, context, "frame_black.bmp", "white_matte.bmp", textures));
}

bool player::addWorkToInventory(const shared_ptr<artwork> &work)
{
	if (inventory.size() < inventory_max)
	{
		collection_value += work->getValue();

		shared_ptr<artwork> copy_ptr(new artwork(*work));
		inventory.insert(pair<int, shared_ptr<artwork> >(work->getData()->getID(), copy_ptr));
		paintings_not_on_display.push_back(copy_ptr);
		return true;
	}

	else return false;
}

bool player::removeWorkFromInventory(const shared_ptr<artwork> &work)
{
	if (work != nullptr && !isOnDisplay(work) && alreadyOwned(work))
	{
		collection_value -= work->getValue();
		bank += work->getValue();
		inventory.erase(work->getData()->getID());

		for (vector<shared_ptr<artwork> >::iterator it = paintings_not_on_display.begin(); it != paintings_not_on_display.end(); it++)
		{
			if ((*it)->getData()->getID() == work->getData()->getID())
			{
				paintings_not_on_display.erase(it);
				return true;
			}
		}
	}

	return false;
}

vector<shared_ptr<artwork> > player::getInventoryCopy() const
{
	vector<shared_ptr<artwork> > copied_inventory;
	copied_inventory.reserve(inventory.size());
	for (auto i : inventory)
	{
		//TODO simplify
		shared_ptr<artwork> ptr_copy = i.second;
		artwork copy = *ptr_copy;
		shared_ptr<artwork> copy_ptr(new artwork(copy));
		copied_inventory.push_back(copy_ptr);
	}

	return copied_inventory;
}

vector<shared_ptr<artwork> > player::getDisplayedCopy()
{
	vector<shared_ptr<artwork> > copied_displayed;
	copied_displayed.reserve(paintings_on_display.size());
	for (auto i : paintings_on_display)
	{
		//TODO simplify
		shared_ptr<artwork> ptr_copy = i;
		artwork copy = *ptr_copy;
		shared_ptr<artwork> copy_ptr(new artwork(copy));
		copied_displayed.push_back(copy_ptr);
	}

	return copied_displayed;
}

vector<shared_ptr<artwork> > player::getNotDisplayedCopy()
{
	vector<shared_ptr<artwork> > copied_not_displayed;
	copied_not_displayed.reserve(paintings_not_on_display.size());
	for (auto i : paintings_not_on_display)
	{
		//TODO simplify
		shared_ptr<artwork> ptr_copy = i;
		artwork copy = *ptr_copy;
		shared_ptr<artwork> copy_ptr(new artwork(copy));
		copied_not_displayed.push_back(copy_ptr);
	}

	return copied_not_displayed;
}

//TODO deprecated
void player::addPaintingToDisplay(const shared_ptr<artwork> &to_add)
{
	vector< shared_ptr<artwork> >::iterator found = paintings_not_on_display.end();
	for (vector< shared_ptr<artwork> >::iterator it = paintings_not_on_display.begin(); it != paintings_not_on_display.end(); it++)
	{
		if ((*it)->getData()->getID() == to_add->getData()->getID())
		{
			found = it;
			break;
		}
	}

	if (found != paintings_not_on_display.end())
	{
		paintings_not_on_display.erase(found);
		paintings_on_display.push_back(to_add);
	}
}

void player::removePaintingFromDisplay(const shared_ptr<artwork> &to_remove)
{
	vector< shared_ptr<artwork> >::iterator found = paintings_on_display.end();
	for (vector< shared_ptr<artwork> >::iterator it = paintings_on_display.begin(); it != paintings_on_display.end(); it++)
	{
		if ((*it)->getData()->getID() == to_remove->getData()->getID())
		{
			found = it;
			break;
		}
	}

	if (found != paintings_on_display.end())
	{
		paintings_on_display.erase(found);
		paintings_not_on_display.push_back(to_remove);
	}
}

//TODO change to return an enumerated status
bool player::isOnDisplay(const shared_ptr<artwork> &to_find) const
{
	for (auto i : paintings_on_display)
	{
		if (i->getData()->getID() == to_find->getData()->getID())
			return true;
	}

	return false;
}

bool player::alreadyOwned(const shared_ptr<artwork> &to_find) const
{
	for (auto i : inventory)
	{
		if (i.second->getData()->getID() == to_find->getData()->getID())
			return true;
	}

	return false;
}

shared_ptr<gallery> player::getGallery(int n) const
{
	if (active_galleries.find(n) != active_galleries.end())
		return active_galleries.at(n);

	else return nullptr;
}

void player::addGallery(const shared_ptr<gallery> &to_add)
{
	int counter = 0;
	for (auto i : active_galleries)
	{
		if (i.first == counter)
		{
			counter++;
			continue;
		}

		else
		{
			active_galleries.insert(pair<int, shared_ptr<gallery> >(counter, to_add));
			return;
		}
	}

	active_galleries.insert(pair<int, shared_ptr<gallery> >(counter, to_add));
}

void player::updateBank()
{
	time_t current_time;
	time(&current_time);

	int elapsed_secs = difftime(current_time, last_balance_check);

	cout << "-----------" << endl;
	cout << "previous balance: $" << bank.getNumberString(true, false, 2) << endl;
	cout << "seconds passed: " << elapsed_secs << endl;
	bignum total_gallery_value;
	for (const auto &gallery : active_galleries)
		total_gallery_value += gallery.second->getGalleryValue();

	bignum max_gallery_value_per_sec(".0001");
	bignum min_gallery_value_per_sec(".000001");

	bignum gallery_value_per_sec = max_gallery_value_per_sec;
	bignum money_made;

	//for each hour
	for (int i = 0; i < elapsed_secs / 3600; i++)
	{
		if (gallery_value_per_sec > min_gallery_value_per_sec)
			gallery_value_per_sec *= bignum("0.5");

		else gallery_value_per_sec = min_gallery_value_per_sec;

		money_made += gallery_value_per_sec * bignum(3600) * total_gallery_value;
	}

	int seconds_remaining = elapsed_secs % 3600;
	money_made += gallery_value_per_sec * bignum(seconds_remaining) * total_gallery_value;

	cout << "money made: $" << money_made.getNumberString(true, false, 2) << endl;
	addFunds(money_made);
	cout << "new balance: $" << bank.getNumberString(true, false, 2) << endl;
	time(&last_balance_check);
}