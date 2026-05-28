#include <vector>
#include <iostream>
#include <functional>
#include "render.hpp"
#include "maze.hpp"

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

const std::string FONT_PATH = "fonts/Roboto-Regular.ttf";
const std::string FONT_LOAD_ERROR = "Unable to open font from " + FONT_PATH 
								+ "Please make sure there is a fonts folder containing Roboto-Regular.ttf in the same directory as CMakeList.txt, and configure CMake";

Maze mazeObj;
MazeGenerator mazeGenerator;

void drawTextInput(sf::RenderWindow& window, TextInput& textInput, const sf::Font& font)
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
		cursorRect.setPosition({ text.getPosition().x + text.getGlobalBounds().size.x + 2.5f,
							     text.getPosition().y + 2.5f });
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

void drawButton(sf::RenderWindow& window, const Button& button, const sf::Font& font)
{
	// button rect
	sf::RectangleShape rect(button.size);
	rect.setFillColor(button.buttonColor);
	rect.setOutlineColor(sf::Color::Black);
	rect.setOutlineThickness(2.0f);
	rect.setPosition(button.position);
	window.draw(rect);

	// button text
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

void onGenerateButtonClicked(std::vector<TextInput>& textInputList)
{
	if (mazeGenerator.mazeStatus.statusType == PROCESSING)
		return;

	sf::Vector2i mazeSize = { 0, 0 };
	int generationSpeed = 0;

	for (TextInput& textInput : textInputList)
	{

		if (textInput.textInputName == "Width")
		{
			if (textInput.inputString.empty())
			{
				std::cout << "Width input is empty, will use default value." << std::endl;
				mazeSize.x = 6;
			}

			else
			{
				mazeSize.x = std::stoi(textInput.inputString);
			}
		}

		else if (textInput.textInputName == "Height")
		{
			if (textInput.inputString.empty())
			{
				std::cout << "Height input is empty, will use default value." << std::endl;
				mazeSize.y = 5;
			}

			else
			{
				mazeSize.y = std::stoi(textInput.inputString);
			}
		}

		else if (textInput.textInputName == "Speed")
		{
			if (textInput.inputString.empty())
			{
				generationSpeed = 0;
			}
			else
			{
				generationSpeed = std::stoi(textInput.inputString);
			}
		}
	}

	std::cout << "Maze Width: " << mazeSize.x << std::endl;
	std::cout << "Maze Height: " << mazeSize.y << std::endl;
	std::cout << "Generation Speed: " << generationSpeed << std::endl;

	mazeObj.maze_width = mazeSize.x;
	mazeObj.maze_height = mazeSize.y;
	initMaze(mazeObj);

	if (generationSpeed <= 0)
	{
		mazeGenerator.mazeStatus = generateMazeInstantly(mazeObj);
	}
	else
	{
		startGeneration(mazeGenerator, mazeObj);
	}
}

void checkButtonClicked(std::vector<Button>& buttonList, std::vector<TextInput>& textInputList, const sf::Vector2f& mousePosition)
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

void drawMazeProcess(sf::RenderWindow& window, sf::Vector2f mazeRenderSize, sf::Vector2f topLeftPos)
{
	// draw visited cell first, then deadend, and lastly current cell
	int m = mazeObj.maze_width;
	int n = mazeObj.maze_height;

	if (m <= 0 || n <= 0)
	{
		std::cerr << "Maze Width or Height is less than or equals to 0, unable to draw maze process." << std::endl;
		return;
	}

	float padding = 0.0f;
	float cellWidth = (mazeRenderSize.x / m) - (2.0f * padding); 
	float cellHeight = (mazeRenderSize.y / n) - (2.0f * padding);

	sf::VertexArray cellVertexArray(sf::PrimitiveType::Triangles);

	for (CellPos cell : mazeGenerator.visitedPos)
	{
		sf::Vector2f p1 = { topLeftPos.x + cellWidth*cell.x + padding, topLeftPos.y + cellHeight*cell.y + padding };
		sf::Vector2f p2 = { p1.x + cellWidth, p1.y };
		sf::Vector2f p3 = { p1.x, p1.y + cellHeight };
		sf::Vector2f p4 = { p1.x + cellWidth, p1.y + cellHeight };

		sf::Color color = sf::Color::Yellow;

		// first triangle
		cellVertexArray.append(sf::Vertex(p1, color));
		cellVertexArray.append(sf::Vertex(p2, color));
		cellVertexArray.append(sf::Vertex(p3, color));

		// second triangle to form a rectangle
		cellVertexArray.append(sf::Vertex(p2, color));
		cellVertexArray.append(sf::Vertex(p3, color));
		cellVertexArray.append(sf::Vertex(p4, color));
	}

	for (CellPos cell : mazeGenerator.deadEndPos)
	{
		sf::Vector2f p1 = { topLeftPos.x + cellWidth * cell.x + padding, topLeftPos.y + cellHeight * cell.y + padding };
		sf::Vector2f p2 = { p1.x + cellWidth, p1.y };
		sf::Vector2f p3 = { p1.x, p1.y + cellHeight };
		sf::Vector2f p4 = { p1.x + cellWidth, p1.y + cellHeight };

		sf::Color color = sf::Color::Green;

		// first triangle
		cellVertexArray.append(sf::Vertex(p1, color));
		cellVertexArray.append(sf::Vertex(p2, color));
		cellVertexArray.append(sf::Vertex(p3, color));

		// second triangle to form a rectangle
		cellVertexArray.append(sf::Vertex(p2, color));
		cellVertexArray.append(sf::Vertex(p3, color));
		cellVertexArray.append(sf::Vertex(p4, color));
	}

	// current cell
	if (mazeGenerator.cellStack.empty())
	{
		window.draw(cellVertexArray);
		return;
	}

	CellPos cell = mazeGenerator.cellStack.top();
	sf::Vector2f p1 = { topLeftPos.x + cellWidth * cell.x + padding, topLeftPos.y + cellHeight * cell.y + padding };
	sf::Vector2f p2 = { p1.x + cellWidth, p1.y };
	sf::Vector2f p3 = { p1.x, p1.y + cellHeight };
	sf::Vector2f p4 = { p1.x + cellWidth, p1.y + cellHeight };

	sf::Color color = sf::Color::Blue;

	// first triangle
	cellVertexArray.append(sf::Vertex(p1, color));
	cellVertexArray.append(sf::Vertex(p2, color));
	cellVertexArray.append(sf::Vertex(p3, color));

	// second triangle to form a rectangle
	cellVertexArray.append(sf::Vertex(p2, color));
	cellVertexArray.append(sf::Vertex(p3, color));
	cellVertexArray.append(sf::Vertex(p4, color));

	window.draw(cellVertexArray);
}

void drawMazeWall(sf::RenderWindow& window, sf::Vector2f mazeRenderSize, sf::Vector2f topLeftPos)
{
	int m = mazeObj.maze_width;
	int n = mazeObj.maze_height;

	if (m <= 0 || n <= 0)
	{
		std::cerr << "Maze Width or Height is less than or equals to 0, unable to draw maze walls." << std::endl;
		return;
	}

	float x0 = topLeftPos.x;
	float y0 = topLeftPos.y;
	float r1 = mazeRenderSize.x / m; // horizontal wall length
	float r2 = mazeRenderSize.y / n; // vertical wall length
	const float WALL_THICKNESS = 2.5f;

	sf::VertexArray walls(sf::PrimitiveType::Lines);

	// draw vertical walls
	for (int i = 0; i < mazeObj.vertical_walls.size(); i++)
	{
		for (int j = 0; j < mazeObj.vertical_walls[i].size(); j++)
		{
			if (!mazeObj.vertical_walls[i][j])
				continue;

			sf::Vector2f startPos = { x0 + r1*j, y0 + r2*i };
			sf::Vector2f endPos = { startPos.x, startPos.y + r2 };
			walls.append(sf::Vertex(startPos, sf::Color::Black));
			walls.append(sf::Vertex(endPos, sf::Color::Black));

			/*
			sf::RectangleShape wallRect({ WALL_THICKNESS, r2 });
			wallRect.setFillColor(sf::Color::Black);
			wallRect.setPosition(wallPos);
			window.draw(wallRect);
			*/
		}
	}

	// draw horizontal walls
	for (int i = 0; i < mazeObj.horizontal_walls.size(); i++)
	{
		for (int j = 0; j < mazeObj.horizontal_walls[i].size(); j++)
		{
			if (!mazeObj.horizontal_walls[i][j])
				continue;

			sf::Vector2f startPos = { x0 + r1 * j, y0 + r2 * i };
			sf::Vector2f endPos = { startPos.x + r1, startPos.y };
			walls.append(sf::Vertex(startPos, sf::Color::Black));
			walls.append(sf::Vertex(endPos, sf::Color::Black));

			/*
			sf::RectangleShape wallRect({ r1, WALL_THICKNESS });
			wallRect.setFillColor(sf::Color::Black);
			wallRect.setPosition(wallPos);
			window.draw(wallRect);
			*/
		}
	}

	window.draw(walls);
}

void update()
{
	updateGeneration(mazeGenerator, mazeObj);
}

void initMazeGeneratorWindow(std::vector<TextInput>& textInputList, std::vector<Button>& buttonList)
{
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
	generateButton.position = { 920.0f, 415.0f };
	buttonList.emplace_back(generateButton);

	Button pathFindingButton;
	pathFindingButton.buttonName = "Path Finding";
	pathFindingButton.position = { 920.0f - 10.0f, 615.0f };
	pathFindingButton.size = { 140.0f, 60.0f };
	buttonList.emplace_back(pathFindingButton);

	Button saveButton;
	saveButton.buttonName = "Save";
	saveButton.position = { 155.0f, 590.0f };
	buttonList.emplace_back(saveButton);

	Button loadButton;
	loadButton.buttonName = "Load";
	loadButton.position = { 155.0f + 200.0f, 590.0f };
	buttonList.emplace_back(loadButton);
}

void drawMazeGeneratorWindow(sf::RenderWindow& window, std::vector<TextInput>& textInputList, 
							 std::vector<Button>& buttonList, const sf::Font& font, const sf::Vector2f mousePosition)
{
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

	sf::Vector2f mazeSize = { 650.0f, 500.0f };
	sf::Vector2f mazePos = { 40.0f, 80.0f };

	if (mazeGenerator.mazeStatus.statusType == PROCESSING)
		drawMazeProcess(window, mazeSize, mazePos);

	drawMazeWall(window, mazeSize, mazePos);

	// Width Text
	sf::Text widthText(font);
	widthText.setString("Width");
	widthText.setCharacterSize(22);
	widthText.setFillColor(sf::Color::Black);
	widthText.setPosition({ 792.0f, 180.0f });
	window.draw(widthText);

	// Height Text
	sf::Text heightText(font);
	heightText.setString("Height");
	heightText.setCharacterSize(22);
	heightText.setFillColor(sf::Color::Black);
	heightText.setPosition({ 792.0f + 300.0f, 180.0f });
	window.draw(heightText);

	// Speed Text
	sf::Text speedText(font);
	speedText.setString("Speed");
	speedText.setCharacterSize(22);
	speedText.setFillColor(sf::Color::Black);
	speedText.setPosition({ 942.0f, 180.0f + 100.0f });
	window.draw(speedText);

	// Maze Generation Status Text
	sf::Text genStatText(font);
	genStatText.setString(mazeGenerator.mazeStatus.statusMessage);
	genStatText.setCharacterSize(20);

	sf::Color genStatColor = sf::Color::Black;
	switch (mazeGenerator.mazeStatus.statusType)
	{
	case IDLE:
		genStatColor = sf::Color::Black;
		break;
	case ERROR:
		genStatColor = sf::Color::Red;
		break;
	case PROCESSING:
		genStatColor = sf::Color(190, 190, 190);
		break;
	case SUCCESSFUL:
		genStatColor = sf::Color::Green;
		break;
	default:
		genStatColor = sf::Color::Black;
		break;
	}

	genStatText.setFillColor(genStatColor);
	float speedTextWidth = speedText.getGlobalBounds().size.x;
	float genTextWidth = genStatText.getGlobalBounds().size.x;
	float speedTextXPos = speedText.getPosition().x;
	float genTextXPos = (2*speedTextXPos + speedTextWidth - genTextWidth) / 2.0f;
	genStatText.setPosition({ genTextXPos, 500.0f });
	window.draw(genStatText);

	// draw text inputs
	for (TextInput& textInput : textInputList)
	{
		drawTextInput(window, textInput, font);
	}

	// draw buttons
	for (Button& button : buttonList)
	{
		drawButton(window, button, font);
	}
}

void windowLoop(sf::RenderWindow& window)
{
	std::vector<TextInput> mazeGenTextInputList;
	std::vector<Button> mazeGenButtonList;

	sf::Font font;
	if (!font.openFromFile(FONT_PATH))
	{
		std::cerr << FONT_LOAD_ERROR << std::endl;
		return;
	}

	initMazeGeneratorWindow(mazeGenTextInputList, mazeGenButtonList);

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
					checkTextInputClicked(mazeGenTextInputList, mousePosition);
					checkButtonClicked(mazeGenButtonList, mazeGenTextInputList, mousePosition);
				}
			}

			// user typing
			if (const auto* textEntered = event->getIf<sf::Event::TextEntered>())
			{
				if (textEntered->unicode < 128)
				{
					char enterdChar = static_cast<char>(textEntered->unicode);
					onTextEntered(enterdChar, mazeGenTextInputList);
				}
			}
		}

		update();

		window.clear(sf::Color::White);

		// render window
		drawMazeGeneratorWindow(window, mazeGenTextInputList, mazeGenButtonList, font, mousePosition);
		
		window.display();
	}
}
