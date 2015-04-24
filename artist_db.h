#ifndef ARTIST_DB_H
#define ARTIST_DB_H
#include "header.h"
#include "artist.h"

class artist_db
{
public:
	artist_db(){};
	~artist_db(){};

	artist lookupArtistByName(string name) { return artist(0, "TEST ARTIST", jep::date(1983, 6, 23), jep::date(1983, 6, 23)); }

private:

};

#endif