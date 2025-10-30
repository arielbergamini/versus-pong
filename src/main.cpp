
#include "game.h"

int main (int argc, char* argv[]) {
    
    if (!init()) return 1;

    //wait before processing next frame
    while (loop()) {
        SDL_Delay(10);
    }
    kill();
    return 0;
}
