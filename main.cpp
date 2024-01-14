#include "libraries/SeaBattleField.h"
#include <iostream>

// Пример использования класса

int main() {
    SeaBattleField sea(10, Visibility::Fog);
    sea.start_game();
    
    return 0;
}