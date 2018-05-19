
#include "ustl_string"
#include <iostream>
#include <vector>

using std::cout;
using std::endl;

void print(const ustl::string& s, const char* strname)
{
    cout << strname << "(" << s.size() << "): ";
    auto it = s.cbegin();
    while (it != s.cend())
        cout << *(it++);
    cout << endl;
}

int main(void)
{
    std::vector<char> vec;
    char ch = 'a';
    while (ch < 'z')
        vec.push_back(ch++);

    ustl::string a(2, '3');
    print(a, "a");
    cout << vec.size() << endl;
    a.insert(a.begin()+1, vec.begin(), vec.end());
    print(a, "a");
    ustl::string b(a, 0, 3);
    print(b, "b");
    return 0;
}
