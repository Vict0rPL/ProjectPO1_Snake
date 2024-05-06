#pragma once
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
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
    enum GameState {
        MAIN_MENU,
        IN_GAME,
        GAME_OVER,
        LEADERBOARD
    };
    GameState currentState;
    bool gameRunning;

    GameBoard* board;
    TTF_Font* font;

    bool initSDL();
    SDL_Texture* loadTexture(const std::string& path);
    void closeSDL();
    void showMainMenu();
    void showGameOverScreen();
	void resetGame();
    void mainLoop();
    void render();
    void update();
    void handleEvents(SDL_Event& e);
    
};

