#ifndef ARTWORK_H
#define ARTWORK_H
#include "header.h"
#include "artist.h"

class artwork
{
public:
	artwork(){};
	~artwork(){};

	genre GetGenre() const { return genre; }
	double GetValue() const { return value; }

private:
	double value;
	genre genre;

	artist artist;
};

#endif