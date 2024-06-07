#ifdef DEBUG
#define DEBUG_MSG(str) do { std::cout << str << std::endl; } while( false )
#else
#define DEBUG_MSG(str) do { } while ( false )
#endif
#include "Game.h"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

Game::Game() {
    if (!initSDL()) {
        std::cerr << "Failed to initialize SDL." << std::endl;
        exit(1); // Exiting if SDL fails to initialize
    }

    // Initialize the game state
    gameRunning = true;
    currentState = MAIN_MENU;
    board = new GameBoard();
    loadLeaderboard();
    loadTextures(); // Load textures for snake and food
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
    if (TTF_Init() == -1) {
        std::cerr << "SDL_ttf could not initialize! SDL_ttf Error: " << TTF_GetError() << std::endl;
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return false;
    }
    font = TTF_OpenFont("./Pixelletters.ttf", 24); // Adjust path and size accordingly
    if (font == nullptr) {
        std::cerr << "Failed to load font! SDL_ttf Error: " << TTF_GetError() << std::endl;
    }

    // Initialization successful
    return true;
}

void Game::loadTextures() {
    // Load the snake head textures for each direction
    SDL_Surface* tempSurface = IMG_Load("snake_head_up.png");
    if (tempSurface == nullptr) {
        std::cerr << "Failed to load snake head up texture! IMG Error: " << IMG_GetError() << std::endl;
        return;
    }
    snakeHeadUpTexture = SDL_CreateTextureFromSurface(renderer, tempSurface);
    SDL_FreeSurface(tempSurface);

    tempSurface = IMG_Load("snake_head_down.png");
    if (tempSurface == nullptr) {
        std::cerr << "Failed to load snake head down texture! IMG Error: " << IMG_GetError() << std::endl;
        return;
    }
    snakeHeadDownTexture = SDL_CreateTextureFromSurface(renderer, tempSurface);
    SDL_FreeSurface(tempSurface);

    tempSurface = IMG_Load("snake_head_left.png");
    if (tempSurface == nullptr) {
        std::cerr << "Failed to load snake head left texture! IMG Error: " << IMG_GetError() << std::endl;
        return;
    }
    snakeHeadLeftTexture = SDL_CreateTextureFromSurface(renderer, tempSurface);
    SDL_FreeSurface(tempSurface);

    tempSurface = IMG_Load("snake_head_right.png");
    if (tempSurface == nullptr) {
        std::cerr << "Failed to load snake head right texture! IMG Error: " << IMG_GetError() << std::endl;
        return;
    }
    snakeHeadRightTexture = SDL_CreateTextureFromSurface(renderer, tempSurface);
    SDL_FreeSurface(tempSurface);

    // Load the snake body texture
    tempSurface = IMG_Load("snake_body.png");
    if (tempSurface == nullptr) {
        std::cerr << "Failed to load snake body texture! IMG Error: " << IMG_GetError() << std::endl;
        return;
    }
    snakeBodyTexture = SDL_CreateTextureFromSurface(renderer, tempSurface);
    SDL_FreeSurface(tempSurface);

    // Load the snake tail texture
    tempSurface = IMG_Load("snake_body.png");
    if (tempSurface == nullptr) {
        std::cerr << "Failed to load snake tail texture! IMG Error: " << IMG_GetError() << std::endl;
        return;
    }
    snakeTailTexture = SDL_CreateTextureFromSurface(renderer, tempSurface);
    SDL_FreeSurface(tempSurface);

    // Load the food texture
    tempSurface = IMG_Load("food.png");
    if (tempSurface == nullptr) {
        std::cerr << "Failed to load food texture! IMG Error: " << IMG_GetError() << std::endl;
        return;
    }
    foodTexture = SDL_CreateTextureFromSurface(renderer, tempSurface);
    SDL_FreeSurface(tempSurface);

    // Load the field texture
    tempSurface = IMG_Load("field.png");
    if (tempSurface == nullptr) {
        std::cerr << "Failed to load field texture! IMG Error: " << IMG_GetError() << std::endl;
        return;
    }
    fieldTexture = SDL_CreateTextureFromSurface(renderer, tempSurface);
    SDL_FreeSurface(tempSurface);

    // Load the starting screen texture
    tempSurface = IMG_Load("start_screen.png");
    if (tempSurface == nullptr) {
        std::cerr << "Failed to load starting screen texture! IMG Error: " << IMG_GetError() << std::endl;
        return;
    }
    startScreenTexture = SDL_CreateTextureFromSurface(renderer, tempSurface);
    SDL_FreeSurface(tempSurface);

    // Load the game over screen texture
    tempSurface = IMG_Load("TheEND.png");
    if (tempSurface == nullptr) {
        std::cerr << "Failed to load starting screen texture! IMG Error: " << IMG_GetError() << std::endl;
        return;
    }
    endScreenTexture = SDL_CreateTextureFromSurface(renderer, tempSurface);
    SDL_FreeSurface(tempSurface);
}

