//shared struct for both cli/svr that defines xy vars for balls, paddles, score
//serialization
#pragma once
#include "entities_server.h"

struct GameState {
    Vec2d ballPos;
    Vec2d paddle1Pos;
    Vec2d paddle2Pos;
};

struct InputState {
    bool up;
    bool down;
}; 