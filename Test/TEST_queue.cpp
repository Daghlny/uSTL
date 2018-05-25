

#include "queue"
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

    return 0;
}