void Game::closeSDL() {
    // Destroy the textures
    if (snakeHeadUpTexture != nullptr) {
        SDL_DestroyTexture(snakeHeadUpTexture);
        snakeHeadUpTexture = nullptr;
    }
    if (snakeHeadDownTexture != nullptr) {
        SDL_DestroyTexture(snakeHeadDownTexture);
        snakeHeadDownTexture = nullptr;
    }
    if (snakeHeadLeftTexture != nullptr) {
        SDL_DestroyTexture(snakeHeadLeftTexture);
        snakeHeadLeftTexture = nullptr;
    }
    if (snakeHeadRightTexture != nullptr) {
        SDL_DestroyTexture(snakeHeadRightTexture);
        snakeHeadRightTexture = nullptr;
    }
    if (snakeBodyTexture != nullptr) {
        SDL_DestroyTexture(snakeBodyTexture);
        snakeBodyTexture = nullptr;
    }
    if (snakeTailTexture != nullptr) {
        SDL_DestroyTexture(snakeTailTexture);
        snakeTailTexture = nullptr;
    }
    if (foodTexture != nullptr) {
        SDL_DestroyTexture(foodTexture);
        foodTexture = nullptr;
    }
    if (fieldTexture != nullptr) {
        SDL_DestroyTexture(fieldTexture);
        fieldTexture = nullptr;
    }
    if (startScreenTexture != nullptr) {
        SDL_DestroyTexture(startScreenTexture);
        startScreenTexture = nullptr;
    }
    if (endScreenTexture != nullptr) {
        SDL_DestroyTexture(endScreenTexture);
        endScreenTexture = nullptr;
    }

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
    SDL_Event e;
    bool menuActive = true;

    while (menuActive) {
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Black background
        SDL_RenderClear(renderer);

        // Draw the start screen
        SDL_Rect startScreenRect = { 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT };
        SDL_RenderCopy(renderer, startScreenTexture, NULL, &startScreenRect);

        // Render the prompt text
        SDL_Color textColor = { 255, 255, 255, 255 }; // White color
        SDL_Surface* surfacePrompt = TTF_RenderText_Solid(font, "Press 'S' to Start, 'L' for Leaderboard", textColor);
        SDL_Texture* texturePrompt = SDL_CreateTextureFromSurface(renderer, surfacePrompt);

        SDL_Rect promptRect = { 50, 50, surfacePrompt->w, surfacePrompt->h };
        SDL_RenderCopy(renderer, texturePrompt, NULL, &promptRect);

        SDL_FreeSurface(surfacePrompt);
        SDL_DestroyTexture(texturePrompt);

        SDL_RenderPresent(renderer);

        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                currentState = GAME_OVER;
                gameRunning = false;
                return;
            }
            else if (e.type == SDL_KEYDOWN) {
                switch (e.key.keysym.sym) {
                case SDLK_s:
                    getPlayerName();
                    if (!playerName.empty()) {
                        currentState = IN_GAME;
                        resetGame();
                        return;
                    }
                    break;
                case SDLK_l:
                    currentState = LEADERBOARD;
                    return;
                }
            }
        }
    }
}

