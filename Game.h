#pragma once
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_main.h>
#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include "GameBoard.h"

class Game
{
public:
    Game();
    ~Game();
    void run();

private:
    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;
    const int WINDOW_WIDTH = 600;
    const int WINDOW_HEIGHT = 600;
    
    bool gameRunning;

    GameBoard* board;

    bool initSDL();
    SDL_Texture* loadTexture(const std::string& path);
    void closeSDL();
    void mainLoop();
    void render();
    void update();
    void handleEvents(SDL_Event& e);
    
};

