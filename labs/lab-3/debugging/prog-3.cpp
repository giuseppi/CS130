#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>

int main()
{
    std::vector<double> v(100);

    for(size_t i = 0; i < v.size(); i++)
        v[i] = cos(i);

    std::sort(v.begin(), v.end());

    std::cout << v[0] << std::endl;
    
    return 0;
}
