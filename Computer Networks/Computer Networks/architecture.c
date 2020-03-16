#include "architecture.h"

int which_architecture()
{
	int i = 1;
	char* first_byte = (char*) &i;

	return i == (int)(*first_byte);
}