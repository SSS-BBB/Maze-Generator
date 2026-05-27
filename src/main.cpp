#include <SFML/Graphics.hpp>
#include "render.hpp"

int main()
{
	srand(time(NULL));

	const int WINDOW_WIDTH = 1280;
	const int WINDOW_HEIGHT = 720;

	sf::RenderWindow window(sf::VideoMode({ WINDOW_WIDTH, WINDOW_HEIGHT }), "Maze Generator");

	drawMazeGeneratorWindow(window);

	return 0;
}
