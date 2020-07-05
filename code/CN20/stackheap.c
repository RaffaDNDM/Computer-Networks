#include<stdlib.h>
#include<stdio.h>

int input = 5;

int fattoriale(int in){
	int * c; //Stack
	c = malloc(sizeof(int)); //Heap memory allocation
	printf("in=%d; local (stack) %lx  malloc (heap)%lx\n", in, (long int) &c, (long int) c);
	if (in == 1) return 1;
	return fattoriale(in-1)*in;
}

int main()
{
	printf("static area: %lx\n",(long int) &input);
	printf("%d\n",fattoriale(input));
}
