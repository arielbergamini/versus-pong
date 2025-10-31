#include "game.h"

const int BALL_WIDTH = 15;
const int BALL_HEIGHT = 15;

const int PADDLE_WIDTH = 10;
const int PADDLE_HEIGHT = 80;

const int BALL_SPEED = 250;

enum class CollisionType {
    None,
    Top,
    Middle,
    Bottom
};

struct Contact {
    CollisionType type;
    float penetration;
};

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


//TODO --> ball physics!
class Ball {
    public:
        Ball(Vec2d position, Vec2d velocity) 
        : position(position), velocity(velocity) {
            rect.x = static_cast<int>(position.x);
            rect.y = static_cast<int>(position.y);
            rect.w = BALL_WIDTH;
            rect.h = BALL_HEIGHT;
        }

        void PaddleCollision(Contact const &contact) {
            position.x += contact.penetration;
            velocity.x = -velocity.x;

            if (contact.type == CollisionType::Top) {
                velocity.y = -.75f * BALL_SPEED;
            } else if (contact.type == CollisionType::Bottom) {
                velocity.y = 0.75f * BALL_SPEED;
            }
        }

        void Update(float dt) {
            position += velocity * dt;

            if (position.y <= 0) {
                position.y = 0;
                velocity.y *= -1;
            } else if (position.y + BALL_HEIGHT >= 480.0f) {
                position.y = 480.0f - BALL_HEIGHT;
                velocity.y *= -1;
            }
        }

        void Draw(SDL_Renderer* renderer) {
            rect.x = static_cast<int>(position.x);
            rect.y = static_cast<int>(position.y);

            SDL_RenderFillRect(renderer, &rect);
        }

        Vec2d position, velocity;
        SDL_Rect rect{};
};


//TODO --> input sync & paddle physics!
class Paddle {
    public:
        Vec2d position, velocity;
        SDL_Rect rect{};

        Paddle(Vec2d position, Vec2d velocity)
         : position(position), velocity(velocity) {
            rect.x = static_cast<int>(position.x);
            rect.y = static_cast<int>(position.y);
            rect.w = PADDLE_WIDTH;
            rect.h = PADDLE_HEIGHT;
        }

        void Update(float moveDir, float dt) {

            if(moveDir == 0.0f) {
                velocity.y *= 0.9f;
            } else {
                velocity.y = moveDir * 300.0f;
            }

            position += velocity * dt;
            
            //screen bounds handling
            if (position.y < 0) {
                position.y = 0; //position restricted to top of screen
            } else if (position.y > (480.0 - PADDLE_HEIGHT )) {
                position.y = 480.0 - PADDLE_HEIGHT; //restricted to bottom of screen
            }
        }

        void Draw(SDL_Renderer* renderer) {
            rect.x = static_cast<int>(position.x);
            rect.y = static_cast<int>(position.y);
            rect.w = PADDLE_WIDTH;
            rect.h = PADDLE_HEIGHT;

            SDL_RenderFillRect(renderer, &rect);
        }
};


Contact Collision(Ball const &ball, Paddle const &paddle) {
    float ballLeft = ball.position.x;
    float ballRight = ball.position.x + BALL_WIDTH;
    float ballTop = ball.position.y;
    float ballBottom = ball.position.y + BALL_HEIGHT;

    float paddleLeft = paddle.position.x;
    float paddleRight = paddle.position.x + PADDLE_WIDTH;
    float paddleTop = paddle.position.y;
    float paddleBottom = paddle.position.y + PADDLE_HEIGHT;

    Contact contact{};
    contact.type = CollisionType::None;
    contact.penetration = 0.0f;

    if (ballLeft >= paddleRight) {
        return contact;
    }
    if (ballRight <= paddleLeft) {
        return contact;
    }
    if (ballTop >= paddleBottom) {
        return contact;
    }
    if (ballBottom <= paddleTop) {
        return contact;
    }
    
    float paddleRangeUpper = paddleBottom - (2.0f * PADDLE_HEIGHT / 3.0f);
    float paddleRangeMiddle = paddleBottom - (PADDLE_HEIGHT / 3.0F);

    //left paddle
    if (ball.velocity.x < 0) {
        contact.penetration = paddleRight - ballLeft;
    }
    //right paddle
    else if (ball.velocity.x > 0) {
        contact.penetration = paddleLeft - ballRight;
    }

    if ((ballBottom > paddleTop) && (ballBottom < paddleRangeUpper)) {
        contact.type = CollisionType::Top;
    }
    else if ((ballBottom > paddleRangeUpper) && (ballBottom < paddleRangeMiddle)) {
        contact.type = CollisionType::Middle;
    }
    else {
        contact.type = CollisionType::Bottom;
    }
    return contact;

};
