#include <iostream>

int main(int argc, char** argv)
{
    int i=1;
    char* p = (char*) &i;

    if((int) *p==i)
        std::cout<<"Little Endian"<<std::endl;
    else
        std::cout<<"Big Endian"<<std::endl;
}
