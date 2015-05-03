#ifndef ARTIST_H
#define ARTIST_H
#include "header.h"

class artist
{
public:
	artist(long id, string name, date birth_date, date death_date) : 
		ID(id), name(name), birth(birth_date), death(death_date) { deceased = (birth_date != death_date); }
	~artist(){};

	long getID() const { return ID; }
	string getName() const { return name; }
	bool isDeceased() const { return deceased; }
	string getBirthDateString(bool include_day=false) const { return getDateString(birth, include_day); }
	string getDeathDateString(bool include_day = false) const { return (deceased ? getDateString(death, include_day) : " "); }
	date getBirthDate() const { return birth; }
	date getDeathDate() const { return death; }

private:
	long ID;
	string name;
	bool deceased;
	date birth;
	date death;
};

#endif