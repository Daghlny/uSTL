
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
    std::vector<int> vec = {1,3,5,7,9,11};
    dq.insert(dq.begin()+1, vec.begin(), vec.end());
    ustl::deque<int>::iterator it = dq.begin();
    while (it != dq.end())
        cout << *(it++) << " ";
    cout << endl;
    return 0;
}
