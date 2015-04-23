#include "header.h"
#include "artist.h"
#include "artwork.h"

void printDate(jep::date d)
{
	cout << getDateString(d) << endl;
}

void printArtist(const artist &a)
{
	cout << a.getID() << ": " << a.getName() << " (" << a.getBirthDateString(false) << " - " << a.getDeathDateString(false) << ")" << endl;
}

void printArtwork(const artwork &a)
{
	cout << a.getTitle() << " by " << a.getArtistName() << "($" << a.getValue() << " Million)" << endl;
}

int main()
{
	jep::init();

	int id_count = 0;

	artist picasso(id_count++, "Pablo Picasso", jep::date(1881, 10, 25), jep::date(1973, 4, 8));
	artist van_gogh(id_count++, "Vincent van Gogh", jep::date(1853, 3, 30), jep::date(1890, 7, 29));
	artist dali(id_count++, "Salvador Dali", jep::date(1904, 5, 11), jep::date(1989, 1, 23));
	artist duchamp(id_count++, "Marcel Duchamp", jep::date(1887, 7, 28), jep::date(1968, 10, 2));

	vector<artist> artist_vec;
	artist_vec.push_back(picasso);
	artist_vec.push_back(van_gogh);
	artist_vec.push_back(dali);
	artist_vec.push_back(duchamp);

	vector<artwork> artwork_vec;
	artwork_vec.push_back(artwork("Nude Descending a Staircase", duchamp, DADA, LEGENDARY, false, 1.0f, jep::date(1958, 10, 2)));
	artwork_vec.push_back(artwork("Cafe Terrace at Night", van_gogh, EXPRESSIONIST, MASTERPIECE, false, 1.0f, jep::date(1958, 10, 2)));

	for (auto i : artist_vec)
		printArtist(i);

	for (auto i : artwork_vec)
		printArtwork(i);
}