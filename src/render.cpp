#include "render.hpp"
#include <string>
#include <vector>
#include <iostream>
#include <functional>

struct TextInput
{
	std::string textInputName = "";
	std::string inputString = "";
	std::string filterText = "0123456789"; // only filter text allowed to input
	bool isListening = false;
	bool showCursor = false; // show | after the last character to let the user know what text input is being focused
	sf::Clock cursorClock;
	sf::Vector2f position = { 0.0f, 0.0f };
	sf::Vector2f size = { 150.0f, 40.0f };
};

struct Button
{
	std::string buttonName = "Button";
	bool isClicked = false;
	sf::Vector2f position = { 0.0f, 0.0f };
	sf::Vector2f size = { 120.0f, 60.0f };
	sf::Color buttonColor = sf::Color(190, 190, 190);
	sf::Color textColor = sf::Color::Black;
};

void drawTextInput(sf::RenderWindow& window, TextInput& textInput)
{
	if (textInput.isListening)
	{
		// show blinking cursor
		if (textInput.cursorClock.getElapsedTime().asSeconds() >= 0.5f)
		{
			textInput.showCursor = !textInput.showCursor;
			textInput.cursorClock.restart();
		}
	}

	// draw text
	sf::Font font("fonts/Roboto-Regular.ttf");
	sf::Text text(font);
	text.setString(textInput.inputString);
	text.setFillColor(sf::Color::Black);
	text.setPosition({ textInput.position.x + 5.0f, textInput.position.y + 5.0f });
	text.setCharacterSize(20.0f);
	window.draw(text);

	if (textInput.showCursor)
	{
		// draw cursor
		sf::RectangleShape cursorRect({ 2.0f, (float) text.getCharacterSize() });
		cursorRect.setPosition({ text.getPosition().x + text.getGlobalBounds().size.x + 3.0f,
							     text.getPosition().y + 3.0f });
		cursorRect.setFillColor(sf::Color::Black);
		window.draw(cursorRect);
	}

	// draw border
	sf::RectangleShape border(textInput.size);
	border.setFillColor(sf::Color::Transparent);
	border.setOutlineColor(sf::Color::Black);
	border.setOutlineThickness(2.0f);
	border.setPosition(textInput.position);
	window.draw(border);
}

void drawButton(sf::RenderWindow& window, const Button& button)
{
	// button rect
	sf::RectangleShape rect(button.size);
	rect.setFillColor(button.buttonColor);
	rect.setOutlineColor(sf::Color::Black);
	rect.setOutlineThickness(2.0f);
	rect.setPosition(button.position);
	window.draw(rect);

	// button text
	sf::Font font("fonts/Roboto-Regular.ttf");
	sf::Text text(font);
	text.setString(button.buttonName);
	text.setFillColor(button.textColor);
	text.setPosition({ button.position.x + 15.0f, button.position.y + 15.0f });
	text.setCharacterSize(20.0f);
	window.draw(text);
}

void checkTextInputClicked(std::vector<TextInput>& textInputList, const sf::Vector2f& mousePosition)
{
	float x = mousePosition.x;
	float y = mousePosition.y;

	for (TextInput& textInput : textInputList)
	{
		float x0 = textInput.position.x;
		float y0 = textInput.position.y;
		float x1 = textInput.position.x + textInput.size.x;
		float y1 = textInput.position.y + textInput.size.y;

		// x0 <= x <= x1 && y0 <= y <= y1
		if (x >= x0 && x <= x1 && y >= y0 && y <= y1)
		{
			textInput.isListening = true;
			textInput.showCursor = true;
			textInput.cursorClock.restart();
		
		}
		else
		{
			textInput.isListening = false;

			// stop blinking cursor
			textInput.showCursor = false;
			textInput.cursorClock.reset();
		}
	}
}

void onGenerateButtonClicked(const std::vector<TextInput>& textInputList)
{
	sf::Vector2i mazeSize = { 0, 0 };
	int generationSpeed = 0;

	for (TextInput textInput : textInputList)
	{
		if (textInput.textInputName == "Width")
		{
			mazeSize.x = std::stoi(textInput.inputString);
		}

		else if (textInput.textInputName == "Height")
		{
			mazeSize.y = std::stoi(textInput.inputString);
		}

		else if (textInput.textInputName == "Speed")
		{
			generationSpeed = std::stoi(textInput.inputString);
		}
	}

	std::cout << "Maze Width: " << mazeSize.x << std::endl;
	std::cout << "Maze Height: " << mazeSize.y << std::endl;
	std::cout << "Generation Speed: " << generationSpeed << std::endl;

}

