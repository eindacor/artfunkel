#include "player.h"
#include "artwork.h"

player::player(string s, const shared_ptr<loot_generator> &lg)
{
	name = s;
	vector< shared_ptr<artwork_instance> > generated_works =  lg->generateArtworks(10, 1.0f);
	for (auto i : generated_works)
		inventory.push_back(i);
}

vector< shared_ptr<artwork_instance> > player::getInventoryCopy()
{
	vector< shared_ptr<artwork_instance> > copied_inventory;
	for (int i = 0; i < inventory.size(); i++)
	{
		string testname = inventory.at(i)->getArtistName();
		shared_ptr<artwork_instance> ptr_copy = inventory.at(i);
		ptr_copy->getFrame();
		artwork_instance copy = *ptr_copy;
		shared_ptr<artwork_instance> copy_ptr(new artwork_instance(copy));
		copied_inventory.push_back(copy_ptr);
		;
	}

	return copied_inventory;
}