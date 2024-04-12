#include "Snake.h"


Snake::Snake(int startX, int startY) {
	direction = Direction::RIGHT; // Default direction
	growSnake(startX, startY); // Start position for snake
}


Snake::~Snake() {

}


void Snake::growSnake(int xpos, int ypos) {
	body.push_back({xpos, ypos});// 
}

void Snake::setDirection(int dir) {
	if (dir == 0 && direction != Direction::DOWN) {
		direction = Direction::UP;
	}
	else if (dir == 1 && direction != Direction::UP) {
		direction = Direction::DOWN;
	}
	else if (dir == 2 && direction != Direction::RIGHT) {
		direction = Direction::LEFT;
	}
	else if (dir == 3 && direction != Direction::LEFT) {
		direction = Direction::RIGHT;
	}
}



void Snake::moveSnake(int BOARD_WIDTH, int BOARD_HEIGHT) {
    // Move the rest of the body
    for (size_t i = body.size() - 1; i > 0; --i) {
        body[i] = body[i - 1];
    }

    // Move the head
    switch (direction) {
    case Direction::UP:    body[0].y -= BODY_SEGMENT_SIZE; break;
    case Direction::DOWN:  body[0].y += BODY_SEGMENT_SIZE; break;
    case Direction::LEFT:  body[0].x -= BODY_SEGMENT_SIZE; break;
    case Direction::RIGHT: body[0].x += BODY_SEGMENT_SIZE; break;
    }

    // Teleport the snake to the opposite side upon reaching the window's edge
    if (body[0].x < 0) body[0].x = BOARD_WIDTH - BODY_SEGMENT_SIZE;
    else if (body[0].x >= BOARD_WIDTH) body[0].x = 0;
    if (body[0].y < 0) body[0].y = BOARD_HEIGHT - BODY_SEGMENT_SIZE;
    else if (body[0].y >= BOARD_HEIGHT) body[0].y = 0;
}


bool Snake::checkCollision() {
    // Check if the snake has collided with itself
    for (size_t i = 1; i < body.size(); ++i) {
        if (body[0].x == body[i].x && body[0].y == body[i].y) {
            return true;
        }
    }
    return false;
}