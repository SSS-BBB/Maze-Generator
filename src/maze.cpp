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
			row.push_back(false);
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
			row.push_back(false);
		}
		mazeObj.vertical_walls.push_back(row);
	}

	std::cout << "Maze Initialized successfully" << std::endl;

}

void generateMaze(Maze& mazeObj)
{
	int m = mazeObj.maze_width;
	int n = mazeObj.maze_height;

	if (m <= 0 || n <= 0)
	{
		std::cerr << "Maze Width or Height is less than or equals to 0, unable to initializae maze." << std::endl;
		return;
	}

	// generate maze randomly

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


	std::cout << "Maze Generated successfully" << std::endl;
}
