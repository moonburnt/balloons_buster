#include "level.hpp"

int main() {
    GameWindow window;
    window.init(1280, 720, "Balloon Buster");
    window.sc_mgr.set_current_scene(new Level());
    window.run();

    return 0;
}