void checkButtonClicked(std::vector<Button>& buttonList, const std::vector<TextInput>& textInputList, const sf::Vector2f& mousePosition)
{
	float x = mousePosition.x;
	float y = mousePosition.y;

	for (Button& button : buttonList)
	{
		float x0 = button.position.x;
		float y0 = button.position.y;
		float x1 = button.position.x + button.size.x;
		float y1 = button.position.y + button.size.y;

		// x0 <= x <= x1 && y0 <= y <= y1
		if (x >= x0 && x <= x1 && y >= y0 && y <= y1)
		{
			button.isClicked = true;
			std::cout << button.buttonName + " button is clicked!" << std::endl;

			if (button.buttonName == "Generate")
			{
				onGenerateButtonClicked(textInputList);
			}

			break;

		}
		else
		{
			button.isClicked = false;
		}
	}
}

bool isCharInString(char target, std::string str)
{
	for (char c : str)
	{
		if (c == target)
			return true;
	}

	return false;
}

void onTextEntered(char enteredChar, std::vector<TextInput>& textInputList)
{
	for (TextInput& textInput : textInputList)
	{
		if (textInput.isListening)
		{
			if (isCharInString(enteredChar, textInput.filterText))
			{
				textInput.inputString += enteredChar;
			}
			if (enteredChar == '\b')
			{
				// back space is preesed, remove last character
				if (!textInput.inputString.empty())
					textInput.inputString.pop_back();
			}
		}
	}
}

void drawMazeGeneratorWindow(sf::RenderWindow& window)
{
	std::vector<TextInput> textInputList;
	std::vector<Button> buttonList;

	// text input and button struct
	TextInput widthTextInput;
	widthTextInput.textInputName = "Width";
	widthTextInput.position = { 750.0f, 215.0f };
	textInputList.emplace_back(widthTextInput);

	TextInput heightTextInput;
	heightTextInput.textInputName = "Height";
	heightTextInput.position = { 750.0f + 300.0f, 215.0f };
	textInputList.emplace_back(heightTextInput);

	TextInput speedTextInput;
	speedTextInput.textInputName = "Speed";
	speedTextInput.position = { 900.0f, 215.0f + 100.0f };
	textInputList.emplace_back(speedTextInput);

	Button generateButton;
	generateButton.buttonName = "Generate";
	generateButton.position = { 920.0f, 315.0f + 100.0f };
	buttonList.emplace_back(generateButton);

	Button saveButton;
	saveButton.buttonName = "Save";
	saveButton.position = { 155.0f, 590.0f };
	buttonList.emplace_back(saveButton);

	Button loadButton;
	loadButton.buttonName = "Load";
	loadButton.position = { 155.0f + 200.0f, 590.0f };
	buttonList.emplace_back(loadButton);

	Button pathFindingButton;
	pathFindingButton.buttonName = "Path Finding";
	pathFindingButton.position = { 920.0f - 10.0f, 415.0f + 100.0f };
	pathFindingButton.size = { 140.0f, 60.0f };
	buttonList.emplace_back(pathFindingButton);

	while (window.isOpen())
	{
		sf::Vector2f mousePosition = window.mapPixelToCoords(sf::Mouse::getPosition(window));

		while (const std::optional event = window.pollEvent())
		{
			if (event->is<sf::Event::Closed>())
				window.close();

			// mouse clicked
			if (const auto* mousePressed = event->getIf<sf::Event::MouseButtonPressed>())
			{
				// left mouse button clicked
				if (mousePressed->button == sf::Mouse::Button::Left)
				{
					checkTextInputClicked(textInputList, mousePosition);
					checkButtonClicked(buttonList, textInputList, mousePosition);
				}
			}

			// user typing
			if (const auto* textEntered = event->getIf<sf::Event::TextEntered>())
			{
				if (textEntered->unicode < 128)
				{
					char enterdChar = static_cast<char>(textEntered->unicode);
					onTextEntered(enterdChar, textInputList);
				}
			}
		}

		window.clear(sf::Color::White);

		// render window

		sf::Font font("fonts/Roboto-Regular.ttf");

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
		// drawTextInput(window, widthTextInput);

		// Height Text
		sf::Text heightText(font);
		heightText.setString("Height");
		heightText.setCharacterSize(22);
		heightText.setFillColor(sf::Color::Black);
		heightText.setPosition({ 792.0f + 300.0f, 180.0f });
		window.draw(heightText);

		// Height Text Input
		// drawTextInput(window, heightTextInput);

		// Speed Text
		sf::Text speedText(font);
		speedText.setString("Speed");
		speedText.setCharacterSize(22);
		speedText.setFillColor(sf::Color::Black);
		speedText.setPosition({ 942.0f, 180.0f + 100.0f });
		window.draw(speedText);

		// Speed Text Input
		// drawTextInput(window, speedTextInput);

		// Generate Button
		// drawButton(window, generateButton);

		// Save Button
		// drawButton(window, saveButton);

		// Load Button
		// drawButton(window, loadButton);

		// Path Finding Button
		// drawButton(window, pathFindingButton);

		
		// draw text inputs
		for (TextInput& textInput : textInputList)
		{
			drawTextInput(window, textInput);
		}

		// draw buttons
		for (Button& button : buttonList)
		{
			drawButton(window, button);
		}
		

		window.display();
	}
}
