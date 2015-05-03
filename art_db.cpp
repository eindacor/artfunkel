#include "art_db.h"
#include "artist.h"
#include "artwork.h"
#include "utility_funcs.h"
#include "header.h"

art_db::art_db(const char* artists_path, 
			const char* paintings_path, 
			const char* image_directory)
{
	jep::csv_file paintings_file(paintings_path);
	jep::csv_file artists_file(artists_path);

	for (int i = 1; i < artists_file.getRowCount(); i++)
	{
		vector<string> row = artists_file.getRow(i);

		int artist_ID(std::stoi(row.at(0)));
		string artist_name(row.at(1));
		date birth_date(row.at(2));
		date death_date(row.at(3));

		shared_ptr<artist> new_artist(new artist(artist_ID, artist_name, birth_date, death_date));
		artists.insert(pair<string, shared_ptr<artist> >(artist_name, new_artist));
	}

	for (int i = 0; i < (int)UNKNOWN_GENRE; i++)
		genre_counts[(genre)i] = 0;

	for (int i = 0; i < (int)UNKNOWN_RARITY; i++)
		rarity_counts[(rarity)i] = 0;

	for (int i = 1; i < paintings_file.getRowCount(); i++)
	{
		vector<string> row = paintings_file.getRow(i);
		int work_id(std::stoi(row.at(0)));
		shared_ptr<artist> work_artist = lookupArtistByName(row.at(1));
		string work_title(row.at(2));
		date work_date(row.at(3));
		genre work_genre(genreFromString(row.at(4)));
		rarity work_rarity(rarityFromString(row.at(5)));
		//reserved for medium
		//reserved for rarity scale
		float work_height(std::stof(row.at(8)));
		float work_width(std::stof(row.at(9)));
		string work_image_name(row.at(10));
		bool work_forgery = false;
		float work_condition = 1.0f;

		string image_path = image_directory + work_image_name;

		shared_ptr<artwork_data> new_data(new artwork_data(work_id, work_title, work_artist, work_genre, work_rarity,
			work_height, work_width, image_path, work_date));

		artworks.insert(pair<int, shared_ptr<artwork_data> >(work_id, new_data));
		//printArtwork(*new_data);
		genre_counts[work_genre] += 1;
		rarity_counts[work_rarity] += 1;
	}
	printCounts();
}

void art_db::printArtist(shared_ptr<artist> target) const
{
	cout << target->getID() << ": " << target->getName() << " (" << target->getBirthDateString(false) 
		<< " - " << target->getDeathDateString(false) << ")" << endl;
}

void art_db::printArtwork(const artwork_data &target) const
{
	cout << target.getID() << ": " << target.getTitle() << " by " << target.getArtistName() << endl;
	cout << "\t" << target.getHeight() << "x" << target.getWidth() << ", " << getDateString(target.getDate(), false) << ", $" << target.getValue() << "k" << endl;
	cout << "Rarity: " << target.getRarity() << endl;
}

void art_db::printCounts() const
{
	cout << "Genre summary: " << endl;
	for (auto i : genre_counts)
		cout << "\t" << stringFromGenre(i.first) << ": " << i.second << endl;

	cout << "Rarity summary: " << endl;
	for (auto i : rarity_counts)
		cout << "\t" << stringFromRarity(i.first) << ": " << i.second << endl;
}

list< shared_ptr<artwork_data> > art_db::getWorksByGenre(genre g, bool match) const
{
	list< shared_ptr<artwork_data> > found;
	std::for_each(artworks.cbegin(), artworks.cend(),
		[&](const pair<int, shared_ptr<artwork_data> > &art) {
		if ((art.second->getGenre() == g) == match) found.push_back(art.second); });

	return found;
}

list < shared_ptr<artwork_data> > art_db::getWorksByRarity(rarity r, bool match) const
{
	list< shared_ptr<artwork_data> > found;
	std::for_each(artworks.cbegin(), artworks.cend(),
		[&](const pair<int, shared_ptr<artwork_data> > &art) {
		if ((art.second->getRarity() == r) == match) found.push_back(art.second); });

	return found;
}

list < shared_ptr<artwork_data> > art_db::getWorksByArtist(string name, bool match) const
{
	list< shared_ptr<artwork_data> > found;
	std::for_each(artworks.cbegin(), artworks.cend(),
		[&](const pair<int, shared_ptr<artwork_data> > &art) {
		if ((art.second->getArtistName() == name) == match) found.push_back(art.second); });

	return found;
}