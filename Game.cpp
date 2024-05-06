#include "Game.h"


Game::Game() {
    if (!initSDL()) {
        std::cerr << "Failed to initialize SDL." << std::endl;
        exit(1); // Exiting if SDL fails to initialize
    }

    // Initialize the game state
    gameRunning = true;
    currentState = MAIN_MENU;
    board = new GameBoard();
}

Game::~Game() {
    closeSDL();
}


bool Game::initSDL() {
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL could not initialize! SDL Error: " << SDL_GetError() << std::endl;
        return false;
    }

    // Set texture filtering to linear
    if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1")) {
        std::cerr << "Warning: Linear texture filtering not enabled!" << std::endl;
    }

    // Create window
    window = SDL_CreateWindow("SDL Snake Game", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
    if (window == nullptr) {
        std::cerr << "Window could not be created! SDL Error: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return false;
    }

    // Create renderer for window
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (renderer == nullptr) {
        std::cerr << "Renderer could not be created! SDL Error: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        return false;
    }

    // Initialize renderer color to black
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

    // Initialize PNG loading with SDL_image
    int imgFlags = IMG_INIT_PNG;
    if (!(IMG_Init(imgFlags) & imgFlags)) {
        std::cerr << "SDL_image could not initialize! SDL_image Error: " << IMG_GetError() << std::endl;
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return false;
    }
    // Initialize SDL_ttf 
    TTF_Init();
    font = TTF_OpenFont("./Pixelletters.ttf", 24); // Adjust path and size accordingly
    if (font == nullptr) {
        std::cerr << "Failed to load font! SDL_ttf Error: " << TTF_GetError() << std::endl;
    }


    // Initialization successful
    return true;
}


void Game::closeSDL() {
    // Destroy the renderer
    if (renderer != nullptr) {
        SDL_DestroyRenderer(renderer);
        renderer = nullptr;
    }

    // Destroy the window
    if (window != nullptr) {
        SDL_DestroyWindow(window);
        window = nullptr;
    }

    // Quit SDL_image
    IMG_Quit();
    // Quit SDL_ttf
    TTF_Quit();
    // Quit SDL
    SDL_Quit();
}

void Game::showMainMenu() {
    // Clear the previous render
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Black background
    SDL_RenderClear(renderer);

    // Render menu text
    SDL_Color textColor = { 255, 255, 255, 255 }; // White color
    SDL_Surface* surfaceMessage = TTF_RenderText_Solid(font, "Press 'S' to Start, 'L' for Leaderboard", textColor);
    SDL_Texture* message = SDL_CreateTextureFromSurface(renderer, surfaceMessage);

    // Define the rectangle to position text
    SDL_Rect message_rect;
    message_rect.x = (WINDOW_WIDTH - surfaceMessage->w) / 2; // Center horizontally
    message_rect.y = (WINDOW_HEIGHT - surfaceMessage->h) / 2; // Center vertically
    message_rect.w = surfaceMessage->w;
    message_rect.h = surfaceMessage->h;

    // Copy the texture to the renderer
    SDL_RenderCopy(renderer, message, NULL, &message_rect);

    // Free the resources
    SDL_FreeSurface(surfaceMessage);
    SDL_DestroyTexture(message);

    // Handle input
    SDL_Event e;
    while (SDL_PollEvent(&e) != 0) {
        if (e.type == SDL_QUIT) {
            currentState = GAME_OVER;
            gameRunning = false;
        }
        else if (e.type == SDL_KEYDOWN) {
            switch (e.key.keysym.sym) {
            case SDLK_s:
                currentState = IN_GAME;
                resetGame();
                break;
            case SDLK_l:
                // Call a function to show the leaderboard
                currentState = LEADERBOARD;
                break;
            }
        }
    }

    SDL_RenderPresent(renderer);
    SDL_Delay(100);
}


#include <SDL_ttf.h>

