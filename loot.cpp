#include "loot.h"
#include "header.h"
#include "artwork.h"
#include "art_db.h"
#include "utility_funcs.h"

loot_generator::loot_generator(shared_ptr<art_db> database)
{
	artist_database = database;

	bool even_odds = false;

	if (even_odds)
	{
		default_rarity_map[COMMON] = 1;
		default_rarity_map[UNCOMMON] = 1;
		default_rarity_map[RARE] = 1;
		default_rarity_map[LEGENDARY] = 1;
		default_rarity_map[MASTERPIECE] = 1;
	}

	else
	{
		//create a map of rarities, with proportions, to pass to jep::catRoll
		default_rarity_map[COMMON] = 2048;
		default_rarity_map[UNCOMMON] = 1024;
		default_rarity_map[RARE] = 256;
		default_rarity_map[LEGENDARY] = 16;
		default_rarity_map[MASTERPIECE] = 1;
	}

	
}

vector<pair<int, shared_ptr<artwork> > > loot_generator::generateArtworks(int count,
	const map<rarity, unsigned int> &rarity_proportions
	) const
{
	vector<pair<int, shared_ptr<artwork> > > loot_vec;
	loot_vec.reserve(count);

	for (int i = 0; i < count; i++)
	{
		rarity random_rarity= jep::catRoll<rarity>(rarity_proportions);
		list< shared_ptr<artwork_data> > rarity_selection = artist_database->getWorksByRarity(random_rarity);

		list< shared_ptr<artwork_data> >::const_iterator it = rarity_selection.begin();
		for (int i = 0; i < jep::intRoll(0, (rarity_selection.size() - 1)); i++)
			it++;

		shared_ptr<artwork> toAdd(new artwork(*it, false, 1.0f));
		loot_vec.push_back(pair< int, shared_ptr<artwork> >(i, toAdd));
	}

	printGenerated(loot_vec, rarity_proportions);
	return loot_vec;
}

vector<pair<int, shared_ptr<artwork> > > loot_generator::generateArtworks(int count, float modifier) const
{
	//add functionality to pass a modifier, increasing/decreasing chances
	vector<pair<int, shared_ptr<artwork> > > loot_vec;

	for (int i = 0; i < count; i++)
	{
		rarity random_rarity = jep::catRoll<rarity>(default_rarity_map);
		list< shared_ptr<artwork_data> > rarity_selection = artist_database->getWorksByRarity(random_rarity);

		int random_roll = jep::intRoll(0, (rarity_selection.size() - 1));
		list< shared_ptr<artwork_data> >::const_iterator it = rarity_selection.begin();
		for (int i = 0; i < random_roll; i++)
			it++;

		shared_ptr<artwork> toAdd(new artwork(*it, false, 1.0f));
		loot_vec.push_back(pair< int, shared_ptr<artwork> >(i, toAdd));
	}

	printGenerated(loot_vec, default_rarity_map);
	return loot_vec;
}

void loot_generator::printGenerated(vector<pair<int, shared_ptr<artwork> > > &art_vec, const map<rarity, unsigned> &rarity_map) const
{
	int rarity_sumtotal = 0;
	for (auto i : rarity_map)
		rarity_sumtotal += i.second;

	cout << "Drop chance: " << endl;
	for (auto i : rarity_map)
		cout << "\t" << stringFromRarity(i.first) << ": " << 100.0f * ((float)i.second / (float)rarity_sumtotal) << "%" << endl;

	map<rarity, unsigned> frequency_results{
		pair<rarity, int>(COMMON, 0),
		pair<rarity, int>(UNCOMMON, 0),
		pair<rarity, int>(RARE, 0),
		pair<rarity, int>(LEGENDARY, 0),
		pair<rarity, int>(MASTERPIECE, 0)
	};
	for (auto i : art_vec)
		frequency_results[i.second->getData()->getRarity()]++;

	cout << "Frequency results..." << endl;
	for (auto i : frequency_results)
		cout << "\t" << stringFromRarity(i.first) << ": " << i.second << endl;

	cout << "Crate Contents: " << endl;
	for (auto i : art_vec)
		printArtwork(i.second);
}