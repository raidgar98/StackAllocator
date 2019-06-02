#include <iostream>
#include <limits>
#include <iomanip>
#include "stackAllocator.h"

using namespace std;

struct test
{
	double tab[20];
	long long int tab1[20];
    float tab2[10];

};

int main() 
{

    stackAllocator<test> allo(std::numeric_limits<size_t>().max());
	//stackAllocator<test> allo(std::numeric_limits<size_t>::max());

    test* ptr0 = allo.allocate(2000);
    test* ptr1 = allo.allocate(14000);
    test* ptr2 = allo.allocate(6300);
	//test* ptr3 = allo.allocate(86700);

	cout << "Poprawnie zaalokowano wszystkie wskaŸniki. Dostêpne miejsce: " << allo.freeSpace() << " B." << endl << endl;

	try
	{
		allo.deallocate(ptr0);
	}
	catch (...)
	{
		cout << "Nie da siê zdealokowaæ tej czêœci pamiêci!" << endl;
	}

//	allo.deallocate(ptr3);
//	cout << "Zdealokowano poprawnie ptr3!\nDostêpna pamiêæ: "<<allo.freeSpace()<<" B." << endl;
	allo.deallocate();
	cout << "Zdealokowano poprawnie ptr2!\nDostêpna pamiêæ: " << allo.freeSpace() << " B." << endl;
	allo.deallocate();
	cout << "Zdealokowano poprawnie ptr1!\nDostêpna pamiêæ: " << allo.freeSpace() << " B." << endl;
	allo.deallocate(ptr0);
	cout << "Zdealokowano poprawnie ptr0!\nDostêpna pamiêæ: " << allo.freeSpace() << " B." << endl;

	cin.get();

	return 0;
}
