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
	std::queue<Path> q;
};

MazeStatus findShortPathInstantly(const Maze& mazeObj, Path& path, CellPos startCell, CellPos endCell);