#include <vector>
#include <iostream>
#include <functional>
#include "render.hpp"
#include "maze.hpp"
#include "path_finding.hpp"

enum Page
{
	MAZE_GEN, PATH_FIND
};

enum Dialog
{
	NONE, SAVE_MAZE, LOAD_MAZE
};

struct TextInput
{
	Page textInputPage = MAZE_GEN;
	Dialog textInputDialog = NONE;
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
	Page buttonPage = MAZE_GEN;
	Dialog buttonDialog = NONE;
	std::string buttonName = "Button";
	sf::Vector2f position = { 0.0f, 0.0f };
	sf::Vector2f size = { 120.0f, 60.0f };
	sf::Color buttonColor = sf::Color(190, 190, 190);
	sf::Color textColor = sf::Color::Black;
};

struct InputDialog
{
	Dialog dialogType = NONE;
	sf::Vector2f position = { 390.0f, 185.0f };
	sf::Vector2f size = { 500.0f, 350.0f };
	std::string title = "Input Dialog";
	std::string descString = "Description"; // description
	std::string statusString = "";

	int titleCharSize = 32;
	int descCharSize = 22;
	int statusCharSize = 18;

	sf::Color statusTextColor = sf::Color::Black;

	sf::Vector2f titlePos;
	sf::Vector2f descPos;
	sf::Vector2f statusPos;

	TextInput textInput;
	Button okButton;
	Button cancelButton;
};

// General Variables - used in both pages
sf::Font font;

Page currentPage = MAZE_GEN;
Dialog currentDialog = NONE;

Maze mazeObj;
sf::Vector2f mazeSize = { 650.0f, 500.0f };
sf::Vector2f mazePos = { 40.0f, 80.0f };

// Maze Generator Page Variables
InputDialog saveMazeDialog, loadMazeDialog;

std::vector<TextInput> mazeGenTextInputList;
std::vector<Button> mazeGenButtonList;

MazeGenerator mazeGenerator;

// Path Finding Page Variables
InputDialog savePathDialog, loadPathDialog;

std::vector<TextInput> pathFindTextInputList;
std::vector<Button> pathFindButtonList;

CellPos selectedPathFindingCell[2] = { {-1, -1}, {-1, -1} }; // selected cell to find path from cell a to cell b

Path path;
PathFinder pathFinder;

// General Functions - used in both pages
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
	sf::Text text(font);
	text.setString(button.buttonName);
	text.setFillColor(button.textColor);
	text.setPosition({ button.position.x + 15.0f, button.position.y + 15.0f });
	text.setCharacterSize(20.0f);
	window.draw(text);
}

bool isElementClicked(const sf::Vector2f& mousePosition, const sf::Vector2f& elementPos, const sf::Vector2f& elementSize)
{
	float x = mousePosition.x;
	float y = mousePosition.y;

	float x0 = elementPos.x;
	float y0 = elementPos.y;
	float x1 = elementPos.x + elementSize.x;
	float y1 = elementPos.y + elementSize.y;

	// x0 <= x <= x1 && y0 <= y <= y1
	return x >= x0 && x <= x1 && y >= y0 && y <= y1;
}

void checkTextInputClicked(const sf::Vector2f& mousePosition, TextInput& textInput)
{
	if (isElementClicked(mousePosition, textInput.position, textInput.size))
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

bool isCharInString(char target, std::string str)
{
	for (char c : str)
	{
		if (c == target)
			return true;
	}

	return false;
}

void onTextEntered(char enteredChar, TextInput& textInput)
{
	if (textInput.isListening)
	{
		if (enteredChar == '\b')
		{
			// back space is preesed, remove last character
			if (!textInput.inputString.empty())
				textInput.inputString.pop_back();
		}

		else if (textInput.filterText.empty() || isCharInString(enteredChar, textInput.filterText))
		{
			textInput.inputString += enteredChar;
		}
	}
}

void drawMazeWall(sf::RenderWindow& window, sf::Vector2f mazeRenderSize, sf::Vector2f topLeftPos)
{
	int m = mazeObj.maze_width;
	int n = mazeObj.maze_height;

	if (m <= 0 || n <= 0)
	{
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

			sf::Vector2f startPos = { x0 + r1 * j, y0 + r2 * i };
			sf::Vector2f endPos = { startPos.x, startPos.y + r2 };
			walls.append(sf::Vertex(startPos, sf::Color::Black));
			walls.append(sf::Vertex(endPos, sf::Color::Black));
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
		}
	}

	window.draw(walls);
}

