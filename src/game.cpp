#include "entities.h"

SDL_Window* window = nullptr;
SDL_Renderer* renderer = nullptr;

bool loop () {
    static const unsigned char* keys = SDL_GetKeyboardState(NULL);
    SDL_Event event;
    SDL_Rect rect;
    static int mx0, my0, mx1, my1 = -1;

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
    SDL_RenderClear(renderer);
    SDL_RenderFillRect(renderer, &rect);

    bool keep_window_open = true;


    while (keep_window_open) {
        //event handling logic
        while (SDL_PollEvent(&event)) { 
                switch(event.type) 
                {
                    case SDL_QUIT: //close window if user wants to quit
                        keep_window_open = false;
                        break;
                }
            }
        SDL_SetRenderDrawColor(renderer, 233, 4, 122, 0.8);
        //create ball
        Ball ball(
            Vec2d((680.0 / 2.0f) - (BALL_WIDTH / 2.0f),
            (480.0 / 2.0f) - (BALL_WIDTH / 2.0f)));

        //draw balL
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