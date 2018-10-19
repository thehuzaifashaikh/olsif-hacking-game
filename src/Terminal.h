#pragma once

#include <iostream>
#include <SFML/Graphics.hpp>

class Terminal
{
public:
	Terminal(sf::Color backColor, sf::Color foreColor, sf::Color caretColor, const std::string& username, const std::string& computer, const std::string& directory, unsigned int characterSize, unsigned int xPos, unsigned int yPos, unsigned int xSize, unsigned int ySize, bool focused, bool readOnly, bool returnString, sf::RenderWindow* window);

	void input(const sf::Event& ev);
	void update();
	void render();

	void setCallbackFunctionOnReturn(void(*function)(const std::string&));
	void output(const std::string& str);
	void output(char chr);
	void outputPeriodically(const std::string& str, float time);
	void clear();

	void setReadOnly(bool readOnly);
	void setFocus(bool focus);

private:
	sf::Vector2f worldCoordToRatio(sf::Vector2f coord);

	sf::RenderWindow* window;

	sf::RectangleShape background;
	sf::RectangleShape cursor;
	sf::RectangleShape header;

	sf::Text text;
	sf::Font andaleMonoFont;
	sf::Clock blinkingCursorClock;
	sf::Color cursorColor;

	size_t textCursorCounter;
	size_t canModify;
	size_t characterSize;

	bool cursorVisible;
	bool isFocused;
	bool readOnly;
	bool returnString;

	bool outputPeriodicallyBoolean;
	std::string outputPeriodicallyStr;
	float outputPeriodicallyTime;
	sf::Clock periodicOutputClock;
	int periodicOutputCounter;

	std::string username, computer, directory;
	std::string terminalOutputOnReturn;

	void(*func)(const std::string&);
};