void drawRectAtCell(sf::VertexArray& cellVertexArray, sf::Vector2f topLeftPos, float cellWidth, float cellHeight, CellPos cell, sf::Color color)
{
	sf::Vector2f p1 = { topLeftPos.x + cellWidth * cell.x, topLeftPos.y + cellHeight * cell.y };
	sf::Vector2f p2 = { p1.x + cellWidth, p1.y };
	sf::Vector2f p3 = { p1.x, p1.y + cellHeight };
	sf::Vector2f p4 = { p1.x + cellWidth, p1.y + cellHeight };

	// first triangle
	cellVertexArray.append(sf::Vertex(p1, color));
	cellVertexArray.append(sf::Vertex(p2, color));
	cellVertexArray.append(sf::Vertex(p3, color));

	// second triangle to form a rectangle
	cellVertexArray.append(sf::Vertex(p2, color));
	cellVertexArray.append(sf::Vertex(p3, color));
	cellVertexArray.append(sf::Vertex(p4, color));
}

void initInputDialog(InputDialog& inputDialog)
{
	const float PADDING = 8.0f;

	// create temporary UI objects to calculate positions
	sf::Text titleText(font);
	titleText.setString(inputDialog.title);
	titleText.setCharacterSize(inputDialog.titleCharSize);
	titleText.setFillColor(sf::Color::Black);
	sf::Vector2f titlePos;
	titlePos.x = (2 * inputDialog.position.x + inputDialog.size.x - titleText.getGlobalBounds().size.x) / 2.0f;
	titlePos.y = inputDialog.position.y + PADDING;
	inputDialog.titlePos = titlePos;
	titleText.setPosition(titlePos);

	// set text input and button size before position
	sf::Text descText(font);
	descText.setString(inputDialog.descString);
	descText.setCharacterSize(inputDialog.descCharSize);
	descText.setFillColor(sf::Color::Black);

	float elementDistance = (inputDialog.size.y - 2 * PADDING - titleText.getGlobalBounds().size.y - inputDialog.textInput.size.y - inputDialog.statusCharSize - descText.getGlobalBounds().size.y - inputDialog.okButton.size.y) / 4.0f;
	sf::Vector2f descPos;
	descPos.x = (2 * titleText.getPosition().x + titleText.getGlobalBounds().size.x - descText.getGlobalBounds().size.x) / 2.0f;
	descPos.y = titlePos.y + titleText.getGlobalBounds().size.y + elementDistance;
	inputDialog.descPos = descPos;

	inputDialog.textInput.textInputDialog = inputDialog.dialogType;
	inputDialog.textInput.filterText = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789 _-";
	sf::Vector2f textInputPos;
	textInputPos.x = (2 * titleText.getPosition().x + titleText.getGlobalBounds().size.x - inputDialog.textInput.size.x) / 2.0f;
	textInputPos.y = descPos.y + descText.getGlobalBounds().size.y + elementDistance;
	inputDialog.textInput.position = textInputPos;

	sf::Vector2f statusTextPos;
	statusTextPos.x = 0.0f; // calculate x later
	statusTextPos.y = textInputPos.y + inputDialog.textInput.size.y + elementDistance;
	inputDialog.statusPos = statusTextPos;

	float buttonDistance = (inputDialog.size.x - 2 * inputDialog.okButton.size.x) / 3.0f;

	inputDialog.okButton.buttonDialog = inputDialog.dialogType;
	sf::Vector2f okBtnPos;
	okBtnPos.x = inputDialog.position.x + PADDING + buttonDistance;
	okBtnPos.y = statusTextPos.y + inputDialog.statusCharSize + elementDistance;
	inputDialog.okButton.position = okBtnPos;

	inputDialog.cancelButton.buttonDialog = inputDialog.dialogType;
	sf::Vector2f cancelBtnPos;
	cancelBtnPos.x = okBtnPos.x + inputDialog.okButton.size.x + buttonDistance;
	cancelBtnPos.y = okBtnPos.y;
	inputDialog.cancelButton.position = cancelBtnPos;

	// WHAT AM I EVEN DOING???!!!

}

void drawInputDialog(sf::RenderWindow& window, InputDialog& inputDialog)
{
	sf::RectangleShape borderRect(inputDialog.size);
	borderRect.setPosition(inputDialog.position);
	borderRect.setFillColor(sf::Color::White);
	borderRect.setOutlineThickness(1.5f);
	borderRect.setOutlineColor(sf::Color::Black);
	window.draw(borderRect);

	sf::Text titleText(font);
	titleText.setString(inputDialog.title);
	titleText.setCharacterSize(inputDialog.titleCharSize);
	titleText.setFillColor(sf::Color::Black);
	titleText.setPosition(inputDialog.titlePos);
	window.draw(titleText);

	sf::Text descText(font);
	descText.setString(inputDialog.descString);
	descText.setCharacterSize(inputDialog.descCharSize);
	descText.setFillColor(sf::Color::Black);
	descText.setPosition(inputDialog.descPos);
	window.draw(descText);

	drawTextInput(window, inputDialog.textInput);

	sf::Text statusText(font);
	statusText.setString(inputDialog.statusString);
	statusText.setCharacterSize(inputDialog.statusCharSize);
	statusText.setFillColor(inputDialog.statusTextColor); // color will change base on status
	statusText.setPosition(inputDialog.statusPos);
	window.draw(statusText);


	drawButton(window, inputDialog.okButton);

	drawButton(window, inputDialog.cancelButton);
}

