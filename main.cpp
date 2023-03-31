#include <iostream>
#include "ingredients.hpp"

int main() {
    std::cout << CraftOpt::Ingredients<CraftOpt::WOODWORKING>::ingredients_effectiveness[0].position_modifiers.at(CraftOpt::Pos_Under) << std::endl;
    return 0;
}
