#ifndef LOOT_H
#define LOOT_H

#include "header.h"
#include "art_db.h"

class loot_generator
{
public:
	loot_generator(shared_ptr<art_db> database){ artist_database = database; }
	~loot_generator(){};

	vector< shared_ptr<artwork_instance> > generateArtworks(int count,
		const map<rarity, unsigned int> &rarity_proportions) const;

private:
	shared_ptr<art_db> artist_database;
};

#endif