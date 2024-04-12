#include "Game.h"

Game::Game() {
    if (!initSDL()) {
        std::cerr << "Failed to initialize SDL." << std::endl;
        exit(1); // Exiting if SDL fails to initialize
    }

    // Initialize the game state
    gameRunning = true;
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

    // Quit SDL
    SDL_Quit();
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
        board->snake->growSnake(0, 0); // Grow the snake, for now at 0,0 point, something must be done about it as it's visible to the user
        board->generateFood();
    }
    else if (board->snake->checkCollision()) {
        gameRunning = false; // End game on collision
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
        mainLoop();
    }
}
