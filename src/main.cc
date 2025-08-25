#include "CometScene.hh"

#include <iostream>

int main() {
    try {
        CometScene scene;
        scene.run();
    }
    catch (const std::exception& e) {
        std::cerr << "Erreur: " << e.what() << std::endl;
        return -1;
    }
    
    return 0;
}
