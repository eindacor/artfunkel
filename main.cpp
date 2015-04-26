#include "header.h"
#include "artist.h"
#include "artwork.h"
#include "art_db.h"
#include "utility_funcs.h"

int main()
{
	jep::init();

	int id_count = 0;

	string data_path("J:\\GitHub\\artfunkel\\");
	string paintings_path = data_path + "paintings.csv";
	string artists_path = data_path + "artists.csv";

	art_db artist_database(artists_path.c_str(), paintings_path.c_str());

}