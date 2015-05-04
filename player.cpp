#include "player.h"
#include "artwork.h"

player::player(string s, const shared_ptr<loot_generator> &lg, shared_ptr<ogl_context> ogl_con, string data_path)
{
	name = s;
	vector< shared_ptr<artwork_instance> > generated_works =  lg->generateArtworks(10, 1.0f);
	for (auto i : generated_works)
		addWorkToInventory(i);

	string frame_path = data_path + "model_data\\frame_black.bmp";
	string matte_path = data_path + "model_data\\white_matte.bmp";
	default_frame = shared_ptr<frame_model>(new frame_model(2.0f, 2.0f, ogl_con, frame_path, matte_path));
}

void player::addWorkToInventory(const shared_ptr<artwork_instance> &work)
{
	int index = 0;
	for (auto i : inventory)
	{
		if (i.first != index)
			break;

		index++;
	}

	shared_ptr<artwork_instance> copy_ptr(new artwork_instance(*work));
	inventory.insert(pair<int, shared_ptr<artwork_instance> >(index, copy_ptr));
}

map<int, shared_ptr<artwork_instance> > player::getInventoryCopy()
{
	map<int, shared_ptr<artwork_instance> > copied_inventory;
	for (auto i : inventory)
	{
		shared_ptr<artwork_instance> ptr_copy = i.second;
		artwork_instance copy = *ptr_copy;
		shared_ptr<artwork_instance> copy_ptr(new artwork_instance(copy));
		copied_inventory[i.first] = (copy_ptr);
	}

	return copied_inventory;
}

map<int, shared_ptr<artwork_instance> > player::getDisplayedCopy()
{
	map<int, shared_ptr<artwork_instance> > copied_displayed;
	for (auto i : paintings_on_display)
	{
		shared_ptr<artwork_instance> ptr_copy = i.second;
		artwork_instance copy = *ptr_copy;
		shared_ptr<artwork_instance> copy_ptr(new artwork_instance(copy));
		copied_displayed[i.first] = (copy_ptr);
	}

	return copied_displayed;
}

void player::addPaintingToDisplay(const pair<int, shared_ptr<artwork_instance> > &toAdd)
{
	if (paintings_on_display.find(toAdd.first) == paintings_on_display.end())
		paintings_on_display.insert(toAdd);
}

void player::removePaintingFromDisplay(const pair<int, shared_ptr<artwork_instance> > &toRemove)
{
	if (paintings_on_display.find(toRemove.first) != paintings_on_display.end())
		paintings_on_display.erase(toRemove.first);
}

bool player::isOnDisplay(int index)
{
	return (paintings_on_display.find(index) != paintings_on_display.end());
}