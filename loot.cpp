#include "loot.h"

vector< shared_ptr<artwork> > loot_generator::generateArtworks(int count,
	map<rarity, int> rarity_proportions,
	map<genre, int> genre_proportions //,
	//map<string, int> artist_proportions
	) const
{
	vector< shared_ptr<artwork> > loot_vec;

	for (int i = 0; i < count; i++)
	{
		genre random_genre = jep::catRoll<genre>(genre_proportions);
	}
	
}