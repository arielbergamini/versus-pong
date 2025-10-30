
//SDL Window that renders paddles & ball, closes on quit.
//TODO --> Draw 2 paddles & 1 ball, add fixed timestamp. rendering loop


#include <SDL2/SDL.h>
#include <iostream>

bool init();
void kill();
bool loop();

SDL_Window *window;
SDL_Renderer *renderer;

int main (int argc, char* argv[]) {
    
    if (!init()) return 1;

    //wait before processing next frame
    while (loop()) {
        SDL_Delay(10);
    }
    kill();
    return 0;
}


bool loop () {
    static const unsigned char* keys = SDL_GetKeyboardState(NULL);
    SDL_Event event;
    SDL_Rect rect;
    static int mx0, my0, mx1, my1 = -1;

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderClear(renderer);
    SDL_RenderFillRect(renderer, &rect);

    bool keep_window_open = true;

    while ((keep_window_open) && (SDL_PollEvent(&event) !=0 ))
    {
            switch(event.type) 
            {
                case SDL_QUIT: //close window if user wants to quit
                    keep_window_open = false;
                    break;
                case SDL_MOUSEBUTTONDOWN:
                    mx0 = event.button.x;
                    my0 = event.button.y;
                    break;
                case SDL_MOUSEMOTION:
                    mx1 = event.button.x;
                    my1 = event.button.y;
                    break;
                case SDL_MOUSEBUTTONUP:
                    mx0 = my0 = mx1 = my1 = -1;
                    break;
            }
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        // Test key states - this could also be done with events
        if ( keys[SDL_SCANCODE_1] ) {
            SDL_RenderDrawPoint(renderer, 10, 10 );
        }
        if ( keys[SDL_SCANCODE_2] ) {
            SDL_RenderDrawLine(renderer, 10, 20, 10, 100 );
        }
        if ( keys[SDL_SCANCODE_3] ) {
            rect.x = 20;
            rect.y = 20;
            rect.w = 100;
            rect.h = 100;
            SDL_RenderFillRect(renderer, &rect);
        }

        // render mouse ptr
        if ( mx0 != -1 ) {
            rect.x = mx0;
            rect.y = my0;
            rect.w = mx1 - mx0;
            rect.h = my1 - my0;
		    SDL_RenderFillRect(renderer, &rect);
	    }

        SDL_RenderPresent(renderer);

        return keep_window_open;
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

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderClear(renderer);
    return true;
}

void kill () {
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}