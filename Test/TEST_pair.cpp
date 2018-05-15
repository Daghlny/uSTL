
#include <iostream>
#include "stl_pair.h"
#include "algorithm.tcc"

using std::cout;
using std::endl;
using ustl::pair;

int main(void)
{
    pair<int,int> p1 = ustl::make_pair(1,2);
    pair<int,int> p2 = ustl::make_pair(2,1);

    if (p1 < p2) 
        cout << "p1 < p2" << endl;
    else
        cout << "p1 >= p2" << endl;

    ustl::swap(p2.first, p2.second);

    if (p1 == p2)
        cout << "p1 == p2" << endl;

    return 0;
}
