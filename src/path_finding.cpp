#include "path_finding.hpp"
#include <iostream>

bool leftWallExist(const Maze& mazeObj, CellPos pos)
{

	if (!validCell(mazeObj, pos))
	{
		return true;
	}

	return mazeObj.vertical_walls[pos.y][pos.x];
}

bool rightWallExist(const Maze& mazeObj, CellPos pos)
{

	if (!validCell(mazeObj, pos))
	{
		return true;
	}

	return mazeObj.vertical_walls[pos.y][pos.x + 1];
}

bool topWallExist(const Maze& mazeObj, CellPos pos)
{

	if (!validCell(mazeObj, pos))
	{
		return true;
	}

	return mazeObj.horizontal_walls[pos.y][pos.x];
}

bool bottomWallExist(const Maze& mazeObj, CellPos pos)
{
	if (!validCell(mazeObj, pos))
	{
		return true;
	}

	return mazeObj.horizontal_walls[pos.y + 1][pos.x];
}


std::vector<CellPos> getNeighbours(const Maze& mazeObj, CellPos cell)
{
	CellPos leftCell;
	leftCell.x = cell.x - 1;
	leftCell.y = cell.y;

	CellPos rightCell;
	rightCell.x = cell.x + 1;
	rightCell.y = cell.y;

	CellPos topCell;
	topCell.x = cell.x;
	topCell.y = cell.y - 1;

	CellPos bottomCell;
	bottomCell.x = cell.x;
	bottomCell.y = cell.y + 1;

	std::vector<CellPos> neighbourCells;
	neighbourCells.reserve(4);
	if (!leftWallExist(mazeObj, cell))
	{
		neighbourCells.push_back(leftCell);
	}

	if (!rightWallExist(mazeObj, cell))
	{
		neighbourCells.push_back(rightCell);
	}

	if (!topWallExist(mazeObj, cell))
	{
		neighbourCells.push_back(topCell);
	}

	if (!bottomWallExist(mazeObj, cell))
	{
		neighbourCells.push_back(bottomCell);
	}

	return neighbourCells;
}

MazeStatus findShortPathInstantly(const Maze& mazeObj, Path& path, CellPos startCell, CellPos endCell)
{
    MazeStatus status;
    if (!validCell(mazeObj, startCell) || !validCell(mazeObj, endCell))
    {
        status.statusType = ERROR;
        status.statusMessage = "Invalid start and/or end cell";
        return status;
    }

	// init
	int m = mazeObj.maze_width;
	int n = mazeObj.maze_height;

	std::vector<std::vector<bool>> cellExplored;
	cellExplored.reserve(n);
	for (int i = 0; i < n; i++)
	{
		std::vector<bool> row;
		row.reserve(m);
		for (int j = 0; j < m; j++)
		{
			row.push_back(false);
		}
		cellExplored.push_back(row);
	}

	if (!path.parents.empty())
		path.parents.clear();
	path.parents.reserve(n);
	for (int i = 0; i < n; i++)
	{
		std::vector<CellPos> row;
		row.reserve(m);
		for (int j = 0; j < m; j++)
			row.push_back({ -1, -1 });
		path.parents.push_back(row);
	}

	// Breadth-first search
	auto startTime = std::chrono::high_resolution_clock::now();
	std::queue<CellPos> q;
	cellExplored[startCell.y][startCell.x] = true;
	q.push(startCell);
	while (!q.empty())
	{
		CellPos v = q.front();
		q.pop();
		if (v == endCell)
		{
			auto endTime = std::chrono::high_resolution_clock::now();
			std::chrono::duration<double, std::milli> elapsed = endTime - startTime;
			status.statusType = SUCCESSFUL;
			status.statusMessage = "Shortest Path Found in " + std::to_string(elapsed.count()) + " ms";
			return status;
		}
		for (CellPos neighbourCell : getNeighbours(mazeObj, v))
		{
			if (!cellExplored[neighbourCell.y][neighbourCell.x])
			{
				CellPos w = { neighbourCell.x, neighbourCell.y };
				cellExplored[w.y][w.x] = true;
				path.parents[w.y][w.x] = v;
				q.push(w);
			}
		}
	}

	status.statusType = FAILED;
	status.statusMessage = "Path Not Found";
	return status;
}
