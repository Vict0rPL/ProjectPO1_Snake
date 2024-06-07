#include "GameBoard.h"
#include <cstdlib>
#include <ctime>

GameBoard::GameBoard() {
    generateFood();
    poo.x = -1;
	poo.y = -1;
    snake = std::make_unique<Snake>(BOARD_WIDTH / 2 - Snake::BODY_SEGMENT_SIZE, BOARD_HEIGHT / 2); // Directly initialize here
}

GameBoard::~GameBoard() {}

void GameBoard::setSnake(Snake* newSnake) {
    snake.reset(newSnake); // Reset to the new snake
}

void GameBoard::generateFood() {
    std::srand(std::time(nullptr)); // This should ideally be called only once
    food.x = std::rand() % (BOARD_WIDTH / FOOD_SEGMENT_SIZE) * FOOD_SEGMENT_SIZE;
    food.y = std::rand() % (BOARD_HEIGHT / FOOD_SEGMENT_SIZE) * FOOD_SEGMENT_SIZE;
}

void GameBoard::generatePoo() {
    std::srand(std::time(nullptr));  // This should ideally be called only once
    do {
        poo.x = std::rand() % (BOARD_WIDTH / FOOD_SEGMENT_SIZE) * FOOD_SEGMENT_SIZE;
        poo.y = std::rand() % (BOARD_HEIGHT / FOOD_SEGMENT_SIZE) * FOOD_SEGMENT_SIZE;
    } while (food.x == poo.x && food.y == poo.y || poo.x == snake->body[0].x && poo.y == snake->body[0].y); // Make sure the poo is not on the food nor snake
    
}
