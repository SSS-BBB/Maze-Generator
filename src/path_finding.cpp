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

	std::vector<std::vector<int>> cellDistance;
	cellDistance.reserve(n);
	for (int i = 0; i < n; i++)
	{
		std::vector<int> row;
		row.reserve(m);
		for (int j = 0; j < m; j++)
		{
			row.push_back(-1);
		}
		cellDistance.push_back(row);
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
	cellDistance[startCell.y][startCell.x] = 0;
	q.push(startCell);
	while (!q.empty())
	{
		CellPos v = q.front();
		q.pop();
		if (v == endCell)
		{
			int distance = cellDistance[v.y][v.x];
			auto endTime = std::chrono::high_resolution_clock::now();
			std::chrono::duration<double, std::milli> elapsed = endTime - startTime;
			status.statusType = SUCCESSFUL;
			status.statusMessage = "Shortest Path of Distance " + std::to_string(distance) + 
								   " Found in " + std::to_string(elapsed.count()) + " ms";
			return status;
		}

		for (CellPos neighbourCell : getNeighbours(mazeObj, v))
		{
			if (!cellExplored[neighbourCell.y][neighbourCell.x])
			{
				CellPos w = { neighbourCell.x, neighbourCell.y };
				cellExplored[w.y][w.x] = true;
				cellDistance[w.y][w.x] = cellDistance[v.y][v.x] + 1;
				path.parents[w.y][w.x] = v;
				q.push(w);
			}
		}
	}

	status.statusType = FAILED;
	status.statusMessage = "Path Not Found";
	return status;
}

void startFindShortPath(const Maze& mazeObj, PathFinder& pathFinder, Path& path)
{
	if (!validCell(mazeObj, pathFinder.startCell) || !validCell(mazeObj, pathFinder.endCell))
	{
		pathFinder.status.statusType = ERROR;
		pathFinder.status.statusMessage = "Invalid start and/or end cell";
		return;
	}

	int m = mazeObj.maze_width;
	int n = mazeObj.maze_height;

	if (!pathFinder.cellExplored.empty())
		pathFinder.cellExplored.clear();

	pathFinder.cellExplored.reserve(n);
	for (int i = 0; i < n; i++)
	{
		std::vector<bool> row;
		row.reserve(m);
		for (int j = 0; j < m; j++)
		{
			row.push_back(false);
		}
		pathFinder.cellExplored.push_back(row);
	}

	if (!pathFinder.cellDistance.empty())
		pathFinder.cellDistance.clear();

	pathFinder.cellDistance.reserve(n);
	for (int i = 0; i < n; i++)
	{
		std::vector<int> row;
		row.reserve(m);
		for (int j = 0; j < m; j++)
		{
			row.push_back(-1);
		}
		pathFinder.cellDistance.push_back(row);
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

	while (!pathFinder.q.empty())
		pathFinder.q.pop();

	if (!pathFinder.exploredPos.empty())
		pathFinder.exploredPos.clear();

	pathFinder.exploredPos.reserve(m * n);

	pathFinder.cellExplored[pathFinder.startCell.y][pathFinder.startCell.x] = true;
	pathFinder.cellDistance[pathFinder.startCell.y][pathFinder.startCell.x] = 0;
	pathFinder.exploredPos.push_back(pathFinder.startCell);
	pathFinder.q.push(pathFinder.startCell);

	pathFinder.status.statusType = PROCESSING;
	pathFinder.status.statusMessage = "Finding Path...";
	pathFinder.startTime = std::chrono::high_resolution_clock::now();
	pathFinder.lastUpdateTime = std::chrono::high_resolution_clock::now();
}

void updateFindShortPath(const Maze& mazeObj, PathFinder& pathFinder, Path& path)
{
	if (pathFinder.status.statusType != PROCESSING)
		return;

	// delay
	auto currentTime = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double, std::milli> elapsed = currentTime - pathFinder.lastUpdateTime;
	if (elapsed.count() < pathFinder.delay)
		return;
	pathFinder.lastUpdateTime = std::chrono::high_resolution_clock::now();

	// path finding
	if (pathFinder.q.empty())
	{
		pathFinder.status.statusType = FAILED;
		pathFinder.status.statusMessage = "Path Not Found";
		return;
	}

	CellPos v = pathFinder.q.front();
	pathFinder.q.pop();
	if (v == pathFinder.endCell)
	{
		int distance = pathFinder.cellDistance[v.y][v.x];
		auto endTime = std::chrono::high_resolution_clock::now();
		std::chrono::duration<double, std::milli> elapsed = endTime - pathFinder.startTime;
		pathFinder.status.statusType = SUCCESSFUL;
		pathFinder.status.statusMessage = "Shortest Path of Distance " + std::to_string(distance) +
										  " Found in " + std::to_string(elapsed.count()) + " ms";
		return;
	}
	for (CellPos neighbourCell : getNeighbours(mazeObj, v))
	{
		if (!pathFinder.cellExplored[neighbourCell.y][neighbourCell.x])
		{
			CellPos w = { neighbourCell.x, neighbourCell.y };
			pathFinder.cellExplored[w.y][w.x] = true;
			pathFinder.cellDistance[w.y][w.x] = pathFinder.cellDistance[v.y][v.x] + 1;
			pathFinder.exploredPos.push_back(w);
			path.parents[w.y][w.x] = v;
			pathFinder.q.push(w);
		}
	}
}