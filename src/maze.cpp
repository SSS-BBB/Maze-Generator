#include "maze.hpp"
#include <iostream>

void initMaze(Maze& mazeObj)
{
	// clear walls
	if (!mazeObj.horizontal_walls.empty())
	{
		mazeObj.horizontal_walls.clear();
	}

	if (!mazeObj.vertical_walls.empty())
	{
		mazeObj.vertical_walls.clear();
	}

	// make every values false
	int m = mazeObj.maze_width;
	int n = mazeObj.maze_height;

	if (m <= 0 || n <= 0)
	{
		std::cerr << "Maze Width or Height is less than or equals to 0, unable to initializae maze." << std::endl;
		return;
	}

	// horizontal walls size = (n + 1) x m
	mazeObj.horizontal_walls.reserve(n + 1);
	for (int i = 0; i < n + 1; i++)
	{
		std::vector<bool> row;
		row.reserve(m);
		for (int j = 0; j < m; j++)
		{
			row.push_back(true);
		}
		mazeObj.horizontal_walls.push_back(row);
	}

	// vertical walls size = n x (m + 1)
	mazeObj.vertical_walls.reserve(n);
	for (int i = 0; i < n; i++)
	{
		std::vector<bool> row;
		row.reserve(m + 1);
		for (int j = 0; j < m + 1; j++)
		{
			row.push_back(true);
		}
		mazeObj.vertical_walls.push_back(row);
	}

	std::cout << "Maze Initialized successfully" << std::endl;

}

MazeStatus setMaze(Maze& mazeObj)
{
	// set maze to test
	MazeStatus mazeStatus;

	int m = mazeObj.maze_width;
	int n = mazeObj.maze_height;

	if (m <= 0 || n <= 0)
	{
		mazeStatus.statusType = ERROR;
		mazeStatus.statusMessage = "Invalid Cell Number";
		std::cerr << "Maze Width or Height is less than or equals to 0, unable to set maze." << std::endl;
		return mazeStatus;
	}
	// vertical walls test
	// 5 x 6
	// (0, 0) -> (4, 4)
	/*
	for (int i = 0; i < 5; i++)
	{
		mazeObj.vertical_walls[i][i] = true;
	}
	*/

	/*
	mazeObj.vertical_walls[0][0] = true;
	mazeObj.vertical_walls[1][1] = true;
	mazeObj.vertical_walls[1][2] = true;
	mazeObj.vertical_walls[4][1] = true;
	mazeObj.vertical_walls[4][2] = true;
	mazeObj.vertical_walls[4][3] = true;
	mazeObj.vertical_walls[4][4] = true;
	mazeObj.vertical_walls[4][5] = true;
	mazeObj.vertical_walls[4][6] = true;
	*/

	// horizontal walls test
	/*
	mazeObj.horizontal_walls[0][0] = true;
	mazeObj.horizontal_walls[1][1] = true;
	mazeObj.horizontal_walls[2][1] = true;
	mazeObj.horizontal_walls[2][2] = true;
	mazeObj.horizontal_walls[2][3] = true;
	*/

	// set every walls to true
	for (int i = 0; i < mazeObj.horizontal_walls.size(); i++)
	{
		for (int j = 0; j < mazeObj.horizontal_walls[i].size(); j++)
		{
			mazeObj.horizontal_walls[i][j] = true;
		}
	}

	for (int i = 0; i < mazeObj.vertical_walls.size(); i++)
	{
		for (int j = 0; j < mazeObj.vertical_walls[i].size(); j++)
		{
			mazeObj.vertical_walls[i][j] = true;
		}
	}

	mazeStatus.statusType = SUCCESSFUL;
	mazeStatus.statusMessage = "Maze Set Successfully";
	std::cout << "Maze Generated successfully" << std::endl;
	return mazeStatus;
}

bool validCell(const Maze& mazeObj, CellPos pos)
{
	int m = mazeObj.maze_width;
	int n = mazeObj.maze_height;

	return !(pos.x < 0 || pos.x >= m || pos.y < 0 || pos.y >= n);
}

bool getLeftWall(const Maze& mazeObj, CellPos pos)
{

	if (!validCell(mazeObj, pos))
	{
		std::cerr << "Invalid cell position, unable to getLeftWall." << std::endl;
		return false;
	}

	return mazeObj.vertical_walls[pos.y][pos.x];
}

bool getRightWall(const Maze& mazeObj, CellPos pos)
{

	if (!validCell(mazeObj, pos))
	{
		std::cerr << "Invalid cell position, unable to getRightWall." << std::endl;
		return false;
	}

	return mazeObj.vertical_walls[pos.y][pos.x + 1];
}

bool getTopWall(const Maze& mazeObj, CellPos pos)
{

	if (!validCell(mazeObj, pos))
	{
		std::cerr << "Invalid cell position, unable to getTopWall." << std::endl;
		return false;
	}

	return mazeObj.horizontal_walls[pos.y][pos.x];
}

