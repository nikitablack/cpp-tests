#include "mainapplication.h"

int main(int argc, char *argv[])
{
    MainApplication& a{MainApplication::get(argc, argv)};
    
    return a.exec();
}

