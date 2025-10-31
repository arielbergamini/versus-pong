#include "entities.h"
#include <chrono>
SDL_Window* window = nullptr;
SDL_Renderer* renderer = nullptr;


//TODO --> read local input and move paddle1
bool loop () {
    SDL_Event event;
    SDL_Rect rect;
    bool keep_window_open = true;
    float dt = 0.016f; //FIXME --> placeholder dt for testing
    const int BALL_SPEED = 250;

    //----------Create game objects once
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
    SDL_RenderClear(renderer);

    Ball ball(
        Vec2d(680.0 / 2.0f,  480.0 / 2.0f),
        Vec2d(BALL_SPEED, 0.0f));

    Paddle paddle1(
        Vec2d(50.0f, 480.0 / 2.0f),
        Vec2d(0.0f, 0.0f));

    Paddle paddle2(
        Vec2d(680.0 - 50.0f, 480.0 / 2.0f),
        Vec2d(0.0f, 0.0f));   

    //GAME LOOP-------------------------------------------
    auto lastTime = std::chrono::high_resolution_clock::now();

    while (keep_window_open) {
        //frame dt

        auto currentTime = std::chrono::high_resolution_clock::now();
        std::chrono::duration<float> elapsed = currentTime - lastTime;
        dt = elapsed.count();
        lastTime = currentTime;

        if (dt > 0.05f) dt = 0.05f; //damping

        //handling window events
        while (SDL_PollEvent(&event)) { 
                if (event.type == SDL_QUIT) { //close window if user wants to quit
                        keep_window_open = false;
                }
            }

            //handle LOCAL CLIENT (paddle1)
            //
            //
            const Uint8* keys = SDL_GetKeyboardState(nullptr);
            float moveDir = 0.0f;
            //player 1
            if (keys[SDL_SCANCODE_W]) moveDir -= 1.0f;
            if (keys[SDL_SCANCODE_S]) moveDir += 1.0f;

            //update object positions
            paddle1.Update(moveDir, dt);
            ball.Update(dt);

            //check collisions
            Contact contact1 = Collision(ball, paddle1);
            if (contact1.type != CollisionType::None) {
                ball.PaddleCollision(contact1);
            }

            Contact contact2 = Collision(ball, paddle2);
            if (contact2.type != CollisionType::None) {
                ball.PaddleCollision(contact2);
            }
            //===TODO --> SEND local input to server
            //
            //
            //auto stopTime = std::chrono::high_resolution_clock::now();
            //dt = std::chrono::duration<float, std::chrono::milliseconds::period>(stopTime - startTime).count();

            //------Rendering-------
            //clear bg to black
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            SDL_RenderClear(renderer);

            //paddle colors set to pink
            SDL_SetRenderDrawColor(renderer, 233, 4, 122, 0.8);
            
            //draw objects to window
            paddle1.Draw(renderer);
            paddle2.Draw(renderer);

            ball.Draw(renderer);

            SDL_RenderPresent(renderer);
        }

    SDL_DestroyWindow(window);
    SDL_Quit();
    return false;
}

//initializing sdl
bool init () {
     if(SDL_Init(SDL_INIT_EVERYTHING) < 0) {
        printf("Failed to initialize SDL2\n");
        system("pause");
        return false;
    }

    window = SDL_CreateWindow("Versus Pong!", SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED, 680, 480, 0);

    if (!window) {
        printf("Failed to create window\n");
        system("pause");
        return false;
    }
    
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        printf("Failed to create renderer\n");
        system("pause");
        return false;
    }

    SDL_RenderClear(renderer);
    return true;
}

void kill () {
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}