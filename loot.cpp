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

	common_rarity_map[COMMON] = 50;
	common_rarity_map[UNCOMMON] = 13;
	common_rarity_map[RARE] = 1;
	
	uncommon_rarity_map[COMMON] = 30;
	uncommon_rarity_map[UNCOMMON] = 30;
	uncommon_rarity_map[RARE] = 2;

	rare_rarity_map[UNCOMMON] = 100;							
	rare_rarity_map[RARE] = 27;								
	rare_rarity_map[LEGENDARY] = 1;						

	legendary_rarity_map[RARE] = 1000;
	legendary_rarity_map[LEGENDARY] = 23;
	legendary_rarity_map[MASTERPIECE] = 1;

	average_common_crate_work_value = calcAveragePaintingValue(common_rarity_map);
	average_uncommon_crate_work_value = calcAveragePaintingValue(uncommon_rarity_map);
	average_rare_crate_work_value = calcAveragePaintingValue(rare_rarity_map);
	average_legendary_crate_work_value = calcAveragePaintingValue(legendary_rarity_map);
	
	cout << "average work value, common crate: $" << average_common_crate_work_value.getNumberString(true, false, 2) << endl;
	cout << "average work value, uncommon crate: $" << average_uncommon_crate_work_value.getNumberString(true, false, 2) << endl;
	cout << "average work value, rare crate: $" << average_rare_crate_work_value.getNumberString(true, false, 2) << endl;
	cout << "average work value, legendary crate: $" << average_legendary_crate_work_value.getNumberString(true, false, 2) << endl;
}

bignum loot_generator::calcAveragePaintingValue(const map<rarity, unsigned> &rarity_map) const
{
	unsigned map_proportion_total = 0;
	for (const auto &r : rarity_map)
		map_proportion_total += r.second;

	bignum map_total;
	for (const auto &r : rarity_map)
	{
		bignum min_value = lookupValue(r.first, 0.0f);
		bignum max_value = lookupValue(r.first, 1.0f);
		bignum sumtotal = min_value + max_value;
		bignum average = sumtotal / 2;

		float proportion_to_total = (float)r.second / (float)map_proportion_total;

		map_total += average * (bignum)proportion_to_total;
	}

	return map_total;
}

bignum loot_generator::getCrateCost(rarity r, int count) const
{
	bignum cost_modifier(1);
	
	for (int i = 0; i < count / 5 ; i++)
		cost_modifier *= (bignum)".90";

	bignum result;

	//potential value is determined by average rarity cost, and placement bonus
	bignum potential_value;

	switch (r)
	{
	case COMMON: 
		potential_value = average_common_crate_work_value + calcPlacementBonus(average_common_crate_work_value);
		result = cost_modifier * potential_value * bignum(count) *  bignum(".8"); 
		break;
	case UNCOMMON: 
		potential_value = average_uncommon_crate_work_value + calcPlacementBonus(average_uncommon_crate_work_value);
		result = cost_modifier * potential_value * bignum(count) *  bignum("1.2");
		break;
	case RARE:
		potential_value = average_rare_crate_work_value + calcPlacementBonus(average_rare_crate_work_value);
		result = cost_modifier * potential_value * bignum(count) *  bignum("1.6");
		break;
	case LEGENDARY:
		potential_value = average_legendary_crate_work_value + calcPlacementBonus(average_legendary_crate_work_value);
		result = cost_modifier * potential_value * bignum(count) *  bignum("2.0");
		break;
	default: break;
	}

	result.roundDownToIndex(ONES_PLACE + 3);
	return result;
}

vector<shared_ptr<artwork> > loot_generator::generateArtworks(int count,
	const map<rarity, unsigned int> &rarity_proportions
	) const
{
	vector<shared_ptr<artwork> > loot_vec;
	loot_vec.reserve(count);

	for (int i = 0; i < count; i++)
	{
		rarity random_rarity= jep::catRoll<rarity>(rarity_proportions);
		list< shared_ptr<artwork_data> > rarity_selection = artist_database->getWorksByRarity(random_rarity);
		cout << "possible " << stringFromRarity(random_rarity) << " paintings: " << rarity_selection.size() << endl;

		int random_int = jep::intRoll(0, (rarity_selection.size() - 1));
		cout << "random roll: " << random_int << endl;

		list< shared_ptr<artwork_data> >::const_iterator it = rarity_selection.begin();
		for (int i = 0; i < random_int; i++)
			it++;

		shared_ptr<artwork> toAdd(new artwork(*it, false, 1.0f));
		loot_vec.push_back(toAdd);
	}

	return loot_vec;
}

