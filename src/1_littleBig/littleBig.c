#include <stdio.h>

int main(void)
{
	int i=1;
	char* p = (char*) &i;

	if((int) *p==i)
		printf("Little Endian \n");
	else
		printf("Big Endian \n");
}
