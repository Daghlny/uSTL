
#include "vector"
#include <iostream>

using std::endl;
using std::cout;

struct F{
    int n;
    F(int _n): n(_n) {}
    F(const F& f): n(f.n) {}
};

template<class T>
int print(ustl::vector<T>& v, const char* vname)
{
    cout << vname << "(" << v.size() << "): ";
    for (typename ustl::vector<T>::iterator it = v.begin(); it != v.end(); ++it)
        cout << *it << " ";
    cout << endl;
    return 0;
}

template<class T>
ustl::vector<T> get_vector(ustl::vector<T> v, int n)
{
    cout << "--------- get_vector ----------" << endl;
    ustl::vector<T> vec(n, 0);
    return vec;
}

int main(void)
{
    ustl::vector<F> v1 = {1,3,5,7,9};
    ustl::vector<F> v2 = get_vector(v1, 10);

    return 0;
}

