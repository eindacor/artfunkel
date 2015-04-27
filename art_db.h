#ifndef ART_DB_H
#define ART_DB_H
#include "header.h"

class art_db
{
public:
	art_db(const char* artists_path, 
		const char* paintings_path, 
		const char* image_directory, 
		shared_ptr<ogl_context> context,
		shared_ptr<ogl_camera> camera);
	~art_db(){};

	shared_ptr<artist> lookupArtistByName(string name) const { return artists.at(name); }
	//replace below functions to take string and look up artist
	void printArtist(shared_ptr<artist>) const;
	void printArtwork(shared_ptr<artwork>) const;

	void printAllArtists() const { for (auto i : artists) printArtist(i.second); }
	void printAllArtwork() const { for (auto i : artworks) printArtwork(i.second); }

	shared_ptr<artwork> getArtwork(int n) { return artworks.at(n); }

private:
	map<string, shared_ptr<artist> > artists;
	map<int, shared_ptr<artwork> > artworks;
};

#endif