#ifndef LOOT_H
#define LOOT_H

#include "header.h"

class loot_generator
{
public:
	loot_generator(shared_ptr<art_db> database);
	~loot_generator(){};

	//functions generate instances instead of classes to add random conditions and traits
	vector<shared_ptr<artwork> > generateArtworks(int count,
		const map<rarity, unsigned int> & rarity_proportions) const;
	vector<shared_ptr<artwork> > generateArtworks(int count, float modifier) const;
	vector<shared_ptr<artwork> > generateArtworks(int count, rarity r) const;

	void printGenerated(vector<shared_ptr<artwork> > &drop, const map<rarity, unsigned> &rarity_map) const;

	bignum calcAveragePaintingValue(const map<rarity, unsigned> &rarity_map) const;

	bignum getCrateCost(rarity r, int count) const;

	bignum calcPlacementBonus(const bignum &value) const { return value * bignum(".3"); }

private:
	shared_ptr<art_db> artist_database;
	map<rarity, unsigned> default_rarity_map;

	map<rarity, unsigned> common_rarity_map;
	map<rarity, unsigned> uncommon_rarity_map;
	map<rarity, unsigned> rare_rarity_map;
	map<rarity, unsigned> legendary_rarity_map;

	bignum average_common_crate_work_value;
	bignum average_uncommon_crate_work_value;
	bignum average_rare_crate_work_value;
	bignum average_legendary_crate_work_value;
};

#endif