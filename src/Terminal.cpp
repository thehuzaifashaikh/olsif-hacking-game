#include "Terminal.h"

Terminal::Terminal(sf::Color backColor, sf::Color foreColor, sf::Color caretColor, const std::string& username, const std::string& computer, const std::string& directory, unsigned int characterSize, unsigned int xPos, unsigned int yPos, unsigned int xSize, unsigned int ySize, bool focused, bool readOnly, bool returnString, sf::RenderWindow *window)
{
	background.setSize(sf::Vector2f(xSize, ySize));
	background.setFillColor(backColor);
	background.setPosition(xPos, yPos);

	andaleMonoFont.loadFromFile("assets/Andale Mono.ttf");
	this->username = username;
	this->computer = computer;
	this->directory = directory;
	if (returnString) {
		terminalOutputOnReturn = '\n' + this->username + "@" + this->computer + ":" + this->directory + "# ";
	} else {
		terminalOutputOnReturn = " ";
	}

	text.setFont(andaleMonoFont);
	text.setString(terminalOutputOnReturn);
	text.setFillColor(foreColor);
	text.setCharacterSize(characterSize);
	text.setPosition(background.getPosition().x + 10, background.getPosition().y);

	textCursorCounter = text.getString().getSize();
	canModify = text.getString().getSize();

	if (!readOnly) {
		cursor.setSize(sf::Vector2f(text.getGlobalBounds().width / text.getString().getSize() + 2,
		                            characterSize + 10));
	} else {
		cursor.setSize(sf::Vector2f(text.getGlobalBounds().width / text.getString().getSize(),
		                            3));
	}
	cursor.setPosition(text.findCharacterPos(textCursorCounter).x, text.findCharacterPos(textCursorCounter).y + characterSize/2);

	cursorVisible = true;
	cursorColor = caretColor;

	this->window = window;
	this->characterSize = characterSize;

//	mainView.reset(sf::FloatRect(0, 0, window->getSize().x, window->getSize().y));
//	terminalView.reset(sf::FloatRect(xPos, yPos, xSize, ySize));
//	terminalView.setViewport(sf::FloatRect(worldCoordToRatio(sf::Vector2f(xPos, yPos)).x, worldCoordToRatio(sf::Vector2f(xPos, yPos)).y, worldCoordToRatio(sf::Vector2f(xPos + xSize, yPos)).x, worldCoordToRatio(sf::Vector2f(xPos, yPos + ySize)).y));

	this->isFocused = focused;
	this->readOnly = readOnly;
	this->returnString = returnString;
	this->outputPeriodicallyBoolean = false;
}

void Terminal::input(const sf::Event& ev)
{
	if (!readOnly) {
		if (isFocused) {
			if (ev.type == sf::Event::Closed) {
				window->close();
			} else if (ev.type == sf::Event::KeyPressed) {
				cursorVisible = true;
				blinkingCursorClock.restart();

				if (ev.key.code == sf::Keyboard::Escape) {
					window->close();
				} else if (ev.key.code == sf::Keyboard::Right) {
					if (textCursorCounter < text.getString().getSize()) {
						textCursorCounter++;
					}
				} else if (ev.key.code == sf::Keyboard::Left) {
					if (textCursorCounter > 0) {
						textCursorCounter--;
					}
				}
			} else if (ev.type == sf::Event::TextEntered) {

				if (cursor.getPosition().y > (background.getPosition().y + background.getSize().y - 50)) {
					text.setPosition(text.getPosition().x,
					                 background.getPosition().y + background.getSize().y -
					                 text.getGlobalBounds().height - 20);
				}
				if (ev.text.unicode == 8) {
					if (textCursorCounter > 0 && textCursorCounter > canModify) {
						text.setString(text.getString().substring(0, textCursorCounter - 1) +
						               text.getString().substring(textCursorCounter, text.getString().getSize()));
						textCursorCounter--;
					}

					if (text.getPosition().y > background.getSize().y - text.getGlobalBounds().height - 40) {
						text.setPosition(text.getPosition().x, background.getSize().y - text.getGlobalBounds().height - 40);
					}
				} else if (static_cast<char>(ev.text.unicode) == '\n') {

					func(text.getString().substring(canModify, text.getString().getSize() - canModify).toAnsiString());

					if (returnString) {
						output(terminalOutputOnReturn);
					}
					textCursorCounter = text.getString().getSize();
					canModify = textCursorCounter;

				// if (cursor.getPosition().y > (background.getPosition().y + background.getSize().y - 40)) {
				// 	text.move(0, -(cursor.getPosition().y - (background.getPosition().y + background.getSize().y - 60)));
				// }
					if (text.getPosition().y > background.getSize().y - text.getGlobalBounds().height - 40) {
						text.setPosition(text.getPosition().x, background.getSize().y - text.getGlobalBounds().height - 40);
					}


					//std::cout << text.getString().substring(canModify, 3).toAnsiString() << std::endl;

				} else if (ev.text.unicode < 128) {

					if (text.getPosition().y > background.getSize().y - text.getGlobalBounds().height - 40) {
						text.setPosition(text.getPosition().x, background.getSize().y - text.getGlobalBounds().height - 40);
					}

				// if (cursor.getPosition().y > (background.getPosition().y + background.getSize().y)) {
				// 	text.move(0, -(cursor.getPosition().y - (background.getPosition().y + background.getSize().y - 50)));
				// }

					text.setString(
							text.getString().substring(0, textCursorCounter) + static_cast<char>(ev.text.unicode) +
							text.getString().substring(textCursorCounter, text.getString().getSize()));
					textCursorCounter++;
				}
			} else if (ev.type == sf::Event::MouseWheelScrolled) {
				if (ev.mouseWheelScroll.delta > 0) {
					// Scrolling up
					if (text.getPosition().y <= 5) {
						text.move(0, ev.mouseWheelScroll.delta * 4);
					}
				}

				if (ev.mouseWheelScroll.delta < 0) {
					// Scrolling down
					if (text.getPosition().y + text.getGlobalBounds().height >=
					    background.getGlobalBounds().height / 2) {
						text.move(0, ev.mouseWheelScroll.delta * 4);
					}
				}
			}
		}
	}
}

