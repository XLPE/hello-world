#include <iostream>
#include "split_v2.h"

using namespace std;

int main(int argc, char** argv)
{
	if(argc != 3)
	{
		cout << "Usage: " << argv[0] << " string delimiter" << endl;
		return -1;
	}

	char* s = argv[1];
	char delimiter = argv[2][0];
	int count = 0;
	string_split sp(s, delimiter);
	char *substring;
	while((substring = sp.parse()) != NULL)
	{
		count++;
		cout << "the first substring: " << substring << endl;
	}
	return 0;
}