void Game::showGameOverScreen() {
    // Set the color and clear the screen
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Black background
    SDL_RenderClear(renderer);

    // Define text color
    SDL_Color textColor = { 255, 255, 255, 255 }; // White color

    // Render "Game Over" text
    SDL_Surface* surfaceGameOver = TTF_RenderText_Solid(font, "Game Over", textColor);
    SDL_Texture* textureGameOver = SDL_CreateTextureFromSurface(renderer, surfaceGameOver);
    SDL_Rect gameOverRect = { (WINDOW_WIDTH - surfaceGameOver->w) / 2, (WINDOW_HEIGHT / 2) - 50, surfaceGameOver->w, surfaceGameOver->h };
    SDL_RenderCopy(renderer, textureGameOver, NULL, &gameOverRect);

    // Render "Press 'R' to Restart, 'Q' to Quit" text
    SDL_Surface* surfaceRestart = TTF_RenderText_Solid(font, "Press 'R' to Restart, 'Q' to Quit", textColor);
    SDL_Texture* textureRestart = SDL_CreateTextureFromSurface(renderer, surfaceRestart);
    SDL_Rect restartRect = { (WINDOW_WIDTH - surfaceRestart->w) / 2, (WINDOW_HEIGHT / 2) + 10, surfaceRestart->w, surfaceRestart->h };
    SDL_RenderCopy(renderer, textureRestart, NULL, &restartRect);

    // Free surfaces and textures
    SDL_FreeSurface(surfaceGameOver);
    SDL_DestroyTexture(textureGameOver);
    SDL_FreeSurface(surfaceRestart);
    SDL_DestroyTexture(textureRestart);

    // Handle input
    SDL_Event e;
    while (SDL_PollEvent(&e) != 0) {
        if (e.type == SDL_QUIT) {
            currentState = GAME_OVER;
            gameRunning = false;
        }
        else if (e.type == SDL_KEYDOWN) {
            switch (e.key.keysym.sym) {
            case SDLK_r:
                currentState = IN_GAME;
                resetGame();
                break;
            case SDLK_q:
                gameRunning = false;
                break;
            }
        }
    }

    // Present the render
    SDL_RenderPresent(renderer);
    SDL_Delay(100);
}


void Game::resetGame() {
    delete board;
    board = new GameBoard();
}



void Game::handleEvents(SDL_Event& e) {
    if (e.type == SDL_KEYDOWN) {
        switch (e.key.keysym.sym) {
        case SDLK_UP:     board->snake->setDirection(0);  break;
        case SDLK_DOWN:   board->snake->setDirection(1); break;
        case SDLK_LEFT:   board->snake->setDirection(2); break;
        case SDLK_RIGHT:  board->snake->setDirection(3); break;
        }
    }
}


void Game::update() {
    board->snake->moveSnake(board->BOARD_WIDTH, board->BOARD_HEIGHT);
    if (board->snake->body[0].x == board->food.x && board->snake->body[0].y == board->food.y) {
      //Grow Snake (getting position of the last Vector element)
        board->snake->growSnake(board->snake->body[board->snake->body.size() - 1].x, board->snake->body[board->snake->body.size() - 1].y);
        board->generateFood();
    }
    else if (board->snake->checkCollision()) {
		currentState = GAME_OVER;
    }
}

void Game::render() {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Black background
    SDL_RenderClear(renderer);

    // Draw food
    SDL_Rect foodRect = { board->food.x, board->food.y, board->FOOD_SEGMENT_SIZE, board->FOOD_SEGMENT_SIZE };
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255); // Red food
    SDL_RenderFillRect(renderer, &foodRect);

    // Draw snake
    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255); // Green snake
    for (const auto& segment : board->snake->body) {
        SDL_Rect rect = { segment.x, segment.y, board->snake->BODY_SEGMENT_SIZE, board->snake->BODY_SEGMENT_SIZE };
        SDL_RenderFillRect(renderer, &rect);
    }

    SDL_RenderPresent(renderer);
}


void Game::mainLoop() {
    SDL_Event e;
    while (SDL_PollEvent(&e) != 0) {
        if (e.type == SDL_QUIT) {
            gameRunning = false;
        }
        handleEvents(e);
    }

    update();
    render();
    SDL_Delay(100); // Adjust to change the game speed
}

void Game::run() {
    while (gameRunning) {
        switch (currentState) {
        case MAIN_MENU:
            showMainMenu();
            break;
        case IN_GAME:
            mainLoop();
            break;
        case GAME_OVER:
            showGameOverScreen();
            break;
        }
    }
}

