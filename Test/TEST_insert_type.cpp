
#include "../stl_list.h"

int main(void)
{
    ustl::list<int> L;
    L.insert(L.begin(), 10, 2);

    return 0;
}
