#include "loot.h"
#include "header.h"
#include "artwork.h"

vector< shared_ptr<artwork> > loot_generator::generateArtworks(int count,
	const map<rarity, unsigned int> &rarity_proportions
	) const
{
	vector< shared_ptr<artwork> > loot_vec;

	for (int i = 0; i < count; i++)
	{
		rarity random_rarity= jep::catRoll<rarity>(rarity_proportions);
		list< shared_ptr<artwork> > rarity_selection = artist_database->getWorksByRarity(random_rarity);

		list< shared_ptr<artwork> >::const_iterator it = rarity_selection.begin();
		for (int i = 0; i < jep::intRoll(0, rarity_selection.size()); i++)
			it++;

		loot_vec.push_back(*it);
	}

	return loot_vec;
}