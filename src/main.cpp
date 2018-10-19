#include <iostream>
#include <fstream>
#include <vector>
#include <random>

#include <SFML/Graphics.hpp>

#include "Terminal.h"

enum State {
	Splash,
	Load,
	Login,
	Game,
	Exit
};

const int WIDTH = 1920, HEIGHT = 1080;
State state = State::Splash;
sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), "Olsif", sf::Style::Close);
std::ofstream outputStream;

sf::Texture logoTexture;
sf::Sprite logoSpr;
sf::Clock deltaClock;
sf::Time passedTime;
sf::Color logoColor;

Terminal loadTerminal(sf::Color::Black, sf::Color::White, sf::Color::Yellow , "", "", "", (int)WIDTH/80,0, 0, WIDTH, HEIGHT, false, true, false, &window);
std::ifstream inputStream;
std::vector<std::string> bootlogs;
float delayTime = 9.0f;
float logDoneTime;
int logCounter = 0;
bool loadingDone = false;

sf::Color loginTerminalBGColor(25, 30, 30, 255);
Terminal loginTerminal(loginTerminalBGColor, sf::Color::Green, sf::Color::Green, "", "", "", (int)WIDTH/60, 0, 0, WIDTH, HEIGHT, true, false, false, &window);
bool loggedLoginMenu = false;
enum LoginState {
	Options,
	ShowLoginUsername,
	LoginUsername,
	LoginPassword,
	SignupUsername, 
	SignupPassword
}; 
LoginState loginState = LoginState::Options;
std::string username;
std::string password;
bool usernameIncorrect = false;
bool passwordIncorrect = false;
bool signingUp = false;
bool signedUp = false;
bool userExists;

sf::Color terminalbackColor(50, 50, 60);
sf::Color helpTerminalBackColor(30, 30, 30);
Terminal* terminal;
Terminal helpTerminal(helpTerminalBackColor, sf::Color::Cyan, sf::Color::Cyan, "", "", "", (int)WIDTH/60, 0, 0, WIDTH/2, HEIGHT, false, true, false, &window);

int randomNumber(int min, int max) 
{
	return rand() % (max-min + 1) + min;
}

void interpretGameTerminalCommand(const std::string& command)
{
	// std::cout << command << std::endl;
	if (command != "") {
		terminal->output("\ndone");
	}
}

void interpretLoginTerminalCommand(const std::string& command) 
{
	std::cout << "Text entered: " << command << "***" << std::endl;
	switch (loginState)
	{
		case LoginState::Options:
			if (command == "1") {
				loginState = LoginState::ShowLoginUsername;
			} else if (command == "2") {
				signingUp = true;
				loginState = LoginState::ShowLoginUsername;
			} else {
				loginTerminal.output("\n# ");
				break;
			}

		case LoginState::ShowLoginUsername:
			loginTerminal.clear();
			if (!signingUp) {
				if (usernameIncorrect) {
					loginTerminal.output("Error 201: Username incorrect.\n");
				} else if (passwordIncorrect) {
					loginTerminal.output("Error 202: Password incorrect.\n");
				}
			} 

			if (signedUp) {
				loginTerminal.output("Signup Successful. Enter credentials to log in.\n");
				signedUp = false;
			}

			if (userExists) {
				loginTerminal.output("User already exists.\n");	
				signingUp = true;
				signedUp = false;
				loginState = LoginState::ShowLoginUsername;
			}

			loginTerminal.outputPeriodically("Username: ", 1.0f);
			loginState = LoginState::LoginUsername;	
			break;

		case LoginState::LoginUsername:
			if (!command.empty() && (command.find(' ') == std::string::npos)) {
				username = command;
				loginTerminal.outputPeriodically("\nPassword: ", 1.0f);
				loginState = LoginState::LoginPassword;
			} else {
				loginTerminal.clear();
				loginTerminal.outputPeriodically("Username: ", 1.0f);
			}
			break;

		case LoginState::LoginPassword:
			if (!command.empty()) {
				password = command;

				if (!signingUp) {
					inputStream.close();
					inputStream.open(username);

					if (!inputStream) {
						usernameIncorrect = true;
						std::cout << "Cannot find file: " << username << std::endl;
					} else {
						usernameIncorrect = false;
					}
					std::string storedPassword;
					inputStream >> storedPassword;
					if (storedPassword != password) {
						passwordIncorrect = true;
						loginState = LoginState::ShowLoginUsername;
					} else {
						passwordIncorrect = false;
						terminal = new Terminal(terminalbackColor, sf::Color::Green, sf::Color::Red, username, "Olsif", "/", (int)WIDTH/60, WIDTH/2, 0, WIDTH/2, HEIGHT, true, false, true, &window);
						terminal->setCallbackFunctionOnReturn(interpretGameTerminalCommand);
						state = State::Game;
					}

				} else {
					inputStream.close();
					inputStream.open(username);
					if (!inputStream) {
						outputStream.open(username);
						outputStream << password;
						outputStream.close();
						signedUp = true;
						signingUp = false;
						userExists = false;
					} else {
						userExists = true;
					}
					loginState = LoginState::ShowLoginUsername;
				}
			} else {
				loginTerminal.clear();
				loginTerminal.output("Username: " + username);
				loginTerminal.outputPeriodically("\nPassword: ", 1.0f);
			}

		default:
			if (loginState == LoginState::ShowLoginUsername) {
				loginTerminal.clear();
				if (!signingUp) {
					if (usernameIncorrect) {
						loginTerminal.output("Error 201: Username incorrect.\n");
					} else if (passwordIncorrect) {
						loginTerminal.output("Error 202: Password incorrect.\n");
					}
				} 

				if (signedUp) {
					loginTerminal.output("Signup Successful. Enter credentials to log in.\n");
					signedUp = false;
				}

				if (userExists) {
					loginTerminal.output("User already exists.\n");	
					signingUp = true;
					signedUp = false;
					loginState = LoginState::ShowLoginUsername;
				}

				loginTerminal.outputPeriodically("Username: ", 1.0f);
				loginState = LoginState::LoginUsername;	
				break;
			}
			break;
	}
}



