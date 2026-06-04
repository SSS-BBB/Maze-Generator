#pragma once
#include "maze.hpp"
#include <queue>

struct Path
{
	std::vector<std::vector<CellPos>> parents; // parent[y][x] is the parent of cell (y, x)
};

// For Visualization Purpose
struct PathFinder
{
	MazeStatus status;

	CellPos startCell, endCell;

	std::vector<std::vector<bool>> cellExplored;
	std::vector<std::vector<int>> cellDistance;
	std::queue<CellPos> q;

	double delay = 0.0;
	std::chrono::time_point<std::chrono::high_resolution_clock> startTime;
	std::chrono::time_point<std::chrono::high_resolution_clock> lastUpdateTime;

	std::vector<CellPos> exploredPos; // to visualize cells that has been explored
};

MazeStatus findShortPathInstantly(const Maze& mazeObj, Path& path, CellPos startCell, CellPos endCell);

// Visualization
void startFindShortPath(const Maze& mazeObj, PathFinder& pathFinder, Path& path);
void updateFindShortPath(const Maze& mazeObj, PathFinder& pathFinder, Path& path);