void Game::getPlayerName() {
    SDL_StartTextInput();
    playerName = " "; // Reset player name to an empty string
    SDL_Event e;

    bool inputActive = true;
    while (inputActive) {
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Black background
        SDL_RenderClear(renderer);

        // Render the prompt text
        SDL_Color textColor = { 255, 255, 255, 255 }; // White color
        SDL_Surface* surfacePrompt = TTF_RenderText_Solid(font, "Enter your name: ", textColor);

        SDL_Texture* texturePrompt = SDL_CreateTextureFromSurface(renderer, surfacePrompt);

        SDL_Rect promptRect = { 50, 50, surfacePrompt->w, surfacePrompt->h };
        SDL_RenderCopy(renderer, texturePrompt, NULL, &promptRect);

        SDL_FreeSurface(surfacePrompt);
        SDL_DestroyTexture(texturePrompt);

        // Render the current input text
        SDL_Surface* surfaceInput = TTF_RenderText_Solid(font, playerName.c_str(), textColor);
        if (!surfaceInput) {
            std::cerr << "TTF_RenderText_Solid error: " << TTF_GetError() << std::endl;
            break;
        }
        SDL_Texture* textureInput = SDL_CreateTextureFromSurface(renderer, surfaceInput);
        if (!textureInput) {
            std::cerr << "SDL_CreateTextureFromSurface error: " << SDL_GetError() << std::endl;
            SDL_FreeSurface(surfaceInput);
            break;
        }

        SDL_Rect inputRect = { 50, 100, surfaceInput->w, surfaceInput->h };
        SDL_RenderCopy(renderer, textureInput, NULL, &inputRect);

        SDL_FreeSurface(surfaceInput);
        SDL_DestroyTexture(textureInput);

        SDL_RenderPresent(renderer);

        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                currentState = GAME_OVER;
                gameRunning = false;
                SDL_StopTextInput();
                return;
            }
            else if (e.type == SDL_TEXTINPUT) {
                // Append the character to playerName
                playerName += e.text.text;
            }
            else if (e.type == SDL_KEYDOWN) {
                if (e.key.keysym.sym == SDLK_BACKSPACE && playerName.length() > 0) {
                    // Handle backspace
                    playerName.pop_back();
                }
                else if (e.key.keysym.sym == SDLK_RETURN) {
                    // Enter key pressed, check if name is not empty
                    if (!playerName.empty()) {
                        SDL_StopTextInput();
                        inputActive = false;
                    }
                }
            }
        }
    }
}

void Game::showGameOverScreen() {
    // Set the color and clear the screen
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Black background
    SDL_RenderClear(renderer);

    // Draw the start screen
    SDL_Rect endScreenRect = { 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT };
    SDL_RenderCopy(renderer, endScreenTexture, NULL, &endScreenRect);

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
                return;
            case SDLK_q:
                gameRunning = false;
                return;
            }
        }
    }

    // Save the score to the leaderboard
    addScoreToLeaderboard(playerScore, playerName);

    // Present the render
    SDL_RenderPresent(renderer);
    SDL_Delay(100);
}

void Game::showLeaderboard() {
    // Clear the previous render
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Black background
    SDL_RenderClear(renderer);

    // Define text color
    SDL_Color textColor = { 255, 255, 255, 255 }; // White color

    // Render leaderboard text
    int yOffset = 50;
    for (size_t i = 0; i < leaderboard.size(); ++i) {
        std::string entry = std::to_string(i + 1) + ". " + leaderboard[i].second + " - " + std::to_string(leaderboard[i].first);
        SDL_Surface* surfaceEntry = TTF_RenderText_Solid(font, entry.c_str(), textColor);
        SDL_Texture* textureEntry = SDL_CreateTextureFromSurface(renderer, surfaceEntry);

        SDL_Rect entryRect = { 50, yOffset, surfaceEntry->w, surfaceEntry->h };
        SDL_RenderCopy(renderer, textureEntry, NULL, &entryRect);

        SDL_FreeSurface(surfaceEntry);
        SDL_DestroyTexture(textureEntry);

        yOffset += 30;
    }

    SDL_RenderPresent(renderer);
    SDL_Delay(3000); // Show for 3 seconds before returning to main menu
    currentState = MAIN_MENU;
}

