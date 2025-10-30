#include "game.h"

const int BALL_WIDTH = 15;
const int BALL_HEIGHT = 15;

class Vec2d {

    public:
        Vec2d() : x(0.0f), y(0.0f) {}
        Vec2d(float x, float y) : x(x), y(y) {}
        
        //where x->position & y->velocity
        float x, y;

        //vector operators
        Vec2d operator+(Vec2d const &rhs) { return Vec2d(x+rhs.x, y+rhs.y);}
        Vec2d& operator+=(Vec2d const &rhs) {
            x += rhs.x;
            y += rhs.y;

            return *this;
        }

        Vec2d operator*(float rhs) {
            return Vec2d(x * rhs, y * rhs);
        }
};

class Ball {
    public:
        Ball(Vec2d position) : position(position) {
            rect.x = static_cast<int>(position.x);
            rect.y = static_cast<int>(position.y);
            rect.w = BALL_WIDTH;
            rect.h = BALL_HEIGHT;
        }

        void Draw(SDL_Renderer* renderer) {
            rect.x = static_cast<int>(position.x);
            rect.y = static_cast<int>(position.y);

            SDL_RenderFillRect(renderer, &rect);
        }

        Vec2d position;
        SDL_Rect rect{};
};

class Paddle {
    public:
    private:

};