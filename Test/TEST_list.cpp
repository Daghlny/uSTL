
#include "stl_list.h"
#include "stl_list.tcc"
#include <iostream>
#include <vector>

using std::cout;
using std::endl;

int main(void)
{
    std::vector<int> nums1{3,2,1,5,7,6,10};
    ustl::list<int> L1(nums1.begin(), nums1.end());

    auto it = L1.begin();
    while (it != L1.end())
        cout << *(it++) << " ";
    cout << endl;

    L1.sort();

    it = L1.begin();
    while (it != L1.end())
        cout << *(it++) << " ";
    cout << endl;

    return 0;
}

