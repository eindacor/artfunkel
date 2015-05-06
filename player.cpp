#include "player.h"
#include "artwork.h"

player::player(string s, const shared_ptr<loot_generator> &lg, shared_ptr<ogl_context> ogl_con, string data_path)
{
	name = s;

	vector<pair<int, shared_ptr<artwork> > > generated_works = lg->generateArtworks(1, 1.0f);
	for (auto i : generated_works)
		addWorkToInventory(i.second);

	string frame_path = data_path + "model_data\\frame_black.bmp";
	string matte_path = data_path + "model_data\\white_matte.bmp";
	default_frame = shared_ptr<frame_model>(new frame_model(2.0f, 2.0f, ogl_con, frame_path, matte_path));
	currency = 5000.0f;
}

void player::addWorkToInventory(const shared_ptr<artwork> &work)
{
	collection_value += work->getValue();

	shared_ptr<artwork> copy_ptr(new artwork(*work));
	inventory.insert(pair<int, shared_ptr<artwork> >(work->getData()->getID(), copy_ptr));
}

vector<pair<int, shared_ptr<artwork> > > player::getInventoryCopy()
{
	vector<pair<int, shared_ptr<artwork> > > copied_inventory;
	for (auto i : inventory)
	{
		//TODO simplify
		shared_ptr<artwork> ptr_copy = i.second;
		artwork copy = *ptr_copy;
		shared_ptr<artwork> copy_ptr(new artwork(copy));
		copied_inventory.push_back(pair<int, shared_ptr<artwork> >(i.first, copy_ptr));
	}

	return copied_inventory;
}

vector<pair<int, shared_ptr<artwork> > > player::getDisplayedCopy()
{
	vector<pair<int, shared_ptr<artwork> > > copied_displayed;
	for (auto i : paintings_on_display)
	{
		//TODO simplify
		shared_ptr<artwork> ptr_copy = i.second;
		artwork copy = *ptr_copy;
		shared_ptr<artwork> copy_ptr(new artwork(copy));
		copied_displayed.push_back(pair<int, shared_ptr<artwork> >(i.first, copy_ptr));
	}

	return copied_displayed;
}

void player::addPaintingToDisplay(const pair<int, shared_ptr<artwork> > &toAdd)
{
	if (paintings_on_display.find(toAdd.first) == paintings_on_display.end())
		paintings_on_display.insert(toAdd);
}

void player::removePaintingFromDisplay(const pair<int, shared_ptr<artwork> > &toRemove)
{
	if (paintings_on_display.find(toRemove.first) != paintings_on_display.end())
		paintings_on_display.erase(toRemove.first);
}

bool player::isOnDisplay(int index)
{
	return (paintings_on_display.find(index) != paintings_on_display.end());
}

bool player::alreadyOwned(int painting_index) const
{
	for (auto i : inventory)
	{
		if (i.second->getData()->getID() == painting_index)
			return true;
	}

	return false;
}