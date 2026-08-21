#include <iostream>

#include "representation/Game.hpp"

int main() {
    try {
        representation::Game game(720, 624, "Bomberman");
        game.run();
    } catch (const std::exception& exception) {
        std::cerr << "Fatal error: " << exception.what() << '\n';
        return 1;
    }
    return 0;
}
