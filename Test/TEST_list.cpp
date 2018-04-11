
#include "../stl_list.h"
#include <iostream>

int main(void)
{
    ustl::list<int> L((size_t)10, 2);
    std::cout << L.size() << std::endl;

    return 0;
}
