#include "header.h"

void printDate(const jep::date &d)
{
	bool day_known = d.getDayOfWeek() != UNKNOWN_DAY;

	if (day_known)
		cout << d.getDayString() << ", ";

	if (d.getMonth() != UNKNOWN_MONTH)
		cout << d.getMonthString() << " ";

	if (day_known)
		cout << d.getDayOfMonth() << ", ";

	cout << d.getYear() << endl;
}

int main()
{
	jep::date test(9070, 6, 23);
	printDate(test);
	test++;
	printDate(test);

	
	jep::date test2(2015, 12, 23);
	printDate(test2);

	for (int i = 0; i < 10; i++)
	{
		test2++;
		printDate(test2);
	}

	test2 += 381;
	printDate(test2);

	cout << endl;

	jep::date test3(2015, 5, -1);
	printDate(test3);
	test3++;
	printDate(test3);

	jep::date test4(2015, -1, -1);
	printDate(test4);
	test4++;
	printDate(test4);
}