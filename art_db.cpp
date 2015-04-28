#include "art_db.h"
#include "artist.h"
#include "artwork.h"
#include "utility_funcs.h"

art_db::art_db(const char* artists_path, 
			const char* paintings_path, 
			const char* image_directory, 
			shared_ptr<ogl_context> context, 
			shared_ptr<ogl_camera> camera)
{
	jep::csv_file paintings_file(paintings_path);
	jep::csv_file artists_file(artists_path);

	for (int i = 1; i < artists_file.getRowCount(); i++)
	{
		vector<string> row = artists_file.getRow(i);

		int artist_ID(std::stoi(row.at(0)));
		string artist_name(row.at(1));
		jep::date birth_date(row.at(2));
		jep::date death_date(row.at(3));

		shared_ptr<artist> new_artist(new artist(artist_ID, artist_name, birth_date, death_date));
		artists.insert(std::pair<string, shared_ptr<artist> >(artist_name, new_artist));
	}

	for (int i = 1; i < paintings_file.getRowCount(); i++)
	{
		vector<string> row = paintings_file.getRow(i);

		int work_id(std::stoi(row.at(0)));
		shared_ptr<artist> work_artist = lookupArtistByName(row.at(1));
		string work_title(row.at(2));
		jep::date work_date(row.at(3));
		genre work_genre(genreFromString(row.at(4)));
		rarity work_rarity(rarityFromString(row.at(5)));
		//reserved for medium
		//reserved for rarity scale
		float work_height(std::stof(row.at(8)));
		float work_width(std::stof(row.at(9)));
		string work_image_name(row.at(10));
		bool work_forgery = false;
		float work_condition = 1.0f;

		string texture_path = image_directory + work_image_name;

		shared_ptr<painting_surface> work_surface(new painting_surface(work_width, work_height, context, camera, texture_path.c_str()));

		shared_ptr<artwork> new_artwork(new artwork(work_id, work_title, work_artist, work_genre, work_rarity, 
			work_forgery, work_condition, work_height, work_width, work_image_name, work_date, work_surface));

		artworks.insert(std::pair<int, shared_ptr<artwork> >(work_id, new_artwork));
	}

	printAllArtists();
	printAllArtwork();
}

void art_db::printArtist(shared_ptr<artist> target) const
{
	cout << target->getID() << ": " << target->getName() << " (" << target->getBirthDateString(false) 
		<< " - " << target->getDeathDateString(false) << ")" << endl;
}

void art_db::printArtwork(const artwork &target) const
{
	cout << target.getID() << ": " << target.getTitle() << " by " << target.getArtistName() << endl;
	cout << "\t" << target.getHeight() << "x" << target.getWidth() << ", " << getDateString(target.getDate(), false) << ", $" << target.getValue() << "k" << endl;
	cout << "Rarity: " << target.getRarity() << endl;
}