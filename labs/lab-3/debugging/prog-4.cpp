#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>

void print_cases(const std::string& name)
{
    std::vector<char> up(name.size());
    std::vector<char> lo(name.size());
    
    for(size_t i = 0; i < name.size(); ++i) {
        up[i] = std::toupper(name[i]);
        lo[i] = std::tolower(name[i]);
	std::cout << lo[i];
    }
	
    std::cout << " ";

    for(size_t i = 0; i < name.size(); ++i) {
	std::cout << up[i];
    }
    
    std::cout << std::endl;
}

int main()
{
    print_cases("Elizabeth");
    print_cases("John");
    print_cases("Frank");
    print_cases("Fred");

    return 0;
}