void inputDialogEventHandling(const sf::Vector2f& mousePosition, const std::optional<sf::Event>& event, InputDialog& inputDialog)
{
	if (const auto* mousePressed = event->getIf<sf::Event::MouseButtonPressed>())
	{
		// left mouse button clicked
		if (mousePressed->button == sf::Mouse::Button::Left)
		{
			checkTextInputClicked(mousePosition, inputDialog.textInput);
			if (isElementClicked(mousePosition, inputDialog.cancelButton.position, inputDialog.cancelButton.size))
			{
				inputDialog.statusString = "";
				inputDialog.statusTextColor = sf::Color::Black;
				currentDialog = NONE;
			}
		}
	}

	if (const auto* textEntered = event->getIf<sf::Event::TextEntered>())
	{
		if (textEntered->unicode < 128)
		{
			char enterdChar = static_cast<char>(textEntered->unicode);

			onTextEntered(enterdChar, inputDialog.textInput);
		}
	}
}

sf::Color getStatusColor(MazeStatus status)
{
	sf::Color color;
	switch (status.statusType)
	{
	case IDLE:
		color = sf::Color::Black;
		break;
	case ERROR:
		color = sf::Color::Red;
		break;
	case PROCESSING:
		color = sf::Color(190, 190, 190);
		break;
	case SUCCESSFUL:
		color = sf::Color::Green;
		break;
	case FAILED:
		color = sf::Color::Yellow;
		break;
	default:
		color = sf::Color::Black;
		break;
	}

	return color;
}

void setStatusInDialog(InputDialog& inputDialog, MazeStatus status)
{
	inputDialog.statusString = status.statusMessage;
	inputDialog.statusTextColor = getStatusColor(status);

	// calculate x position
	sf::Text statusText(font);
	statusText.setString(inputDialog.statusString);
	statusText.setCharacterSize(inputDialog.statusCharSize);
	statusText.setFillColor(inputDialog.statusTextColor);
	inputDialog.statusPos.x = (2 * inputDialog.textInput.position.x + inputDialog.textInput.size.x - statusText.getGlobalBounds().size.x) / 2.0f;
}

sf::Vector2f getWindowPosFromCellPos(CellPos cellPos)
{
	int m = mazeObj.maze_width;
	int n = mazeObj.maze_height;

	if (m <= 0 || n <= 0)
	{
		return { -1.0f, -1.0f };
	}

	float x0 = mazePos.x;
	float y0 = mazePos.y;
	float r1 = mazeSize.x / m; // horizontal wall length
	float r2 = mazeSize.y / n; // vertical wall length

	int x = cellPos.x;
	int y = cellPos.y;

	return { x0 + r1*x, y0 + r2*y };
}

sf::Vector2f getMidWindowPosFromCellPos(CellPos cellPos)
{
	int m = mazeObj.maze_width;
	int n = mazeObj.maze_height;

	if (m <= 0 || n <= 0)
	{
		return { -1.0f, -1.0f };
	}

	float x0 = mazePos.x;
	float y0 = mazePos.y;
	float r1 = mazeSize.x / m; // horizontal wall length
	float r2 = mazeSize.y / n; // vertical wall length

	int x = cellPos.x;
	int y = cellPos.y;

	return { x0 + r1 * x + r1 / 2.0f, y0 + r2 * y + r2 / 2.0f };
}

// Maze Generator Functions
void onGenerateButtonClicked()
{
	if (mazeGenerator.mazeStatus.statusType == PROCESSING)
		return;

	sf::Vector2i mazeSize = { 0, 0 };
	int generationDelay = 0;

	for (TextInput& textInput : mazeGenTextInputList)
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

		else if (textInput.textInputName == "Delay")
		{
			if (textInput.inputString.empty())
			{
				generationDelay = 0;
			}
			else
			{
				generationDelay = std::stoi(textInput.inputString);
			}
		}
	}

	std::cout << "Maze Width: " << mazeSize.x << std::endl;
	std::cout << "Maze Height: " << mazeSize.y << std::endl;
	std::cout << "Generation Delay: " << generationDelay << std::endl;

	mazeObj.maze_width = mazeSize.x;
	mazeObj.maze_height = mazeSize.y;
	initMaze(mazeObj);

	if (generationDelay <= 0)
	{
		mazeGenerator.mazeStatus = generateMazeInstantly(mazeObj);
	}
	else
	{
		mazeGenerator.delay = generationDelay;
		startGeneration(mazeGenerator, mazeObj);
	}
}

void onSaveMazeButtonClicked()
{
	if (mazeGenerator.mazeStatus.statusType != SUCCESSFUL)
		return;

	currentDialog = SAVE_MAZE;
}

void onLoadMazeButtonClicked()
{
	if (mazeGenerator.mazeStatus.statusType == PROCESSING)
		return;

	currentDialog = LOAD_MAZE;
}

