#include "SDL.h"
#include "SDL_image.h"
#include "SDL_ttf.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

#define WIDTH 1920
#define HEIGHT 1080
#define velocityX_pipe -1000.0f
#define PIPE_GAP 250
#define PIPE_WIDTH 80
#define SPAWN_INTERVAL 1000 // in milliseconds
#define FPS 60
#define MAX_PIPES 4

float velocityY_bird = 0.0f;
const float gravity = 98.0f;
const float jumpForce = -1100.0f;

SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;
SDL_Texture *birdTexture = NULL;
SDL_Texture *pipeTexture = NULL;

typedef struct
{
    SDL_Rect upPipe;
    SDL_Rect downPipe;
    bool scored;
} Pipe;

Pipe pipes[MAX_PIPES];
int numPipes = 0;
Uint32 lastPipeSpawnTime = 0;
int score = 0;

SDL_Texture *scoreTexture = NULL;
SDL_Rect scoreRect;

void initialize();
void cleanup();
SDL_Texture *load_texture(const char *file_path);
void render_sprite(SDL_Texture *texture, SDL_Rect *destRect);
void create_new_pipe();
void update_pipes(float deltaTime);
void render_pipes();
bool check_collision(SDL_Rect a, SDL_Rect b);
void reset_game(float *y_bird, SDL_Rect *destRect_bird);
void update_score_texture();

int main(void)
{
    srand(time(NULL) + clock());
    initialize();

    pipeTexture = load_texture("../assets/pipe.png");
    if (!pipeTexture)
    {
        cleanup();
        return -1;
    }

    birdTexture = load_texture("../assets/bird.png");
    if (!birdTexture)
    {
        cleanup();
        return -1;
    }

    // Bird setup
    int textW_bird, textH_bird;
    SDL_QueryTexture(birdTexture, NULL, NULL, &textW_bird, &textH_bird);

    float scale_bird = 0.05f * fmin((float)WIDTH / textW_bird, (float)HEIGHT / textH_bird);
    int newWidth_bird = textW_bird * scale_bird, newHeight_bird = textH_bird * scale_bird;
    float y_bird = (HEIGHT / 2) - (newHeight_bird / 2);
    SDL_Rect destRect_bird = {150, y_bird, newWidth_bird, newHeight_bird};

    bool running = true;
    SDL_Event event;
    Uint32 lastTime = SDL_GetTicks64();
    const Uint32 frameDelay = 1000 / FPS;

    update_score_texture();

    while (running)
    {
        Uint32 frameStart = SDL_GetTicks64();
        Uint32 currentTime = SDL_GetTicks64();
        float deltaTime = (currentTime - lastTime) / 1000.0f;
        lastTime = currentTime;

        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
                running = false;
            if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_SPACE)
            {
                velocityY_bird = jumpForce;
            }
        }

        // Pipe spawn logic
        if (currentTime - lastPipeSpawnTime > SPAWN_INTERVAL)
        {
            lastPipeSpawnTime = currentTime;
            create_new_pipe();
        }

        update_pipes(deltaTime);

        // Bird movement and bounds checking
        velocityY_bird += gravity;
        y_bird += velocityY_bird * deltaTime;
        if (y_bird + newHeight_bird > HEIGHT)
        {
            reset_game(&y_bird, &destRect_bird);
        }
        else if (y_bird < 0)
        {
            y_bird = 0;
            velocityY_bird = 0;
        }
        destRect_bird.y = y_bird;

        // Rendering
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        SDL_RenderCopy(renderer, scoreTexture, NULL, &scoreRect);
        render_sprite(birdTexture, &destRect_bird);
        render_pipes();

        // Collision detection
        for (int i = 0; i < numPipes; i++)
        {
            if (check_collision(destRect_bird, pipes[i].upPipe) ||
                check_collision(destRect_bird, pipes[i].downPipe))
            {
                printf("Collision detected!\n");
                reset_game(&y_bird, &destRect_bird);
                break;
            }
        }

        SDL_RenderPresent(renderer);

        // Cap frame rate
        Uint32 frameEnd = SDL_GetTicks64();
        Uint32 frameTime = frameEnd - frameStart;
        if (frameDelay > frameTime)
        {
            SDL_Delay(frameDelay - frameTime);
        }
        else
        {
            printf("bok");
        }
    }

    cleanup();
    return 0;
}

void create_new_pipe()
{
    if (numPipes >= MAX_PIPES)
        return;

    int pipeHeight = rand() % (HEIGHT - PIPE_GAP);
    pipes[numPipes].upPipe = (SDL_Rect){WIDTH, 0, PIPE_WIDTH, pipeHeight};
    pipes[numPipes].downPipe = (SDL_Rect){WIDTH, pipeHeight + PIPE_GAP, PIPE_WIDTH, HEIGHT - (pipeHeight + PIPE_GAP)};
    pipes[numPipes].scored = false;
    numPipes++;
}