bool getBottomWall(const Maze& mazeObj, CellPos pos)
{
	if (!validCell(mazeObj, pos))
	{
		std::cerr << "Invalid cell position, unable to getBottomWall." << std::endl;
		return false;
	}

	return mazeObj.horizontal_walls[pos.y + 1][pos.x];
}

void setLeftWall(Maze& mazeObj, CellPos pos, bool wallExists)
{
	if (!validCell(mazeObj, pos))
	{
		std::cerr << "Invalid cell position, unable to setLeftWall." << std::endl;
		return;
	}

	mazeObj.vertical_walls[pos.y][pos.x] = wallExists;
}

void setRightWall(Maze& mazeObj, CellPos pos, bool wallExists)
{
	if (!validCell(mazeObj, pos))
	{
		std::cerr << "Invalid cell position, unable to setRightWall." << std::endl;
		return;
	}

	mazeObj.vertical_walls[pos.y][pos.x + 1] = wallExists;
}

void setTopWall(Maze& mazeObj, CellPos pos, bool wallExists)
{
	if (!validCell(mazeObj, pos))
	{
		std::cerr << "Invalid cell position, unable to getTopWall." << std::endl;
		return;
	}

	mazeObj.horizontal_walls[pos.y][pos.x] = wallExists;
}

void setBottomWall(Maze& mazeObj, CellPos pos, bool wallExists)
{
	if (!validCell(mazeObj, pos))
	{
		std::cerr << "Invalid cell position, unable to getBottomWall." << std::endl;
		return;
	}

	mazeObj.horizontal_walls[pos.y + 1][pos.x] = wallExists;
}

bool isCellUnVisited(const Maze& mazeObj, const std::vector<std::vector<bool>>& cellsVisited, CellPos cell)
{
	if (!validCell(mazeObj, cell))
		return false;

	return !cellsVisited[cell.y][cell.x];
}

CellPos getUnvisitedNeighbour(const Maze& mazeObj, const std::vector<std::vector<bool>>& cellsVisited, CellPos cell)
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

	std::vector<CellPos> unVisitedCells;
	unVisitedCells.reserve(4);
	if (isCellUnVisited(mazeObj, cellsVisited, leftCell))
	{
		unVisitedCells.push_back(leftCell);
	}

	if (isCellUnVisited(mazeObj, cellsVisited, rightCell))
	{
		unVisitedCells.push_back(rightCell);
	}

	if (isCellUnVisited(mazeObj, cellsVisited, topCell))
	{
		unVisitedCells.push_back(topCell);
	}

	if (isCellUnVisited(mazeObj, cellsVisited, bottomCell))
	{
		unVisitedCells.push_back(bottomCell);
	}

	// no unvisited neighbor
	if (unVisitedCells.empty())
	{
		CellPos nullCell;
		nullCell.x = -1;
		nullCell.y = -1;
		return nullCell;
	}

	int selectedCellIndex = rand() % unVisitedCells.size();
	return unVisitedCells[selectedCellIndex];
}

MazeStatus generateMazeInstantly(Maze& mazeObj)
{
	// generate maze randomly
	MazeStatus mazeStatus;

	int m = mazeObj.maze_width;
	int n = mazeObj.maze_height;

	if (m <= 0 || n <= 0)
	{
		mazeStatus.statusType = ERROR;
		mazeStatus.statusMessage = "Invalid Cell Number";
		std::cerr << "Maze Width or Height is less than or equals to 0, unable to generate maze." << std::endl;
		return mazeStatus;
	}

	std::vector<std::vector<bool>> cellVisited;
	cellVisited.reserve(n);
	for (int i = 0; i < n; i++)
	{
		std::vector<bool> row;
		row.reserve(m);
		for (int j = 0; j < m; j++)
		{
			row.push_back(false);
		}
		cellVisited.push_back(row);
	}

	// Randomized depth-first search
	auto startTime = std::chrono::high_resolution_clock::now();

	std::stack<CellPos> cellStack;
	CellPos initCell;
	initCell.x = rand() % m;
	initCell.y = rand() % n;
	cellVisited[initCell.y][initCell.x] = true;
	cellStack.push(initCell);
	while (!cellStack.empty())
	{
		CellPos currentCell = cellStack.top();
		cellStack.pop(); // pop function doesn't return a value for some reason

		CellPos chosenCell = getUnvisitedNeighbour(mazeObj, cellVisited, currentCell);
		if (chosenCell.x < 0 || chosenCell.y < 0) // dead end
			continue;

		cellStack.push(currentCell);

		if (chosenCell.x == currentCell.x - 1)
		{
			// left cell -> remove left wall
			setLeftWall(mazeObj, currentCell, false);
		}

		else if (chosenCell.x == currentCell.x + 1)
		{
			// right cell -> remove right wall
			setRightWall(mazeObj, currentCell, false);
		}

		else if (chosenCell.y == currentCell.y - 1)
		{
			// top cell -> remove top wall
			setTopWall(mazeObj, currentCell, false);
		}

		else if (chosenCell.y == currentCell.y + 1)
		{
			// bottom cell -> remove bottom wall
			setBottomWall(mazeObj, currentCell, false);
		}

		else
		{
			mazeStatus.statusType = ERROR;
			mazeStatus.statusMessage = "The chosen cell is not the neighbor of the current cell";
			return mazeStatus;
		}

		cellVisited[chosenCell.y][chosenCell.x] = true;
		cellStack.push(chosenCell);
	}

	auto endTime = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double, std::milli> elapsed = endTime - startTime;
	std::cout << "Maze Generated in " << elapsed.count() << " ms" << std::endl;

	mazeStatus.statusType = SUCCESSFUL;
	mazeStatus.statusMessage = "Maze Generated Successfully in " + std::to_string(elapsed.count()) + " ms";

	return mazeStatus;
}

