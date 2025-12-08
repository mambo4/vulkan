#include "first_app.hpp"
#include <cstdlib>
#include <iostream>
#include <stdexcept>
#include <direct.h>

int main() {
    std::cout << "Current working directory: " << getcwd(nullptr, 0) << std::endl;
    lbe::FirstApp app{};

    try{
        app.run();
    } catch (const std::exception& e){
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}