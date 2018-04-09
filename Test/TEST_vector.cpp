
#include "../stl_vector.h"
#include <iostream>

int main(void)
{
    ustl::vector<int> vec;
    vec.push_back(1);
    vec.push_back(2);
    std::cout << vec.size() << std::endl;
    std::cout << vec.capacity() << std::endl;
    std::cout << vec[1] << std::endl;

    vec.erase(vec.begin()+1);
    std::cout << vec.size() << std::endl;
    std::cout << vec.capacity() << std::endl;

    return 0;
}