vector<shared_ptr<artwork> > loot_generator::generateArtworks(int count, float modifier) const
{
	//add functionality to pass a modifier, increasing/decreasing chances
	vector<shared_ptr<artwork> > loot_vec;

	for (int i = 0; i < count; i++)
	{
		rarity random_rarity = jep::catRoll<rarity>(default_rarity_map);
		list< shared_ptr<artwork_data> > rarity_selection = artist_database->getWorksByRarity(random_rarity);

		int random_roll = jep::intRoll(0, (rarity_selection.size() - 1));
		list< shared_ptr<artwork_data> >::const_iterator it = rarity_selection.begin();
		for (int i = 0; i < random_roll; i++)
			it++;

		shared_ptr<artwork> toAdd(new artwork(*it, false, 1.0f));
		loot_vec.push_back(toAdd);
	}

	printGenerated(loot_vec, default_rarity_map);
	return loot_vec;
}

vector<shared_ptr<artwork> > loot_generator::generateArtworks(int count, rarity r) const
{
	map<rarity, unsigned int> rarity_proportions;

	switch (r)
	{
	case COMMON: rarity_proportions = common_rarity_map; break;
	case UNCOMMON: rarity_proportions = uncommon_rarity_map; break;
	case RARE: rarity_proportions = rare_rarity_map; break;
	case LEGENDARY: rarity_proportions = legendary_rarity_map; break;
	}

	vector<shared_ptr<artwork> > loot_vec;
	loot_vec.reserve(count);

	for (int i = 0; i < count; i++)
	{
		rarity random_rarity = jep::catRoll<rarity>(rarity_proportions);
		list< shared_ptr<artwork_data> > rarity_selection = artist_database->getWorksByRarity(random_rarity);
		cout << "possible " << stringFromRarity(random_rarity) << " paintings: " << rarity_selection.size() << endl;

		int random_int = jep::intRoll(0, (rarity_selection.size() - 1));
		cout << "random roll: " << random_int << endl;

		list< shared_ptr<artwork_data> >::const_iterator it = rarity_selection.begin();
		for (int i = 0; i < random_int; i++)
			it++;

		shared_ptr<artwork> toAdd(new artwork(*it, false, 1.0f));
		loot_vec.push_back(toAdd);

		//disabled when not testing
		if (i == 0 && false)
		{
			//index, times rolled
			map<int, int> random_rolls;
			for (int j = 0; j < rarity_selection.size(); j++)
				random_rolls[j] = 0;

			for (int j = 0; j < 10000; j++)
			{
				int test_random_int = jep::intRoll(0, (rarity_selection.size() - 1));
				random_rolls[test_random_int] += 1;
			}

			for (int j = 0; j < random_rolls.size(); j++)
				cout << j << ": " << random_rolls[j] << endl;
		}
	}

	return loot_vec;
}

void loot_generator::printGenerated(vector<shared_ptr<artwork> > &art_vec, const map<rarity, unsigned> &rarity_map) const
{
	int rarity_sumtotal = 0;
	for (const auto &i : rarity_map)
		rarity_sumtotal += i.second;

	cout << "Drop chance: " << endl;
	for (const auto &i : rarity_map)
		cout << "\t" << stringFromRarity(i.first) << ": " << 100.0f * ((float)i.second / (float)rarity_sumtotal) << "%" << endl;

	map<rarity, unsigned> frequency_results{
		pair<rarity, int>(COMMON, 0),
		pair<rarity, int>(UNCOMMON, 0),
		pair<rarity, int>(RARE, 0),
		pair<rarity, int>(LEGENDARY, 0),
		pair<rarity, int>(MASTERPIECE, 0)
	};
	for (const auto &i : art_vec)
		frequency_results[i->getData()->getRarity()]++;

	cout << "Frequency results..." << endl;
	for (const auto &i : frequency_results)
		cout << "\t" << stringFromRarity(i.first) << ": " << i.second << endl;

	cout << "Crate Contents: " << endl;
	for (const auto &i : art_vec)
		printArtwork(i);
}