void Terminal::update()
{
	if (!readOnly) {
		cursor.setPosition(text.findCharacterPos(textCursorCounter).x,
		                   text.findCharacterPos(textCursorCounter).y + 2);
	} else {
		cursor.setPosition(text.findCharacterPos(textCursorCounter).x,
		                   text.findCharacterPos(textCursorCounter).y + characterSize + 2);
	}

	if (!readOnly) {
		if (isFocused) {

			if (blinkingCursorClock.getElapsedTime().asMilliseconds() >= 600) {
				cursorVisible = !cursorVisible;
				blinkingCursorClock.restart();
			}

			cursor.setFillColor((cursorVisible ? cursorColor : sf::Color::Transparent));
		}

		// This input handling in an update method is an exception
		if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)) {
			if (background.getGlobalBounds().contains(sf::Mouse::getPosition(*window).x,
			                                          sf::Mouse::getPosition(*window).y)) {
				isFocused = true;
				cursor.setOutlineThickness(0);
			} else {
				isFocused = false;
			}
		}
	} else {
		if (blinkingCursorClock.getElapsedTime().asMilliseconds() >= 600) {
			cursorVisible = !cursorVisible;
			blinkingCursorClock.restart();
		}

		cursor.setFillColor((cursorVisible ? cursorColor : sf::Color::Transparent));
	}

	if (outputPeriodicallyBoolean) 
	{
		if (periodicOutputClock.getElapsedTime().asSeconds() >= (outputPeriodicallyTime / outputPeriodicallyStr.length()) * periodicOutputCounter)
		{
			this->output(outputPeriodicallyStr[periodicOutputCounter]);
			periodicOutputCounter++;
		}
		if ((outputPeriodicallyStr.length()) == periodicOutputCounter) {
			outputPeriodicallyBoolean = false;
			canModify = textCursorCounter;
		} 
	}
}

void Terminal::render()
{
	window->draw(background);
	window->draw(text);

	if (!isFocused && !readOnly) {
		cursor.setFillColor(background.getFillColor());
		cursor.setOutlineThickness(2);
		cursor.setOutlineColor(cursorColor);
	}
	window->draw(cursor);
}

void Terminal::setCallbackFunctionOnReturn(void(*function)(const std::string&))
{
	this->func = function;
}

void Terminal::output(const std::string& str)
{
	text.setString(text.getString() + str);
	textCursorCounter = text.getString().getSize();
	if (text.getPosition().y > background.getSize().y - text.getGlobalBounds().height - 40) {
		text.setPosition(text.getPosition().x, background.getSize().y - text.getGlobalBounds().height - 40);
	}
	canModify = textCursorCounter;
}

void Terminal::output(char chr) 
{
	text.setString(text.getString() + chr);
	textCursorCounter = text.getString().getSize();
	if (text.getPosition().y > background.getSize().y - text.getGlobalBounds().height - 40) {
		text.setPosition(text.getPosition().x, background.getSize().y - text.getGlobalBounds().height - 40);
	}
	canModify = textCursorCounter;
}

void Terminal::outputPeriodically(const std::string& str, float time)
{
	this->outputPeriodicallyBoolean = true;
	this->outputPeriodicallyStr = str;
	this->outputPeriodicallyTime = time;
	this->periodicOutputCounter = 0;

	periodicOutputClock.restart();
}

void Terminal::clear()
{
	text.setString("");
	textCursorCounter = 0;
}

void Terminal::setReadOnly(bool readOnly)
{
	this->readOnly = readOnly;
}

void Terminal::setFocus(bool focus)
{
	this->isFocused = focus;
}

sf::Vector2f Terminal::worldCoordToRatio(sf::Vector2f coord)
{
	sf::Vector2f ratioCoord;
	ratioCoord.x = coord.x / window->getSize().x;
	ratioCoord.y = coord.y / window->getSize().y;

	return ratioCoord;
}
