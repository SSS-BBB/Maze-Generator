#include "render.hpp"
#include <string>
#include <vector>

struct TextInput
{
	std::string inputString = "Input Here";
	bool isListening = false;
	sf::Vector2f position = { 0.0f, 0.0f };
	sf::Vector2f size = { 150.0f, 40.0f };
};

struct Button
{
	std::string buttonString = "Button";
	bool isClicked = false;
	sf::Vector2f position = { 0.0f, 0.0f };
	sf::Vector2f size = { 120.0f, 60.0f };
	sf::Color buttonColor = sf::Color::Blue;
	sf::Color textColor = sf::Color::White;
};

void drawTextInputText(sf::RenderWindow& window, TextInput& textInput)
{
	sf::Font font("fonts/Roboto-Regular.ttf");
	sf::Text text(font);
	text.setString(textInput.inputString);
	text.setFillColor(sf::Color::Black);
	text.setPosition({ textInput.position.x + 5.0f, textInput.position.y + 5.0f });
	text.setCharacterSize(20.0f);
	window.draw(text);
}

void drawTextInputBorder(sf::RenderWindow& window, TextInput& textInput)
{
	sf::RectangleShape border(textInput.size);
	border.setFillColor(sf::Color::Transparent);
	border.setOutlineColor(sf::Color::Black);
	border.setOutlineThickness(2.0f);
	border.setPosition(textInput.position);
	window.draw(border);
}

// draw rect first then text
void drawButtonText(sf::RenderWindow& window, Button& button)
{
	sf::Font font("fonts/Roboto-Regular.ttf");
	sf::Text text(font);
	text.setString(button.buttonString);
	text.setFillColor(button.textColor);
	text.setPosition({ button.position.x + 15.0f, button.position.y + 15.0f });
	text.setCharacterSize(20.0f);
	window.draw(text);
}

void drawButtonRect(sf::RenderWindow& window, Button& button)
{
	// 

	sf::RectangleShape rect(button.size);
	rect.setFillColor(button.buttonColor);
	rect.setOutlineColor(sf::Color::Black);
	rect.setOutlineThickness(2.0f);
	rect.setPosition(button.position);
	window.draw(rect);
}

void drawMazeGeneratorWindow(sf::RenderWindow& window)
{
	std::vector<TextInput> textInputList;
	std::vector<Button> buttonList;

	while (window.isOpen())
	{
		while (const std::optional event = window.pollEvent())
		{
			if (event->is<sf::Event::Closed>())
				window.close();
		}

		window.clear(sf::Color::White);

		// render window

		sf::Font font("fonts/Roboto-Regular.ttf");
		sf::Vector2f mousePosition = window.mapPixelToCoords(sf::Mouse::getPosition(window));

		// Mouse Position Text
		sf::Text mousePosText(font);
		mousePosText.setString("Mouse Position: (" + 
								std::to_string(mousePosition.x) + "," +
								std::to_string(mousePosition.y) + ")");
		mousePosText.setCharacterSize(16);
		mousePosText.setFillColor(sf::Color::Black);
		mousePosText.setPosition({ 16.0, 680.0 });
		window.draw(mousePosText);

		// Maze Generator Title
		sf::Text titleText(font);
		titleText.setString("Maze Generator");
		titleText.setCharacterSize(30);
		titleText.setFillColor(sf::Color::Black);
		titleText.setPosition({ 256.0, 20.0 });
		window.draw(titleText);

		// Maze Frame
		sf::RectangleShape mazeFrame({ 650.0, 500.0 });
		mazeFrame.setFillColor(sf::Color::Black);
		mazeFrame.setPosition( { 40.0, 80.0 });
		window.draw(mazeFrame);

		// Width Text
		sf::Text widthText(font);
		widthText.setString("Width");
		widthText.setCharacterSize(22);
		widthText.setFillColor(sf::Color::Black);
		widthText.setPosition({ 792.0f, 180.0f });
		window.draw(widthText);

		// Width Text Input
		TextInput widthTextInput;
		widthTextInput.position = { 750.0f, 215.0f };
		drawTextInputText(window, widthTextInput);
		drawTextInputBorder(window, widthTextInput);

		// Height Text
		sf::Text heightText(font);
		heightText.setString("Height");
		heightText.setCharacterSize(22);
		heightText.setFillColor(sf::Color::Black);
		heightText.setPosition({ 792.0f + 300.0f, 180.0f });
		window.draw(heightText);

		// Height Text Input
		TextInput heightTextInput;
		heightTextInput.position = { 750.0f + 300.0f, 215.0f };
		drawTextInputText(window, heightTextInput);
		drawTextInputBorder(window, heightTextInput);

		// Speed Text
		sf::Text speedText(font);
		speedText.setString("Speed");
		speedText.setCharacterSize(22);
		speedText.setFillColor(sf::Color::Black);
		speedText.setPosition({ 942.0f, 180.0f + 100.0f });
		window.draw(speedText);

		// Speed Text Input
		TextInput speedTextInput;
		speedTextInput.position = { 900.0f, 215.0f + 100.0f };
		drawTextInputText(window, speedTextInput);
		drawTextInputBorder(window, speedTextInput);

		// Generate Button
		Button generateButton;
		generateButton.buttonString = "Generate";
		generateButton.position = { 920.0f, 315.0f + 100.0f };
		drawButtonRect(window, generateButton);
		drawButtonText(window, generateButton);

		// Save Button
		Button saveButton;
		saveButton.buttonString = "Save";
		saveButton.position = { 155.0f, 590.0f };
		drawButtonRect(window, saveButton);
		drawButtonText(window, saveButton);

		// Load Button
		Button loadButton;
		loadButton.buttonString = "Load";
		loadButton.position = { 155.0f + 200.0f, 590.0f };
		drawButtonRect(window, loadButton);
		drawButtonText(window, loadButton);

		// Generate Button
		Button pathFindingButton;
		pathFindingButton.buttonString = "Path Finding";
		pathFindingButton.position = { 920.0f, 415.0f + 100.0f };
		drawButtonRect(window, pathFindingButton);
		drawButtonText(window, pathFindingButton);

		window.display();
	}
}
