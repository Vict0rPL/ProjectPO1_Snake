#pragma once
#include <iostream>
#include <vector>

class Snake
{
public:
	Snake(int startX, int startY);
	~Snake();
	static const int BODY_SEGMENT_SIZE = 20;
	enum class Direction { UP, DOWN, LEFT, RIGHT } direction;
	void setDirection(int dir);
	void moveSnake(int BOARD_WIDTH, int BOARD_HEIGHT);
	void growSnake(int xpos, int ypos);
	bool checkCollision();
	struct BodySegment {
		int x;
		int y;
	};

	std::vector<BodySegment> body;


private:
	
	

	
	
	
};

