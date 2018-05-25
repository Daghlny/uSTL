
#include "string"
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

    ustl::string c = a;
    print(c, "c");

    c += a;
    print(c, "c");

    ustl::string::size_type first_b_in_c = c.find(b);
    ustl::string::size_type second_b_in_c = c.find(b, first_b_in_c+1);
    cout << first_b_in_c << " " << second_b_in_c  << endl;

    int pos = 0;
    while (pos < c.size() && pos != ustl::string::npos)
    {
        pos = c.find_first_of(b, pos);
        if (pos != ustl::string::npos)
            cout << c[pos] << " ";
        pos = pos+1;
    }
    cout << endl;

    return 0;
}

