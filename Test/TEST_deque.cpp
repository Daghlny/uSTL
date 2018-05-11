
#include "stl_deque.h"
#include "deque.tcc"
#include <iostream>
#include <vector>

using std::cout;
using std::endl;

int main(void)
{
    ustl::deque<int> dq;
    cout << dq.size() << endl;
    dq.insert(dq.begin(), 2, 13);

    ustl::deque<int>::iterator it = dq.begin();
    while (it != dq.end())
        cout << *(it++) << " ";
    cout << endl;

    std::vector<int> vec = {1,3,5,7,9,11};
    dq.insert(dq.begin()+1, vec.begin(), vec.end());

    it = dq.begin();
    while (it != dq.end())
        cout << *(it++) << " ";
    cout << endl;

    ustl::deque<int> dq2(dq);
    auto it2 = dq2.crbegin();
    while (it2 != dq2.crend())
        cout << *(it2++) << " ";
    cout << endl;
    cout << dq2[5] << " front:" << dq2.front() << " back:" << dq2.back() << endl;
    dq2.shrink_to_fit();


    return 0;
}
