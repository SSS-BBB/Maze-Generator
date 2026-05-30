#pragma once
#include <vector>
#include <stack>
#include <string>
#include <chrono>

struct CellPos
{
	int x = 0;
	int y = 0;
};

struct Maze
{
	// number of cells
	int maze_width = 0;   // m
	int maze_height = 0;  // n

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
	IDLE, ERROR, PROCESSING, SUCCESSFUL
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
	std::stack<CellPos> cellStack;
	std::chrono::time_point<std::chrono::high_resolution_clock> startTime;
	std::chrono::time_point<std::chrono::high_resolution_clock> lastUpdateTime;
	double delay = 100.0f; // ms

	// visualization
	std::vector<CellPos> visitedPos;
	std::vector<CellPos> deadEndPos;
};

void initMaze(Maze& mazeObj);

MazeStatus setMaze(Maze& mazeObj);
MazeStatus generateMazeInstantly(Maze& mazeObj);

void startGeneration(MazeGenerator& mazeGenerator, Maze& mazeObj);
void updateGeneration(MazeGenerator& mazeGenerator, Maze& mazeObj);

MazeStatus saveMaze(const Maze& mazeObj, std::string filename);
MazeStatus loadMaze(Maze& mazeObj, std::string filename);