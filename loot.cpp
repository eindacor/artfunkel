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

	bronze_rarity_map[COMMON] = 60;
	bronze_rarity_map[UNCOMMON] = 12;
	//bronze_rarity_map[RARE] = 0;
	
	silver_rarity_map[COMMON] = 24;
	silver_rarity_map[UNCOMMON] = 38;
	silver_rarity_map[RARE] = 2;

	gold_rarity_map[UNCOMMON] = 450;							
	gold_rarity_map[RARE] = 150;
	gold_rarity_map[LEGENDARY] = 1;

	platinum_rarity_map[RARE] = 2400;
	platinum_rarity_map[LEGENDARY] = 10;
	platinum_rarity_map[MASTERPIECE] = 1;

	/*
	bool rare_in_bronze_found = false;
	bool rare_in_silver_found = false;
	bool legendary_in_gold_found = false;
	bool legendary_in_platinum_found = false;
	bool masterpiece_in_platinum_found = false;
	
	jep::avg_container rare_in_bronze_rolls, rare_in_silver_rolls, legendary_in_gold_rolls, 
		legendary_in_platinum_rolls, masterpiece_in_platinum_rolls;

	for (int count = 0; count < 100; count++)
	{
		for (int i = 0; i < 10000; i++)
		{
			if (rare_in_bronze_found && rare_in_silver_found && legendary_in_gold_found &&
				legendary_in_platinum_found && masterpiece_in_platinum_found)
			{
				rare_in_bronze_found = false;
				rare_in_silver_found = false;
				legendary_in_gold_found = false;
				legendary_in_platinum_found = false;
				masterpiece_in_platinum_found = false;
				break;
			}

			if (!rare_in_bronze_found)
			{
				rarity rarity_rolled = jep::catRoll(bronze_rarity_map);
				if (rarity_rolled == RARE)
				{
					rare_in_bronze_found = true;
					rare_in_bronze_rolls.addValue(i);
				}
			}

			if (!rare_in_silver_found)
			{
				rarity rarity_rolled = jep::catRoll(silver_rarity_map);
				if (rarity_rolled == RARE)
				{
					rare_in_silver_found = true;
					rare_in_silver_rolls.addValue(i);
				}
			}

			if (!legendary_in_gold_found)
			{
				rarity rarity_rolled = jep::catRoll(gold_rarity_map);
				if (rarity_rolled == LEGENDARY)
				{
					legendary_in_gold_found = true;
					legendary_in_gold_rolls.addValue(i);
				}
			}

			if (!legendary_in_platinum_found)
			{
				rarity rarity_rolled = jep::catRoll(platinum_rarity_map);
				if (rarity_rolled == LEGENDARY)
				{
					legendary_in_platinum_found = true;
					legendary_in_platinum_rolls.addValue(i);
				}
			}

			if (!masterpiece_in_platinum_found)
			{
				rarity rarity_rolled = jep::catRoll(platinum_rarity_map);
				if (rarity_rolled == MASTERPIECE)
				{
					masterpiece_in_platinum_found = true;
					masterpiece_in_platinum_rolls.addValue(i);
				}
			}
		}
	}
	
	cout << "average rolls for rare in bronze: " << rare_in_bronze_rolls.getAverage() << endl;
	cout << "average rolls for rare in silver: " << rare_in_silver_rolls.getAverage() << endl;
	cout << "average rolls for legendary in gold: " << legendary_in_gold_rolls.getAverage() << endl;
	cout << "average rolls for legendary in platinum: " << legendary_in_platinum_rolls.getAverage() << endl;
	cout << "average rolls for masterpiece in platinum: " << masterpiece_in_platinum_rolls.getAverage() << endl;
	*/

	average_bronze_crate_work_value = calcAveragePaintingValue(bronze_rarity_map);
	average_silver_crate_work_value = calcAveragePaintingValue(silver_rarity_map);
	average_gold_crate_work_value = calcAveragePaintingValue(gold_rarity_map);
	average_platinum_crate_work_value = calcAveragePaintingValue(platinum_rarity_map);
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
	bignum quantity_discount(1);
	
	for (int i = 0; i < count / crate_size_module ; i++)
		quantity_discount *= (bignum)".90";

	bignum result;

	//potential value is determined by average rarity cost, and placement bonus
	bignum potential_value;

	switch (r)
	{
	case COMMON: 
		potential_value = average_bronze_crate_work_value + calcPlacementBonus(average_bronze_crate_work_value);
		result = quantity_discount * potential_value * bignum(count) *  bignum("1.2345");
		break;
	case UNCOMMON: 
		potential_value = average_silver_crate_work_value + calcPlacementBonus(average_silver_crate_work_value);
		result = quantity_discount * potential_value * bignum(count) *  bignum("1.6049");
		break;
	case RARE:
		potential_value = average_gold_crate_work_value + calcPlacementBonus(average_gold_crate_work_value);
		result = quantity_discount * potential_value * bignum(count) *  bignum("1.975");
		break;
	case LEGENDARY:
		potential_value = average_platinum_crate_work_value + calcPlacementBonus(average_platinum_crate_work_value);
		result = quantity_discount * potential_value * bignum(count) *  bignum("2.345");
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

		int random_int = jep::intRoll(0, (rarity_selection.size() - 1));
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
	case COMMON: rarity_proportions = bronze_rarity_map; break;
	case UNCOMMON: rarity_proportions = silver_rarity_map; break;
	case RARE: rarity_proportions = gold_rarity_map; break;
	case LEGENDARY: rarity_proportions = platinum_rarity_map; break;
	}

	vector<shared_ptr<artwork> > loot_vec;
	loot_vec.reserve(count);

	for (int i = 0; i < count; i++)
	{
		rarity random_rarity = jep::catRoll<rarity>(rarity_proportions);
		list< shared_ptr<artwork_data> > rarity_selection = artist_database->getWorksByRarity(random_rarity);

		int random_int = jep::intRoll(0, (rarity_selection.size() - 1));

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