void update_pipes(float deltaTime)
{
    for (int i = 0; i < numPipes; i++)
    {
        pipes[i].upPipe.x += velocityX_pipe * deltaTime;
        pipes[i].downPipe.x += velocityX_pipe * deltaTime;

        if (!pipes[i].scored && pipes[i].upPipe.x + PIPE_WIDTH < 150)
        {
            pipes[i].scored = true;
            score++;
            printf("Score: %d\n", score);
            update_score_texture();
        }

        if (pipes[i].upPipe.x + PIPE_WIDTH <= 0)
        {
            for (int j = i; j < numPipes - 1; j++)
            {
                pipes[j] = pipes[j + 1];
            }
            numPipes--;
            i--; // Stay at the same index after shifting
        }
    }
}

void render_pipes()
{
    for (int i = 0; i < numPipes; i++)
    {
        SDL_RenderCopyEx(renderer, pipeTexture, NULL, &pipes[i].upPipe, 0, NULL, SDL_FLIP_VERTICAL);
        SDL_RenderCopyEx(renderer, pipeTexture, NULL, &pipes[i].downPipe, 0, NULL, SDL_FLIP_NONE);
    }
}

void update_score_texture()
{
    if (scoreTexture)
    {
        SDL_DestroyTexture(scoreTexture);
    }

    TTF_Font *font = TTF_OpenFont("../assets/font.ttf", 28);
    if (!font)
    {
        fprintf(stderr, "Font Loading Failed: %s\n", TTF_GetError());
        return;
    }

    SDL_Color color = {255, 255, 255, 255};
    char scoreText[10];
    snprintf(scoreText, sizeof(scoreText), "Score: %d", score);

    SDL_Surface *textSurface = TTF_RenderText_Solid(font, scoreText, color);
    if (!textSurface)
    {
        fprintf(stderr, "Text Rendering Failed: %s\n", TTF_GetError());
        TTF_CloseFont(font);
        return;
    }

    scoreTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
    SDL_FreeSurface(textSurface);
    TTF_CloseFont(font);

    scoreRect = (SDL_Rect){WIDTH - 300, 50, 200, 50};
}

bool check_collision(SDL_Rect a, SDL_Rect b)
{
    return (a.x + a.w > b.x && a.x < b.x + b.w && a.y + a.h > b.y && a.y < b.y + b.h);
}

void reset_game(float *y_bird, SDL_Rect *destRect_bird)
{
    *y_bird = (HEIGHT / 2) - (destRect_bird->h / 2);
    destRect_bird->y = *y_bird;
    velocityY_bird = 0;
    numPipes = 0;
    score = 0;
    update_score_texture();
}

void initialize()
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        fprintf(stderr, "SDL Initialization Failed: %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }

    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG))
    {
        fprintf(stderr, "SDL_image Initialization Failed: %s\n", IMG_GetError());
        SDL_Quit();
        exit(EXIT_FAILURE);
    }

    if (TTF_Init() == -1)
    {
        fprintf(stderr, "TTF Initialization Failed: %s\n", TTF_GetError());
        SDL_Quit();
        exit(EXIT_FAILURE);
    }

    window = SDL_CreateWindow("Flappy Bird Clone", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, SDL_WINDOW_SHOWN);
    if (!window)
    {
        fprintf(stderr, "Window Creation Failed: %s\n", SDL_GetError());
        cleanup();
        exit(EXIT_FAILURE);
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer)
    {
        fprintf(stderr, "Renderer Creation Failed: %s\n", SDL_GetError());
        cleanup();
        exit(EXIT_FAILURE);
    }
}

SDL_Texture *load_texture(const char *file_path)
{
    SDL_Texture *texture = IMG_LoadTexture(renderer, file_path);
    if (!texture)
    {
        fprintf(stderr, "Texture Loading Failed: %s\n", SDL_GetError());
    }
    return texture;
}

void render_sprite(SDL_Texture *texture, SDL_Rect *destRect)
{
    SDL_RenderCopy(renderer, texture, NULL, destRect);
}

void cleanup()
{
    if (scoreTexture)
        SDL_DestroyTexture(scoreTexture);
    if (birdTexture)
        SDL_DestroyTexture(birdTexture);
    if (pipeTexture)
        SDL_DestroyTexture(pipeTexture);
    if (renderer)
        SDL_DestroyRenderer(renderer);
    if (window)
        SDL_DestroyWindow(window);

    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
}
