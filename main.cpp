#include <iostream>
#include <SFML/Graphics.hpp>
#include "GameEngine.hpp"

int main()
{
    std::cout << "Hello, Zelda!" << std::endl;
    GameEngine g("config/assets.txt");
    g.run();

    return 0;
}
