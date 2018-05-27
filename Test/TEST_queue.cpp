

#include "queue"
#include "vector"
#include "bits/algorithm.tcc"
#include <iostream>

using std::cout;
using std::endl;

int main(void)
{
    ustl::queue<int> st;
    st.push(0);
    st.push(1);
    cout << st.front() << endl;
    cout << st.back() << endl;
    st.pop();
    cout << st.front() << endl;

    cout << "priority_queue TEST" << endl;

    ustl::priority_queue<int, ustl::vector<int>, ustl::greater<int>> pq;
    pq.push(1);
    pq.push(2);
    pq.push(42);

    cout << pq.top() << endl;
    pq.pop();
    cout << pq.top() << endl;

    return 0;
}
