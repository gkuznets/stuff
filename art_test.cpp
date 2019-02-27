#include <stuff/container/adaptive_radix_tree.hpp>

int main() {
    stuff::container::adaptive_radix_tree<int> art;
    art.insert(1, 10);
    art.insert(10, 100);
    art.insert(100, 1000);
    art.insert(1000, 10000);
    art.insert(10000, 100000);
    art.dump();

    std::cout << '\n';
    art.erase(1);
    art.dump();
}
