#include "libraries/SeaBattleField.h"
#include <iostream>

// Пример использования класса

int main() {
    SeaBattleField sea(10, Visibility::Clear);

    sea.add_ship({3, 3}, Orientation::Horizontal, 4);
    std::cout << sea;
    sea.add_ship({1, 1}, Orientation::Vertical, 6);
    std::cout << sea;
    sea.hit_at({3,1});
    std::cout << sea;
    sea.set_visible(Visibility::Fog);
    std::cout << sea;

    return 0;
}