#include "Renderer.h"

const unsigned int WINDOW_WIDTH = 1000;
const unsigned int WINDOW_HEIGHT = 700;

int main() {
    Renderer renderer(WINDOW_WIDTH, WINDOW_HEIGHT, "Smart Traffic Simulation - Group 3");

    while (renderer.isWindowOpen()) {
        renderer.handleEvents();
        renderer.update();
        renderer.render();
    }

    return 0;
}