
#include "../stl_list.h"
#include <iostream>

int main(void)
{
    ustl::list<int> L((size_t)10, 2);
    std::cout << L.size() << std::endl;

    ustl::list<int>::iterator pos = L.begin();
    while (pos != L.end()) {
        std::cout << *(pos++) << " ";
    }
    std::cout << std::endl;

    pos = L.begin();
    for (int i = 0; i < 10; ++i) {
        if ( int(i & 0x01) == 0 ) {
            pos = L.erase(pos);
        }
        else
            pos++;

    }

    pos = L.begin();
    while (pos != L.end()) {
        std::cout << *(pos++) << " ";
    }
    std::cout << std::endl;
    // should be 2 2 2 2 2

    L.insert(L.begin(), 7);
    pos = L.begin();
    pos++;
    pos++;
    L.insert(pos, 2, 42);

    pos = L.begin();
    while (pos != L.end()) {
        std::cout << *(pos++) << " ";
    }
    std::cout << std::endl;
    // should be 7 2 42 42 2 2 2 2

    return 0;
}
