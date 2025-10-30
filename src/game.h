#ifndef GAME_H
#define GAME_H

#include <SDL2/SDL.h>
#include <iostream>

bool init();
void kill();
bool loop();


extern SDL_Window *window;
extern SDL_Renderer *renderer;

#endif