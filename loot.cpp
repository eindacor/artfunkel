#include "loot.h"
#include "header.h"
#include "artwork.h"
#include "art_db.h"

loot_generator::loot_generator(shared_ptr<art_db> database)
{
	artist_database = database;

	//create a map of rarities, with proportions, to pass to jep::catRoll
	default_rarity_map[COMMON] = 2048;
	default_rarity_map[UNCOMMON] = 1024;
	default_rarity_map[RARE] = 256;
	default_rarity_map[LEGENDARY] = 16;
	default_rarity_map[MASTERPIECE] = 1;
}

map<int, shared_ptr<artwork_instance> > loot_generator::generateArtworks(int count,
	const map<rarity, unsigned int> &rarity_proportions
	) const
{
	map<int, shared_ptr<artwork_instance> > loot_map;

	for (int i = 0; i < count; i++)
	{
		rarity random_rarity= jep::catRoll<rarity>(rarity_proportions);
		list< shared_ptr<artwork_data> > rarity_selection = artist_database->getWorksByRarity(random_rarity);

		list< shared_ptr<artwork_data> >::const_iterator it = rarity_selection.begin();
		for (int i = 0; i < jep::intRoll(0, (rarity_selection.size() - 1)); i++)
			it++;

		shared_ptr<artwork_instance> toAdd(new artwork_instance((*it)->getID(), (*it)->getTitle(), (*it)->getArtist(), (*it)->getGenre(), (*it)->getRarity(),
			false, 1.0f, (*it)->getHeight(), (*it)->getWidth(), (*it)->getImagePath(), (*it)->getDate(), (*it)->getBaseValue()));
		loot_map.insert(pair< int, shared_ptr<artwork_instance> >(i, toAdd));
	}

	printGenerated(loot_map, rarity_proportions);
	return loot_map;
}

map<int, shared_ptr<artwork_instance> > loot_generator::generateArtworks(int count, float modifier) const
{
	//add functionality to pass a modifier, increasing/decreasing chances
	map<int, shared_ptr<artwork_instance> > loot_map;

	for (int i = 0; i < count; i++)
	{
		rarity random_rarity = jep::catRoll<rarity>(default_rarity_map);
		list< shared_ptr<artwork_data> > rarity_selection = artist_database->getWorksByRarity(random_rarity);

		int random_roll = jep::intRoll(0, (rarity_selection.size() - 1));
		list< shared_ptr<artwork_data> >::const_iterator it = rarity_selection.begin();
		for (int i = 0; i < random_roll; i++)
			it++;

		shared_ptr<artwork_instance> toAdd(new artwork_instance((*it)->getID(), (*it)->getTitle(), (*it)->getArtist(), (*it)->getGenre(), (*it)->getRarity(),
			false, 1.0f, (*it)->getHeight(), (*it)->getWidth(), (*it)->getImagePath(), (*it)->getDate(), (*it)->getBaseValue()));
		loot_map.insert(pair< int, shared_ptr<artwork_instance> >(i, toAdd));
	}

	printGenerated(loot_map, default_rarity_map);
	return loot_map;
}

void loot_generator::printGenerated(const map<int, shared_ptr<artwork_instance> > &artwork_map, const map<rarity, unsigned> &rarity_map) const
{
	int rarity_sumtotal = 0;
	for (auto i : rarity_map)
		rarity_sumtotal += i.second;

	cout << "Drop chance: " << endl;
	for (auto i : rarity_map)
		cout << "\t" << stringFromRarity(i.first) << ": " << 100.0f * ((float)i.second / (float)rarity_sumtotal) << "%" << endl;

	cout << "Crate Contents: " << endl;
	for (auto i : artwork_map)
	{
		cout << "\t" << i.second->getTitle() << " by " << i.second->getArtistName() <<
			" (" << stringFromRarity(i.second->getRarity()) << ")" << endl;
		cout << "\t\tEstimated value: $" << i.second->getValue().getNumberString(true, false, 2) << endl;
	}

	map<rarity, unsigned> frequency_results{
		pair<rarity, int>(COMMON, 0),
		pair<rarity, int>(UNCOMMON, 0),
		pair<rarity, int>(RARE, 0),
		pair<rarity, int>(LEGENDARY, 0),
		pair<rarity, int>(MASTERPIECE, 0)
	};
	for (auto i : artwork_map)
		frequency_results[i.second->getRarity()]++;

	cout << "Frequency results..." << endl;
	for (auto i : frequency_results)
		cout << "\t" << stringFromRarity(i.first) << ": " << i.second << endl;
}