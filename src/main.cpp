#include "menus.hpp"

int main() {
    GameWindow window;
    window.init(1280, 720, "Balloon Buster");
    window.sc_mgr.set_current_scene(new TitleScreen(&window.sc_mgr));
    window.run();

    return 0;
}