void onButtonClicked(const sf::Vector2f& mousePosition, std::vector<Button>& buttonList)
{
	for (Button& button : buttonList)
	{
		// x0 <= x <= x1 && y0 <= y <= y1
		if (isElementClicked(mousePosition, button.position, button.size))
		{
			// maze generation page
			if (currentPage == MAZE_GEN)
			{
				if (button.buttonName == "Generate")
				{
					onGenerateButtonClicked();
				}

				else if (button.buttonName == "Save")
				{
					onSaveMazeButtonClicked();
				}

				else if (button.buttonName == "Load")
				{
					onLoadMazeButtonClicked();
				}

				else if (button.buttonName == "Path Finding")
				{
					if (mazeGenerator.mazeStatus.statusType != PROCESSING)
					{
						// Change Page to Path Finding
						currentDialog = NONE;
						currentPage = PATH_FIND;
					}
				}
			}

			else if (currentPage == PATH_FIND)
			{
				if (button.buttonName == "Find Path")
				{
					if (pathFinder.status.statusType != PROCESSING)
					{
						double delay = 0.0f;
						if (!pathFindTextInputList[4].inputString.empty())
						{
							delay = std::stod(pathFindTextInputList[4].inputString);
						}

						pathFinder.startCell = selectedPathFindingCell[0];
						pathFinder.endCell = selectedPathFindingCell[1];
						if (delay <= 0)
						{
							pathFinder.status = findShortPathInstantly(mazeObj, path, selectedPathFindingCell[0], selectedPathFindingCell[1]);
						}

						else
						{
							pathFinder.delay = delay;
							startFindShortPath(mazeObj, pathFinder, path);
						}
					}
				}

				else if (button.buttonName == "Maze Generator")
				{
					if (pathFinder.status.statusType != PROCESSING)
					{
						// Change Page to Maze Generator
						currentDialog = NONE;
						currentPage = MAZE_GEN;
						selectedPathFindingCell[0] = { -1, -1 };
						selectedPathFindingCell[1] = { -1, -1 };
						pathFinder.status.statusType = IDLE;
						pathFinder.status.statusMessage = "";
						pathFinder.startCell = selectedPathFindingCell[0];
						pathFinder.endCell = selectedPathFindingCell[1];
					}
				}
			}

			std::cout << button.buttonName << " is clicked." << std::endl;
			break;

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

bool updateMazeGeneratorWindow()
{
	updateGeneration(mazeGenerator, mazeObj);

	// change page or not
	return false;
}

void initMazeGeneratorWindow()
{
	// text input and button struct
	TextInput widthTextInput;
	widthTextInput.textInputName = "Width";
	widthTextInput.position = { 750.0f, 215.0f };
	mazeGenTextInputList.emplace_back(widthTextInput);

	TextInput heightTextInput;
	heightTextInput.textInputName = "Height";
	heightTextInput.position = { 750.0f + 300.0f, 215.0f };
	mazeGenTextInputList.emplace_back(heightTextInput);

	TextInput delayTextInput;
	delayTextInput.textInputName = "Delay";
	delayTextInput.position = { 900.0f, 215.0f + 100.0f };
	mazeGenTextInputList.emplace_back(delayTextInput);

	Button generateButton;
	generateButton.buttonName = "Generate";
	generateButton.position = { 920.0f, 415.0f };
	mazeGenButtonList.emplace_back(generateButton);

	Button pathFindingButton;
	pathFindingButton.buttonName = "Path Finding";
	pathFindingButton.position = { 920.0f - 10.0f, 560.0f };
	pathFindingButton.size = { 140.0f, 60.0f };
	mazeGenButtonList.emplace_back(pathFindingButton);

	Button saveButton;
	saveButton.buttonName = "Save";
	saveButton.position = { 155.0f, 590.0f };
	mazeGenButtonList.emplace_back(saveButton);

	Button loadButton;
	loadButton.buttonName = "Load";
	loadButton.position = { 155.0f + 200.0f, 590.0f };
	mazeGenButtonList.emplace_back(loadButton);
}

void mazeGeneratorEventHandling(const sf::Vector2f& mousePosition, const std::optional<sf::Event>& event)
{
	// mouse clicked
	if (const auto* mousePressed = event->getIf<sf::Event::MouseButtonPressed>())
	{
		// left mouse button clicked
		if (mousePressed->button == sf::Mouse::Button::Left)
		{
			for (TextInput& textInput : mazeGenTextInputList)
				checkTextInputClicked(mousePosition, textInput);

			onButtonClicked(mousePosition, mazeGenButtonList);
		}
	}

	// user typing
	if (const auto* textEntered = event->getIf<sf::Event::TextEntered>())
	{
		if (textEntered->unicode < 128)
		{
			char enterdChar = static_cast<char>(textEntered->unicode);

			for (TextInput& textInput : mazeGenTextInputList)
			{
				if (enterdChar == '0' && textInput.inputString.empty())
					continue;
				onTextEntered(enterdChar, textInput);
			}
		}
	}
}

void drawMazeGeneratorWindow(sf::RenderWindow& window, const sf::Vector2f& mousePosition)
{
	// Maze Generator Title
	sf::Text titleText(font);
	titleText.setString("Maze Generator");
	titleText.setCharacterSize(30);
	titleText.setFillColor(sf::Color::Black);
	titleText.setPosition({ 256.0, 20.0 });
	window.draw(titleText);

	if (mazeGenerator.mazeStatus.statusType == PROCESSING)
		drawMazeProcess(window, mazeSize, mazePos);

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

	// Delay Text
	sf::Text delayText(font);
	delayText.setString("Delay");
	delayText.setCharacterSize(22);
	delayText.setFillColor(sf::Color::Black);
	delayText.setPosition({ 942.0f, 180.0f + 100.0f });
	window.draw(delayText);

	// Maze Generation Status Text
	sf::Text genStatText(font);
	genStatText.setString(mazeGenerator.mazeStatus.statusMessage);
	genStatText.setCharacterSize(20);
	sf::Color genStatColor = getStatusColor(mazeGenerator.mazeStatus);
	genStatText.setFillColor(genStatColor);
	float delayTextWidth = delayText.getGlobalBounds().size.x;
	float genTextWidth = genStatText.getGlobalBounds().size.x;
	float delayTextXPos = delayText.getPosition().x;
	float genTextXPos = (2* delayTextXPos + delayTextWidth - genTextWidth) / 2.0f;
	genStatText.setPosition({ genTextXPos, 500.0f });
	window.draw(genStatText);

	// draw text inputs
	for (TextInput& textInput : mazeGenTextInputList)
	{
		drawTextInput(window, textInput);
	}

	// draw buttons
	for (Button& button : mazeGenButtonList)
	{
		drawButton(window, button);
	}
}

void initGeneratorDialog()
{
	saveMazeDialog.title = "Save Maze";
	saveMazeDialog.descString = "Enter file name to save the maze";
	saveMazeDialog.okButton.buttonName = "Save";
	saveMazeDialog.cancelButton.buttonName = "Cancel";
	saveMazeDialog.textInput.size = { 300.0f, 40.0f };
	saveMazeDialog.dialogType = SAVE_MAZE;
	initInputDialog(saveMazeDialog);

	loadMazeDialog.title = "Load Maze";
	loadMazeDialog.descString = "Enter file name to load the maze";
	loadMazeDialog.okButton.buttonName = "Load";
	loadMazeDialog.cancelButton.buttonName = "Cancel";
	loadMazeDialog.textInput.size = { 300.0f, 40.0f };
	loadMazeDialog.dialogType = LOAD_MAZE;
	initInputDialog(loadMazeDialog);
}

void saveMazeDialogEventHandling(const sf::Vector2f& mousePosition, const std::optional<sf::Event>& event)
{
	if (const auto* mousePressed = event->getIf<sf::Event::MouseButtonPressed>())
	{
		// left mouse button clicked
		if (mousePressed->button == sf::Mouse::Button::Left)
		{
			if (isElementClicked(mousePosition, saveMazeDialog.okButton.position, saveMazeDialog.okButton.size))
			{
				MazeStatus saveStatus = saveMaze(mazeObj, saveMazeDialog.textInput.inputString);
				setStatusInDialog(saveMazeDialog, saveStatus);
			}
		}
	}
}

void loadMazeDialogEventHandling(const sf::Vector2f& mousePosition, const std::optional<sf::Event>& event)
{
	if (const auto* mousePressed = event->getIf<sf::Event::MouseButtonPressed>())
	{
		// left mouse button clicked
		if (mousePressed->button == sf::Mouse::Button::Left)
		{
			if (isElementClicked(mousePosition, saveMazeDialog.okButton.position, saveMazeDialog.okButton.size))
			{
				MazeStatus loadStatus = loadMaze(mazeObj, loadMazeDialog.textInput.inputString);
				setStatusInDialog(loadMazeDialog, loadStatus);
			}
		}
	}
}

// Path Finding Page
CellPos getMazePosFromWindowPos(sf::Vector2f windowPos)
{
	int m = mazeObj.maze_width;
	int n = mazeObj.maze_height;

	if (m <= 0 || n <= 0)
	{
		std::cerr << "Invalid Cell Number. Unable to get maze position" << std::endl;
		return { -1, -1 };
	}

	float x0 = mazePos.x;
	float y0 = mazePos.y;
	float r1 = mazeSize.x / m; // horizontal wall length
	float r2 = mazeSize.y / n; // vertical wall length

	CellPos mazePos;
	mazePos.x = (windowPos.x - x0) / r1;
	mazePos.y = (windowPos.y - y0) / r2;

	// Invalid Cell
	if (mazePos.x < 0 || mazePos.x >= m || mazePos.y < 0 || mazePos.y >= n)
		return { -1, -1 };

	return mazePos;
}

void initPathFindingWindow()
{
	// Text Input
	// default size = (150, 40)
	TextInput x1TextInput;
	x1TextInput.textInputName = "X1";
	x1TextInput.textInputPage = PATH_FIND;
	x1TextInput.position = { 845.0f, 165.0f };
	x1TextInput.size.x = 100.0f;
	pathFindTextInputList.emplace_back(x1TextInput);

	TextInput y1TextInput;
	y1TextInput.textInputName = "Y1";
	y1TextInput.textInputPage = PATH_FIND;
	y1TextInput.position = { 995.0f, 165.0f };
	y1TextInput.size.x = 100.0f;
	pathFindTextInputList.emplace_back(y1TextInput);

	TextInput x2TextInput;
	x2TextInput.textInputName = "X2";
	x2TextInput.textInputPage = PATH_FIND;
	x2TextInput.position = { 845.0f, 235.0f };
	x2TextInput.size.x = 100.0f;
	pathFindTextInputList.emplace_back(x2TextInput);

	TextInput y2TextInput;
	y2TextInput.textInputName = "Y2";
	y2TextInput.textInputPage = PATH_FIND;
	y2TextInput.position = { 995.0f, 235.0f };
	y2TextInput.size.x = 100.0f;
	pathFindTextInputList.emplace_back(y2TextInput);

	TextInput delayTextInput;
	delayTextInput.textInputName = "Delay";
	delayTextInput.textInputPage = PATH_FIND;
	delayTextInput.position = { 891.5f, 352.0f };
	pathFindTextInputList.emplace_back(delayTextInput);

	// Button
	// default size = (120, 60)
	Button findPathButton;
	findPathButton.buttonName = "Find Path";
	findPathButton.buttonPage = PATH_FIND;
	findPathButton.position = { 906.5f, 412.0f };
	pathFindButtonList.emplace_back(findPathButton);

	Button mazeGenButton;
	mazeGenButton.buttonName = "Maze Generator";
	mazeGenButton.buttonPage = PATH_FIND;
	mazeGenButton.size.x = 175.0f;
	mazeGenButton.position = { 879.0f, 560.0f };
	pathFindButtonList.emplace_back(mazeGenButton);
}

void updatePathFindingWindow()
{
	updateFindShortPath(mazeObj, pathFinder, path);
}

void pathFindingEventHandling(const sf::Vector2f& mousePosition, const std::optional<sf::Event>& event)
{
	// mouse clicked
	if (const auto* mousePressed = event->getIf<sf::Event::MouseButtonPressed>())
	{
		// left mouse button clicked
		if (mousePressed->button == sf::Mouse::Button::Left)
		{
			
			for (TextInput& textInput : pathFindTextInputList)
				checkTextInputClicked(mousePosition, textInput);

			onButtonClicked(mousePosition, pathFindButtonList);

			if (isElementClicked(mousePosition, mazePos, mazeSize))
			{
				// maze clicked
				CellPos mazePos = getMazePosFromWindowPos(mousePosition);
				if (mazePos.x >= 0 && mazePos.y >= 0)
				{
					if (selectedPathFindingCell[0].x < 0 || selectedPathFindingCell[0].y < 0)
					{
						selectedPathFindingCell[0] = mazePos;
						pathFindTextInputList[0].inputString = std::to_string(mazePos.x);
						pathFindTextInputList[1].inputString = std::to_string(mazePos.y);

					}
					else if (selectedPathFindingCell[1].x < 0 || selectedPathFindingCell[1].y < 0)
					{
						selectedPathFindingCell[1] = mazePos;
						pathFindTextInputList[2].inputString = std::to_string(mazePos.x);
						pathFindTextInputList[3].inputString = std::to_string(mazePos.y);
					}
				
				}
			}
			
		}
	}

	// user typing
	if (const auto* textEntered = event->getIf<sf::Event::TextEntered>())
	{
		if (textEntered->unicode < 128)
		{
			char enterdChar = static_cast<char>(textEntered->unicode);

			for (TextInput& textInput : pathFindTextInputList)
			{
				onTextEntered(enterdChar, textInput);

				// update selected cell position
				if (textInput.textInputName == "X1")
				{
					if (textInput.inputString.empty())
						selectedPathFindingCell[0].x = -1;
					else
					{
						int x = stoi(textInput.inputString);
						if (validCell(mazeObj, { x, 0 }))
						{
							selectedPathFindingCell[0].x = x;
						}
						else
						{
							selectedPathFindingCell[0].x = -1;
						}
					}
				}

				else if (textInput.textInputName == "Y1")
				{
					if (textInput.inputString.empty())
						selectedPathFindingCell[0].y = -1;
					else
					{
						int y = stoi(textInput.inputString);
						if (validCell(mazeObj, { 0, y }))
						{
							selectedPathFindingCell[0].y = y;
						}
						else
						{
							selectedPathFindingCell[0].y = -1;
						}
					}
				}

				if (textInput.textInputName == "X2")
				{
					if (textInput.inputString.empty())
						selectedPathFindingCell[1].x = -1;
					else
					{
						int x = stoi(textInput.inputString);
						if (validCell(mazeObj, { x, 0 }))
						{
							selectedPathFindingCell[1].x = x;
						}
						else
						{
							selectedPathFindingCell[1].x = -1;
						}
					}
				}

				else if (textInput.textInputName == "Y2")
				{
					if (textInput.inputString.empty())
						selectedPathFindingCell[1].y = -1;
					else
					{
						int y = stoi(textInput.inputString);
						if (validCell(mazeObj, { 0, y }))
						{
							selectedPathFindingCell[1].y = y;
						}
						else
						{
							selectedPathFindingCell[1].y = -1;
						}
					}
				}
			}
			
		}
	}
}

void drawMazePath(sf::RenderWindow& window)
{
	if (pathFinder.status.statusType != SUCCESSFUL)
		return;

	int m = mazeObj.maze_width;
	int n = mazeObj.maze_height;

	if (m <= 0 || n <= 0)
	{
		std::cerr << "Maze Width or Height is less than or equals to 0, unable to draw maze path." << std::endl;
		return;
	}

	float cellWidth = mazeSize.x / m;
	float cellHeight = mazeSize.y / n;

	CellPos currentCell = pathFinder.endCell;
	CellPos nextCell = path.parents[currentCell.y][currentCell.x];
	sf::VertexArray cellVertexArray(sf::PrimitiveType::Lines);
	while (nextCell.x >= 0 && nextCell.y >= 0)
	{
		// draw line
		sf::Vector2f p1 = getMidWindowPosFromCellPos(currentCell);
		sf::Vector2f p2 = getMidWindowPosFromCellPos(nextCell);
		sf::Color color = sf::Color::Blue;

		cellVertexArray.append(sf::Vertex(p1, color));
		cellVertexArray.append(sf::Vertex(p2, color));

		// update cell
		currentCell = nextCell;
		nextCell = path.parents[currentCell.y][currentCell.x];
	}

	window.draw(cellVertexArray);
}

void drawPathFindingProcess(sf::RenderWindow& window)
{
	if (pathFinder.status.statusType != PROCESSING)
		return;

	int m = mazeObj.maze_width;
	int n = mazeObj.maze_height;

	if (m <= 0 || n <= 0)
	{
		std::cerr << "Maze Width or Height is less than or equals to 0, unable to draw maze path." << std::endl;
		return;
	}

	float cellWidth = mazeSize.x / m;
	float cellHeight = mazeSize.y / n;

	sf::VertexArray cellVertexArray(sf::PrimitiveType::Triangles);

	// explored
	for (CellPos exploredCell : pathFinder.exploredPos)
	{
		drawRectAtCell(cellVertexArray, mazePos, cellWidth, cellHeight, exploredCell, sf::Color::Cyan);
	}

	// queue
	std::queue<CellPos> copyQ = pathFinder.q; // make a copy of a queue
	while (!copyQ.empty())
	{
		drawRectAtCell(cellVertexArray, mazePos, cellWidth, cellHeight, copyQ.front(), sf::Color::Yellow);
		copyQ.pop();
	}

	// front
	if (!pathFinder.q.empty())
		drawRectAtCell(cellVertexArray, mazePos, cellWidth, cellHeight, pathFinder.q.front(), sf::Color::Red);

	window.draw(cellVertexArray);
}

void drawPathFindingWindow(sf::RenderWindow& window, const sf::Vector2f& mousePosition)
{
	// Path Finding Title
	sf::Text titleText(font);
	titleText.setString("Path Finding");
	titleText.setCharacterSize(30);
	titleText.setFillColor(sf::Color::Black);
	titleText.setPosition({ 280.0f, 20.0f });
	window.draw(titleText);

	// X Text
	sf::Text xText(font);
	xText.setString("X");
	xText.setCharacterSize(22);
	xText.setFillColor(sf::Color::Black);
	xText.setPosition({ 884.0f, 128.0f });
	window.draw(xText);

	// Y Text
	sf::Text yText(font);
	yText.setString("Y");
	yText.setCharacterSize(22);
	yText.setFillColor(sf::Color::Black);
	yText.setPosition({ 1034.0f, 128.0f });
	window.draw(yText);

	// Cell A Text
	sf::Text cellAText(font);
	cellAText.setString("Cell A");
	cellAText.setCharacterSize(22);
	cellAText.setFillColor(sf::Color::Black);
	cellAText.setPosition({ 745.0f, 174.0f });
	window.draw(cellAText);

	// Cell B Text
	sf::Text cellBText(font);
	cellBText.setString("Cell B");
	cellBText.setCharacterSize(22);
	cellBText.setFillColor(sf::Color::Black);
	cellBText.setPosition({ 745.0f, 244.0f });
	window.draw(cellBText);

	// Delay Text
	sf::Text delayText(font);
	delayText.setString("Delay");
	delayText.setCharacterSize(22);
	delayText.setFillColor(sf::Color::Black);
	delayText.setPosition({ 940.0f, 310.0f });
	window.draw(delayText);

	// Path Finding Status Text
	sf::Text pathFindStatText(font);
	pathFindStatText.setString(pathFinder.status.statusMessage);
	pathFindStatText.setCharacterSize(20);
	sf::Color pathFindStatColor = getStatusColor(pathFinder.status);
	pathFindStatText.setFillColor(pathFindStatColor);
	float delayTextWidth = delayText.getGlobalBounds().size.x;
	float statTextWidth = pathFindStatText.getGlobalBounds().size.x;
	float delayTextXPos = delayText.getPosition().x;
	float statTextXPos = (2 * delayTextXPos + delayTextWidth - statTextWidth) / 2.0f;
	pathFindStatText.setPosition({ statTextXPos, 500.0f });
	window.draw(pathFindStatText);
	
	drawPathFindingProcess(window);
	drawMazePath(window);

	// draw cell a and cell b points
	if (mazeObj.maze_width > 0 && mazeObj.maze_height > 0)
	{
		const float PADDING = 2.0f;

		float radius = std::min(mazeSize.x / mazeObj.maze_width, mazeSize.y / mazeObj.maze_height);
		radius /= 2;
		radius -= PADDING;

		if (selectedPathFindingCell[0].x >= 0 && selectedPathFindingCell[0].y >= 0)
		{
			sf::CircleShape cellAPointCircle(radius);
			cellAPointCircle.setFillColor(sf::Color::Green);
			sf::Vector2f circlePos = getWindowPosFromCellPos(selectedPathFindingCell[0]);
			circlePos.x += PADDING;
			circlePos.y += PADDING;
			cellAPointCircle.setPosition(circlePos);
			window.draw(cellAPointCircle);
		}

		if (selectedPathFindingCell[1].x >= 0 && selectedPathFindingCell[1].y >= 0)
		{
			sf::CircleShape cellBPointCircle(radius);
			cellBPointCircle.setFillColor(sf::Color::Red);
			sf::Vector2f circlePos = getWindowPosFromCellPos(selectedPathFindingCell[1]);
			circlePos.x += PADDING;
			circlePos.y += PADDING;
			cellBPointCircle.setPosition(circlePos);
			window.draw(cellBPointCircle);
		}
	}
	
	

	// draw text inputs and buttons
	for (TextInput& textInput : pathFindTextInputList)
		drawTextInput(window, textInput);
	for (Button& button : pathFindButtonList)
		drawButton(window, button);

}

// Main - functions called by main
void init(sf::Font windowFont)
{
	font = windowFont;

	initMazeGeneratorWindow();
	initGeneratorDialog();

	initPathFindingWindow();

	// Debug
	/*
	loadMaze(mazeObj, "TestMaze");
	currentPage = PATH_FIND;
	*/
}

void update()
{
	if (currentPage == MAZE_GEN && currentDialog == NONE)
		updateMazeGeneratorWindow();

	if (currentPage == PATH_FIND && currentDialog == NONE)
		updatePathFindingWindow();
}

void eventHandling(sf::RenderWindow& window, const sf::Vector2f& mousePosition)
{
	while (const std::optional<sf::Event> event = window.pollEvent())
	{
		if (event->is<sf::Event::Closed>())
			window.close();

		if (currentDialog == NONE)
		{
			if (currentPage == MAZE_GEN)
				mazeGeneratorEventHandling(mousePosition, event);

			else if (currentPage == PATH_FIND)
				pathFindingEventHandling(mousePosition, event);
		}
		
		else
		{
			if (currentPage == MAZE_GEN)
			{
				if (currentDialog == SAVE_MAZE)
				{
					inputDialogEventHandling(mousePosition, event, saveMazeDialog);
					saveMazeDialogEventHandling(mousePosition, event);

				}
				else if (currentDialog == LOAD_MAZE)
				{
					inputDialogEventHandling(mousePosition, event, loadMazeDialog);
					loadMazeDialogEventHandling(mousePosition, event);
				}
			}

		}
	}
}

void draw(sf::RenderWindow& window, const sf::Vector2f& mousePosition)
{
	window.clear(sf::Color::White);

	if (currentPage == MAZE_GEN)
		drawMazeGeneratorWindow(window, mousePosition);
	else if (currentPage == PATH_FIND)
		drawPathFindingWindow(window, mousePosition);

	// both in maze generator and path finder page
	drawMazeWall(window, mazeSize, mazePos);

	// Mouse Position Text For Debug
	/*
	sf::Text mousePosText(font);
	mousePosText.setString("Mouse Position: (" +
		std::to_string(mousePosition.x) + "," +
		std::to_string(mousePosition.y) + ")");
	mousePosText.setCharacterSize(16);
	mousePosText.setFillColor(sf::Color::Black);
	mousePosText.setPosition({ 16.0, 680.0 });
	window.draw(mousePosText);
	*/

	if (currentPage == MAZE_GEN)
	{
		if (currentDialog == SAVE_MAZE)
			drawInputDialog(window, saveMazeDialog);
		else if (currentDialog == LOAD_MAZE)
			drawInputDialog(window, loadMazeDialog);
	}
}
