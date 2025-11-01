#include "entities.h"
#include "gamestate.h"
#include <chrono>
#include <winsock2.h>
#include <windows.h>
#include <ws2tcpip.h>

SDL_Window* window = nullptr;
SDL_Renderer* renderer = nullptr;


//TODO --> read local input and move paddle1
bool loop (SOCKET ServerSock, int playerId) {
    SDL_Event event;
    bool keep_window_open = true;
    float dt = 0.016f; 

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
    
    InputState input{};
    input.up = false;
    input.down = false;
    GameState state{};

    //GAME LOOP-------------------------------------------
    auto lastTime = std::chrono::high_resolution_clock::now();

    while (keep_window_open) {
        //handling window events
        while (SDL_PollEvent(&event)) { 
                if (event.type == SDL_QUIT) { //close window if user wants to quit
                        keep_window_open = false;
                }
            }

            //handle LOCAL input
            const Uint8* keys = SDL_GetKeyboardState(nullptr);
            input.up = keys[SDL_SCANCODE_W];
            input.down = keys[SDL_SCANCODE_W];

            //send to server
            send(ServerSock, (char*)&input, sizeof(InputState), 0);

            int bytes = recv(ServerSock, (char*)&input, sizeof(InputState), 0);
            if (bytes <= 0) {
                printf("Server disconnected\n");
                break;
            }
            //update object positions
            ball.position = state.ballPos;
            paddle1.position = state.paddle1Pos;
            paddle2.position = state.paddle2Pos;

          
            //------Rendering-------
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            SDL_RenderClear(renderer);
            //paddle colors set to pink
            SDL_SetRenderDrawColor(renderer, 233, 4, 122, 0.8);
            
            //draw objects to window
            paddle1.Draw(renderer);
            paddle2.Draw(renderer);
            ball.Draw(renderer);

            SDL_RenderPresent(renderer);
            SDL_Delay(16); //60fps
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