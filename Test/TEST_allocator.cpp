
#include "../stl_allocator.h"
#include <vector>
#include <iostream>

int main(void)
{
    std::vector<int, ustl::allocator<int> > vec;
    vec.push_back(1);
    vec.push_back(2);
    std::cout << vec.size() << std::endl;
    vec.erase(vec.begin(), vec.end());
    std::cout << vec.size() << std::endl;

    return 0;
}
