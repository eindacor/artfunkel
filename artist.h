#ifndef ARTIST_H
#define ARTIST_H
#include "header.h"

class artist
{
public:
	artist(long i, string n, jep::date b, jep::date d) : ID(i), name(n) {};
	~artist(){};

private:
	long ID;
	string name;
	bool deceased;
	jep::date birth;
	jep::date death;
};

#endif