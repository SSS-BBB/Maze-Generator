#pragma once

#include <SFML/Graphics.hpp>

void init(sf::Font windowFont);
void update();
void eventHandling(sf::RenderWindow& window, const sf::Vector2f& mousePosition);
void draw(sf::RenderWindow& window, const sf::Vector2f& mousePosition);