void Game::resetGame() {
    delete board;
    board = new GameBoard();
    playerScore = 0; // Reset score
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
        board->snake->growSnake(board->snake->body.back().x, board->snake->body.back().y);
        board->generateFood();
        playerScore += 10; // Increase score when food is eaten
    }
    else if (board->snake->checkCollision()) {
        currentState = GAME_OVER;
    }
}

void Game::render() {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Black background
    SDL_RenderClear(renderer);

    // Draw the field
    SDL_Rect fieldRect = { 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT };
    SDL_RenderCopy(renderer, fieldTexture, NULL, &fieldRect);

    // Draw food
    SDL_Rect foodRect = { board->food.x, board->food.y, board->FOOD_SEGMENT_SIZE, board->FOOD_SEGMENT_SIZE };
    SDL_RenderCopy(renderer, foodTexture, NULL, &foodRect);

    // Draw snake
    for (size_t i = 0; i < board->snake->body.size(); ++i) {
        SDL_Rect rect = { board->snake->body[i].x, board->snake->body[i].y, board->snake->BODY_SEGMENT_SIZE, board->snake->BODY_SEGMENT_SIZE };
        if (i == 0) {
            // Draw the head
            SDL_Texture* headTexture = nullptr;
            switch (board->snake->direction) {
            case Snake::Direction::UP: headTexture = snakeHeadUpTexture; break;
            case Snake::Direction::DOWN: headTexture = snakeHeadDownTexture; break;
            case Snake::Direction::LEFT: headTexture = snakeHeadLeftTexture; break;
            case Snake::Direction::RIGHT: headTexture = snakeHeadRightTexture; break;
            }
            SDL_RenderCopy(renderer, headTexture, NULL, &rect);
        }
        else if (i == board->snake->body.size() - 1) {
            // Draw the tail
            SDL_RenderCopy(renderer, snakeTailTexture, NULL, &rect);
        }
        else {
            // Draw the body
            SDL_RenderCopy(renderer, snakeBodyTexture, NULL, &rect);
        }
    }

    // Render the current score
    std::string scoreText = "Score: " + std::to_string(playerScore);
    SDL_Color textColor = { 255, 255, 255, 255 }; // White color
    SDL_Surface* scoreSurface = TTF_RenderText_Solid(font, scoreText.c_str(), textColor);
    SDL_Texture* scoreTexture = SDL_CreateTextureFromSurface(renderer, scoreSurface);
    SDL_Rect scoreRect = { 10, 10, scoreSurface->w, scoreSurface->h };
    SDL_RenderCopy(renderer, scoreTexture, NULL, &scoreRect);

    SDL_FreeSurface(scoreSurface);
    SDL_DestroyTexture(scoreTexture);

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
        case LEADERBOARD:
            showLeaderboard();
            break;
        }
    }
}

void Game::loadLeaderboard() {
    std::ifstream file("leaderboard.json");
    if (file.is_open()) {
        json j;
        file >> j;
        leaderboard.clear();
        for (const auto& entry : j) {
            leaderboard.push_back({ entry["score"], entry["name"] });
        }
        file.close();
    }
}

void Game::saveLeaderboard() {
    std::ofstream file("leaderboard.json");
    if (file.is_open()) {
        json j = json::array();
        for (const auto& entry : leaderboard) {
            j.push_back({ {"score", entry.first}, {"name", entry.second} });
        }
        file << j.dump(4);
        file.close();
    }
}

void Game::addScoreToLeaderboard(int score, const std::string& name) {
    // Create a temporary vector to hold the scores including the new one
    std::vector<std::pair<int, std::string>> tempLeaderboard = leaderboard;
    tempLeaderboard.push_back({ score, name });

    // Sort the temporary leaderboard
    std::sort(tempLeaderboard.begin(), tempLeaderboard.end(), [](const std::pair<int, std::string>& a, const std::pair<int, std::string>& b) {
        return a.first > b.first;
        });

    // Keep only the top 10 scores
    if (tempLeaderboard.size() > 10) {
        tempLeaderboard.resize(10);
    }

    // Update the actual leaderboard
    leaderboard = tempLeaderboard;

    saveLeaderboard();
}

