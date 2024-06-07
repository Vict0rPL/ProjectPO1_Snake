#pragma once
#include <iostream>
#include <memory>
#include "Snake.h"

class GameBoard
{
public:
    GameBoard();
    ~GameBoard();
    std::unique_ptr<Snake> snake; // Use smart pointer here
    const int BOARD_WIDTH = 600;
    const int BOARD_HEIGHT = 600;
    const int FOOD_SEGMENT_SIZE = 20;

    struct FoodSegment {
        int x;
        int y;
    };

    FoodSegment food;
    FoodSegment poo;

    void generateFood();
    void generatePoo();
private:
    void setSnake(Snake* newSnake); // Clearer parameter naming
};
