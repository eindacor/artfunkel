#include "loot.h"
#include "header.h"
#include "artwork.h"

vector< shared_ptr<artwork_instance> > loot_generator::generateArtworks(int count,
	const map<rarity, unsigned int> &rarity_proportions
	) const
{
	vector< shared_ptr<artwork_instance> > loot_vec;

	for (int i = 0; i < count; i++)
	{
		rarity random_rarity= jep::catRoll<rarity>(rarity_proportions);
		list< shared_ptr<artwork_data> > rarity_selection = artist_database->getWorksByRarity(random_rarity);

		list< shared_ptr<artwork_data> >::const_iterator it = rarity_selection.begin();
		for (int i = 0; i < jep::intRoll(0, rarity_selection.size() - 1); i++)
			it++;

		shared_ptr<artwork_instance> toAdd(new artwork_instance((*it)->getID(), (*it)->getTitle(), (*it)->getArtist(), (*it)->getGenre(), (*it)->getRarity(),
			false, 1.0f, (*it)->getHeight(), (*it)->getWidth(), (*it)->getImagePath(), (*it)->getDate()));
		loot_vec.push_back(toAdd);
	}

	return loot_vec;
}