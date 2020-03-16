#include "architecture.h"
#include <stdio.h>

int main(void)
{
	if (which_architecture)
		printf("Little Endian");
	else
		printf("Big Endian");
}