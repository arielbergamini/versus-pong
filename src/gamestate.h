//shared struct for both cli/svr that defines xy vars for balls, paddles, score
//serialization
#include <SDL2/SDL.h.>
#include "entities.h"

struct GameState {
    Vec2d ballPos;
    Vec2d paddle1Pos;
    Vec2d paddle2Pos;
};

struct InputState {
    bool up;
    bool down;
}; 