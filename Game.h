#pragma once
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <SDL_main.h>
#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <algorithm>
#include <SDL_mixer.h>
#include "GameBoard.h"

/* Short desc.
Long desc.
\param p1_ 
return _*/
class Game {
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
    bool pooSpawned = false;

    GameBoard* board;
    TTF_Font* font;

    std::string playerName;
    int playerScore;
    std::vector<std::pair<int, std::string>> leaderboard;

    SDL_Texture* fieldTexture = nullptr;
    SDL_Texture* snakeHeadUpTexture = nullptr;
    SDL_Texture* snakeHeadDownTexture = nullptr;
    SDL_Texture* snakeHeadLeftTexture = nullptr;
    SDL_Texture* snakeHeadRightTexture = nullptr;
    SDL_Texture* snakeBodyTexture = nullptr;
    SDL_Texture* snakeTailTexture = nullptr;
    SDL_Texture* foodTexture = nullptr;
    SDL_Texture* pooTexture = nullptr;
    SDL_Texture* startScreenTexture = nullptr;
    SDL_Texture* endScreenTexture = nullptr;

    Mix_Music* startScreenMusic = nullptr;
    Mix_Music* inGameMusic = nullptr;
    Mix_Music* gameOverMusic = nullptr;

    Mix_Chunk* pingSound = nullptr;
    Mix_Chunk* dedSound = nullptr;

    bool initSDL();
    void loadTextures();
    void loadMusic();
    void closeSDL();
    void showMainMenu();
    void showGameOverScreen();
    void showLeaderboard();
    void resetGame();
    void mainLoop();
    void render();
    void update();
    void handleEvents(SDL_Event& e);
    void loadLeaderboard();
    void saveLeaderboard();
    void getPlayerName();
    void addScoreToLeaderboard(int score, const std::string& name);
};
