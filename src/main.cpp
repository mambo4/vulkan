#include "first_app.hpp"
#include <cstdlib>
#include <iostream>
#include <stdexcept>

/* Vulkan Game Engine Tutorial 03
https://www.youtube.com/watch?v=LYKlEIzGmW4&list=PL8327DO66nu9qYVKLDmdLW_84-yE4auCR&index=4
*/

int main() {

    lbe::FirstApp app{};

    try{
        app.run();
    } catch (const std::exception& e){
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}