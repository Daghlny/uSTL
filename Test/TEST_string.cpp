
#include "ustl_string"
#include <iostream>

using std::cout;
using std::endl;

int main(void)
{
    ustl::string str(10, 'a');
    auto it = str.begin();
    while (it != str.end())
        cout << *(it++) << " ";
    cout << endl;

    str.insert(2, 20, 'c');
    it = str.begin();
    while (it != str.end())
        cout << *(it++) << " ";
    cout << endl;

    ustl::string str1(str);

    it = str1.begin();
    while (it != str1.end())
        cout << *(it++);
    cout << endl;

    str1.insert(2, "hello");

    it = str1.begin();
    while (it != str1.end())
        cout << *(it++);
    cout << endl;
    return 0;
}
