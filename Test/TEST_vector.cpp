
#include "../stl_vector.h"
#include <iostream>

int main(void)
{
    ustl::vector<int> vec1;
    ustl::vector<int> vec2;

    vec1.push_back(1);
    vec1.push_back(2);
    vec2.push_back(3);
    vec2.push_back(4);

    std::cout << vec1.size() << std::endl;
    std::cout << vec1.capacity() << std::endl;
    std::cout << vec1[1] << std::endl;

    vec1.erase(vec1.begin()+1);

    std::cout << vec1.size() << std::endl;
    std::cout << vec1.capacity() << std::endl;

    vec1.insert(vec1.begin(), 10, 2);

    vec1.insert(vec1.begin()+5, 7);
    vec1.erase(vec1.begin(), vec1.begin()+5);

    for (int i = 0; i < vec1.size(); ++i)
        std::cout << vec1[i] << " ";
    std::cout << std::endl;

    return 0;
}

