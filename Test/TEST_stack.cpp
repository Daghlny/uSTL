
#include "stack"
#include <iostream>

using std::cout;
using std::endl;

int main(void)
{
    ustl::stack<int> st;
    st.push(0);
    st.push(1);
    cout << st.top() << endl;
    st.pop();
    cout << st.top() << endl;

    return 0;
}
