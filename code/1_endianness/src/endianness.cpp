#include <iostream>
#include "endianness.hpp"

int main(int argc, char**argv)
{
    int x1 = 0x01020304;
    short int x2 = 0x0102;

    unsigned char* p1 = (unsigned char*) &x1; 
    unsigned char* p2 = (unsigned char*) &x2;

    int i;
    std::cout<<"0x";
    
    for(i=0; i<sizeof(x1); i++)
        std::cout<<std::hex<< static_cast<int>(p1[i]);
    
    std::cout<<"\n0x";
    for(i=0; i<sizeof(x2); i++)
        std::cout<<std::hex<< static_cast<int>(p2[i]);

    x1 = htonl(x1);
    x2 = htons(x2);

    std::cout<<"\n0x";
    
    for(i=0; i<sizeof(x1); i++)
        std::cout<<std::hex<< static_cast<int>(p1[i]);
    
    std::cout<<"\n0x";
    for(i=0; i<sizeof(x2); i++)
        std::cout<<std::hex<< static_cast<int>(p2[i]);
    std::cout<<std::endl;

    return 0;
}

bool is_little_endian()
{
    int i=1;
    char* p = (char*) &i;

    return (static_cast<int>(*p))==i;
}


short int htons(short int num)
{
    int size = sizeof(num);
    short int num2 = 0;
    int i;
    unsigned char* p1 = (unsigned char*) &num;
    unsigned char* p2 = (unsigned char*) &num2;

    if(is_little_endian)
    {
        for(i=0; i<size; i++)
            p2[i]=p1[size-i-1];

        return num2;
    }
    else
        return num;
}

int htonl(int num)
{
    int size = sizeof(num);
    int num2 = 0;
    int i;
    unsigned char* p1 = (unsigned char*) &num;
    unsigned char* p2 = (unsigned char*) &num2;

    if(is_little_endian)
    {
        for(i=0; i<size; i++)
            p2[i]=p1[size-i-1];

        return num2;
    }
    else
        return num;
}
