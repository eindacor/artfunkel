#include "player.h"
#include "artwork.h"

player::player(string s, const shared_ptr<loot_generator> &lg, const shared_ptr<ogl_context> ogl_con, 
	shared_ptr<texture_handler> &textures, string data_path)
{
	name = s;

	vector< shared_ptr<artwork> > generated_works = lg->generateArtworks(1, 1.0f);
	for (auto i : generated_works)
		addWorkToInventory(i);

	default_frame = shared_ptr<frame_model>(new frame_model(2.0f, 2.0f, ogl_con, "frame_black.bmp", "white_matte.bmp", textures));
	currency = 5000.0f;
}

void player::addWorkToInventory(const shared_ptr<artwork> &work)
{
	collection_value += work->getValue();

	shared_ptr<artwork> copy_ptr(new artwork(*work));
	inventory.insert(pair<int, shared_ptr<artwork> >(work->getData()->getID(), copy_ptr));
	paintings_not_on_display.push_back(copy_ptr);
}

vector<shared_ptr<artwork> > player::getInventoryCopy()
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