
#include "../stl_list.h"
#include <iostream>
#include <vector>

using std::cout;
using std::endl;

int main(void)
{
    std::vector<int> nums{1,2,3,4,5,6};
    ustl::list<int> L1(nums.begin(), nums.end());
    ustl::list<int> L2(3, 43);

    L1.splice(L1.begin(), L2);

    /*
    cout << "L1 node addr: ";
    auto it1 = L1.begin();
    while (it1 != L1.end()) {
        cout << (it1.node_ptr) << " ";
        it1++;
    }
    cout << endl;

    cout << "L2 node addr: ";
    auto it2 = L2.begin();
    while (it2 != L2.end()) {
        cout << (it2.node_ptr) << " ";
        it2++;
    }
    cout << endl;


    cout << "After splice: ";
    auto pos = L1.begin();
    while (pos != L1.end()) {
        cout << (pos.node_ptr) << " ";
        pos++;
    }
    cout << endl;


*/
    cout << "Finish All OUTPUT" << endl;
    //exit(0);

    return 0;
}

