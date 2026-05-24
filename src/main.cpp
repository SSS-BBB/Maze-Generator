#include <SFML/Graphics.hpp>
#include "render.hpp"

int main()
{
	sf::RenderWindow window(sf::VideoMode({ 200, 200 }), "SFML works!");

	draw(window);

	return 0;
}
