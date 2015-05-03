#ifndef LOOT_H
#define LOOT_H

#include "header.h"

class loot_generator
{
public:
	loot_generator(shared_ptr<art_db> database);
	~loot_generator(){};

	vector< shared_ptr<artwork_instance> > generateArtworks(int count,
		const map<rarity, unsigned int> & rarity_proportions) const;
	vector< shared_ptr<artwork_instance> > generateArtworks(int count, float modifier) const;

	void printGenerated(const vector< shared_ptr<artwork_instance> > &drop, const map<rarity, unsigned> &rarity_map) const;

private:
	shared_ptr<art_db> artist_database;
	map<rarity, unsigned> default_rarity_map;
};

#endif