#include "header.h"
#include "artist.h"
#include "artwork.h"
#include "artist_db.h"
#include "utility_funcs.h"

int main()
{
	jep::init();

	int id_count = 0;
	artist_db artist_database;

	vector<artwork> artwork_vec;
	//artwork_vec.push_back(artwork("Nude Descending a Staircase", duchamp, DADA, LEGENDARY, false, 1.0f, jep::date(1958, 10, 2)));
	//artwork_vec.push_back(artwork("Cafe Terrace at Night", van_gogh, EXPRESSIONIST, MASTERPIECE, false, 1.0f, jep::date(1958, 10, 2)));

	jep::csv_file csv_test("C:\\Users\\admin\\Desktop\\paintings.csv");
	
	for (int i = 1; i < csv_test.getRowCount(); i++)
	{
		vector<string> row = csv_test.getRow(i);
		
		artist work_artist(artist_database.lookupArtistByName(row.at(0)));
		string work_title(row.at(1));
		jep::date work_date(std::stoi(row.at(2)), -1, -1);
		genre work_genre(genreFromString(row.at(3)));
		rarity work_rarity(rarityFromString(row.at(4)));
		bool work_forgery = false;
		float work_condition = 1.0f;
		
		artwork_vec.push_back(artwork(work_title, work_artist, work_genre, work_rarity, work_forgery, work_condition, work_date));
	}

	for (auto i : artwork_vec)
		printArtwork(i);
	
	string ds = "1983/06/23";
	jep::date string_date(ds);
	printDate(string_date);
}