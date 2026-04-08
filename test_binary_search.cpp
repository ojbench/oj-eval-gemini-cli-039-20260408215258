#include <iostream>
#include <vector>
#include <algorithm>

int main() {
    std::vector<int> v = {1, 3, 5, 7};
    auto it = std::lower_bound(v.begin(), v.end(), 4);
    std::cout << (it - v.begin()) << std::endl;
    return 0;
}