void startGeneration(MazeGenerator& mazeGenerator, Maze& mazeObj)
{
	int m = mazeObj.maze_width;
	int n = mazeObj.maze_height;

	if (m <= 0 || n <= 0)
	{
		mazeGenerator.mazeStatus.statusType = ERROR;
		mazeGenerator.mazeStatus.statusMessage = "Invalid Cell Number";
		std::cerr << "Maze Width or Height is less than or equals to 0, unable to start generation." << std::endl;
		return;
	}

	mazeGenerator.mazeStatus.statusType = PROCESSING;
	mazeGenerator.mazeStatus.statusMessage = "Generating...";

	if (!mazeGenerator.cellVisited.empty())
		mazeGenerator.cellVisited.clear();

	mazeGenerator.cellVisited.reserve(n);
	for (int i = 0; i < n; i++)
	{
		std::vector<bool> row;
		row.reserve(m);
		for (int j = 0; j < m; j++)
		{
			row.push_back(false);
		}
		mazeGenerator.cellVisited.push_back(row);
	}

	while (!mazeGenerator.cellStack.empty())
		mazeGenerator.cellStack.pop();

	if (!mazeGenerator.visitedPos.empty())
		mazeGenerator.visitedPos.clear();

	if (!mazeGenerator.deadEndPos.empty())
		mazeGenerator.deadEndPos.clear();

	mazeGenerator.visitedPos.reserve(n * m);
	mazeGenerator.deadEndPos.reserve(n * m);

	mazeGenerator.startTime = std::chrono::high_resolution_clock::now();

	CellPos initCell;
	initCell.x = rand() % m;
	initCell.y = rand() % n;
	mazeGenerator.cellVisited[initCell.y][initCell.x] = true;
	mazeGenerator.visitedPos.push_back(initCell);
	mazeGenerator.cellStack.push(initCell);
}

void updateGeneration(MazeGenerator& mazeGenerator, Maze& mazeObj)
{
	if (mazeGenerator.mazeStatus.statusType != PROCESSING)
		return;

	if (mazeGenerator.cellStack.empty())
	{
		// finished generating
		std::chrono::time_point<std::chrono::high_resolution_clock> endTime = std::chrono::high_resolution_clock::now();
		std::chrono::duration<double, std::milli> elapsed = endTime - mazeGenerator.startTime;

		mazeGenerator.mazeStatus.statusType = SUCCESSFUL;
		mazeGenerator.mazeStatus.statusMessage = "Maze Generated Successfully in " + std::to_string(elapsed.count()) + " ms";
		return;
	}

	CellPos currentCell = mazeGenerator.cellStack.top();
	mazeGenerator.cellStack.pop(); // pop function doesn't return a value for some reason

	CellPos chosenCell = getUnvisitedNeighbour(mazeObj, mazeGenerator.cellVisited, currentCell);
	if (chosenCell.x < 0 || chosenCell.y < 0)
	{
		// dead end
		mazeGenerator.deadEndPos.push_back(currentCell);
		return;
	}

	mazeGenerator.cellStack.push(currentCell);

	if (chosenCell.x == currentCell.x - 1)
	{
		// left cell -> remove left wall
		setLeftWall(mazeObj, currentCell, false);
	}

	else if (chosenCell.x == currentCell.x + 1)
	{
		// right cell -> remove right wall
		setRightWall(mazeObj, currentCell, false);
	}

	else if (chosenCell.y == currentCell.y - 1)
	{
		// top cell -> remove top wall
		setTopWall(mazeObj, currentCell, false);
	}

	else if (chosenCell.y == currentCell.y + 1)
	{
		// bottom cell -> remove bottom wall
		setBottomWall(mazeObj, currentCell, false);
	}

	else
	{
		mazeGenerator.mazeStatus.statusType = ERROR;
		mazeGenerator.mazeStatus.statusMessage = "The chosen cell is not the neighbor of the current cell";
		return;
	}

	mazeGenerator.cellVisited[chosenCell.y][chosenCell.x] = true;
	mazeGenerator.visitedPos.push_back(chosenCell);
	mazeGenerator.cellStack.push(chosenCell);
}
