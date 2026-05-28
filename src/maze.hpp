#pragma once
#include <vector>
#include <string>

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

enum StatusType
{
	IDLE, ERROR, WARNING, PROCESSING, SUCCESSFUL
};

struct MazeStatus
{
	StatusType statusType = IDLE;
	std::string statusMessage = "";
};

struct MazeGenerator
{
	MazeStatus mazeStatus;
	std::vector<std::vector<bool>> cellVisited;
};

void initMaze(Maze& mazeObj);

MazeStatus setMaze(Maze& mazeObj);
MazeStatus generateMaze(Maze& mazeObj);