int main()
{
	logoTexture.loadFromFile("assets/logo.png", sf::IntRect(0, 0, 729, 634));
	logoSpr.setTexture(logoTexture);
	logoSpr.setPosition(WIDTH/2 - logoSpr.getGlobalBounds().width/2, HEIGHT/2 - logoSpr.getGlobalBounds().height/2);
	logoSpr.setColor(sf::Color::Transparent);
	logoColor.r = 255;
	logoColor.b = 255;
	logoColor.g = 255;
	logoColor.a = 0;

	loadTerminal.clear();
	inputStream.open("assets/boot.log");
	std::string tmp;
	bootlogs = std::vector<std::string>();
	while (std::getline(inputStream, tmp)) {
		bootlogs.push_back(tmp);
	}
	inputStream.close();

	loginTerminal.setCallbackFunctionOnReturn(interpretLoginTerminalCommand);

	while (window.isOpen()) {
		sf::Event ev;

		while (window.pollEvent(ev)) {
			if (ev.type == sf::Event::Closed) {
				window.close();
			}

			// Input
			if (state == State::Load) {
				loadTerminal.input(ev);
			} else if (state == State::Login) {
				loginTerminal.input(ev);
			} else if (state == State::Game) {
				terminal->input(ev);
			}
		}

		// Update
		sf::Time deltaTime = deltaClock.restart();
		passedTime += deltaTime;
		if (state == State::Splash) {
			if (passedTime.asSeconds() >= 2.0f && passedTime.asSeconds() <= 4.0f) {
				logoColor.a += deltaTime.asSeconds() * randomNumber(1500, 4000);
				logoSpr.setColor(logoColor);

				if (passedTime.asSeconds() >= 2.8f && passedTime.asSeconds() <= 3.6f) {
					logoColor.a = 0;
					logoSpr.setColor(logoColor);
				}

			} else {
				logoSpr.setColor(sf::Color::Transparent);
			}
			
			if (passedTime.asSeconds() >= 6.0f) {
				state = State::Load;
			}
		} else if (state == State::Load) {
			loadTerminal.update();
			if (passedTime.asSeconds() >= 9.0f && !loadingDone) {
				if (logCounter == bootlogs.size()) {
					loadingDone = true;
					logDoneTime = passedTime.asSeconds();
				}
				if (passedTime.asSeconds() > delayTime) {
					loadTerminal.output(bootlogs[logCounter] + '\n');
					logCounter++;
					if (logCounter <= 5) {
						delayTime += 0.01f;
						// TODO: reduce delayTime increment to 1.8f
					} else {
						delayTime += (((float)randomNumber(10, 10)) / 1000.0f);
						// TODO: reduce delayTime increment to 30, 70
					}
				}
			} else if ((passedTime.asSeconds() > (logDoneTime + 3.0f)) && passedTime.asSeconds() > 9.0f) {
				state = State::Login;
				logDoneTime = passedTime.asSeconds();
			}

		} else if (state == State::Login) {

			if ((passedTime.asSeconds() >= logDoneTime + 0.5f) && !loggedLoginMenu) {
				loginTerminal.output("\n[1] Log in\n[2] Sign up\n\n# ");
				loggedLoginMenu = true;
			}
			loginTerminal.update();

		} else if (state == State::Game) {
			terminal->update();
			helpTerminal.update();
		}

		window.clear();
		// Draw
		if (state == State::Splash) {
			window.draw(logoSpr);
		} else if (state == State::Load) {
			loadTerminal.render();
		} else if (state == State::Login) {
			loginTerminal.render();
		} else if (state == State::Game) {
			terminal->render();
			helpTerminal.render();
		}
		window.display();
	}

	delete terminal;
}
