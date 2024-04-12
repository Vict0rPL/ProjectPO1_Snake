#include "GameBoard.h"
#include <cstdlib>
#include <ctime>

GameBoard::GameBoard() {
    generateFood();
    snake = std::make_unique<Snake>(BOARD_WIDTH / 2 - Snake::BODY_SEGMENT_SIZE, BOARD_HEIGHT / 2); // Directly initialize here
}

GameBoard::~GameBoard() {}

void GameBoard::setSnake(Snake* newSnake) {
    snake.reset(newSnake); // Reset to the new snake, assuming ownership transfer
}

void GameBoard::generateFood() {
    std::srand(std::time(nullptr)); // This should ideally be called only once
    food.x = std::rand() % (BOARD_WIDTH / FOOD_SEGMENT_SIZE) * FOOD_SEGMENT_SIZE;
    food.y = std::rand() % (BOARD_HEIGHT / FOOD_SEGMENT_SIZE) * FOOD_SEGMENT_SIZE;
}
