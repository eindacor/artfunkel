#ifndef LOOT_H
#define LOOT_H

#include "header.h"
#include "art_db.h"

map<rarity, int> getDefaultRarityProportions()
{
	map<rarity, int> proportions;

	for (int i = 0; i < UNKNOWN_RARITY; i++)
		proportions[(rarity)i] = 1;

	return proportions;
}

map<genre, int> getDefaultGenreProportions()
{
	map<genre, int> proportions;

	for (int i = 0; i < UNKNOWN_GENRE; i++)
		proportions[(genre)i] = 1;
}

//TODO make function generate proper proportions
map<string, int> getDefaultArtistProportions(shared_ptr<art_db> database)
{
	map<string, int> proportions;

	return proportions;
}

class loot_generator
{
public:
	loot_generator(shared_ptr<art_db> database){ artist_database = database; }
	~loot_generator(){};

	vector< shared_ptr<artwork> > generateArtworks(int count,
		map<rarity, int> rarity_proportions = getDefaultRarityProportions(),
		map<genre, int> genre_proportions = getDefaultGenreProportions() //,
		//add artist proportions
		//map<string, int> artist_proportions = getDefaultArtistProportions(database)
		) const;

private:
	shared_ptr<art_db> artist_database;
};

#endif