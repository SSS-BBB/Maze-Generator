#pragma once
#include <vector>

struct Maze
{
	// number of cells
	int maze_width = 10;   // m
	int maze_height = 15;  // n

	// true if wall exists, fall if does not
	// at cell (i, j)
	// left wall = vertical_walls[i, j]
	// right wall = vertical_walls[i, j + 1]
	// top wall = horizontal_walls[i, j]
	// bottom wall = horizontal_walls[i + 1, j]
	std::vector<std::vector<bool>> horizontal_walls;
	std::vector<std::vector<bool>> vertical_walls;
};

void initMaze(Maze& mazeObj);

void setMaze(Maze& mazeObj);
void generateMaze(Maze& mazeObj);