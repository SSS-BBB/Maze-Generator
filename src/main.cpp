#include "render.hpp"
#include <iostream>

int main()
{
	srand(time(NULL));

	const int WINDOW_WIDTH = 1280;
	const int WINDOW_HEIGHT = 720;

	sf::RenderWindow window(sf::VideoMode({ WINDOW_WIDTH, WINDOW_HEIGHT }), "Maze Generator");
	window.setVerticalSyncEnabled(false);

	const std::string FONT_PATH = "fonts/Roboto-Regular.ttf";
	const std::string FONT_LOAD_ERROR = "Unable to open font from " + FONT_PATH
		+ "Please make sure there is a fonts folder containing Roboto-Regular.ttf in the same directory as CMakeList.txt, and configure CMake";

	sf::Font font;
	if (!font.openFromFile(FONT_PATH))
	{
		std::cerr << FONT_LOAD_ERROR << std::endl;
		return 1;
	}

	init(font);

	while (window.isOpen())
	{
		update();
		
		draw(window);

		window.display();
	}

	return 0;
}
