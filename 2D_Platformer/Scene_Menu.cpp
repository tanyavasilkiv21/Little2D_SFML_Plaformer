#include "Scene_Menu.h"
#include "Scene.h"
#include "GameEngine.h"
#include "Scene_Level.h"
#include <iostream>

Scene_Menu::Scene_Menu(GameEngine* gameEngine)
	:Scene(gameEngine)
{
	m_font = m_game->assets().getFont("georgia");
	m_title.setFont(m_font);
	m_title.setString("Meow Knight!");
	m_title.setCharacterSize(50); 
	m_title.setFillColor(sf::Color(217, 80, 117)); 
	m_title.setPosition(sf::Vector2f(30, 200));
	m_describe.setFont(m_font);
	m_describe.setString("W: UP       D: DOWN       F: START LEVEL       ESC: QUIT GAME");
	m_describe.setCharacterSize(30);
	m_describe.setFillColor(sf::Color(217, 80, 117));
	m_describe.setPosition(sf::Vector2f(30, m_game->window().getSize().y - 100));
	for (int i = 0; i < 3; ++i)
	{
		m_menu[i].setFont(m_font);
		m_menu[i].setFillColor(sf::Color(228, 77, 124));
		m_menu[i].setString(m_menuOptions[i]);
		m_menu[i].setPosition(sf::Vector2f(30, 350+(i * 50)));
	}
	m_menu[m_selectedItemIndex].setFillColor(sf::Color(220, 20, 60));
	registerAction(sf::Keyboard::W, "UP");
	registerAction(sf::Keyboard::S, "DOWN");
	registerAction(sf::Keyboard::F, "PLAY");
	registerAction(sf::Keyboard::Escape, "QUIT");
}

void Scene_Menu::update()
{
	sRender();
}

void Scene_Menu::sDoAction(const Action& action)
{
	if (action.type() == "START")
	{
		if (action.name() == "UP")
		{
			if (m_selectedItemIndex - 1 >= 0) {
				m_menu[m_selectedItemIndex].setFillColor(sf::Color(228, 77, 124));
				m_selectedItemIndex--;
				m_menu[m_selectedItemIndex].setFillColor(sf::Color(220, 20, 60));
			}
		}
		if (action.name() == "DOWN")
		{
			if (m_selectedItemIndex + 1 < 3) {
				m_menu[m_selectedItemIndex].setFillColor(sf::Color(228, 77, 124));
				m_selectedItemIndex++;
				m_menu[m_selectedItemIndex].setFillColor(sf::Color(220, 20, 60));
			}
		}
		if (action.name() == "PLAY")
		{
			switch (m_selectedItemIndex)
			{
			case 0:
				m_game->changeScene("Level1", std::make_shared<Scene_Level>(m_game, "levels_config/level1.txt"));
				break;
			case 1:
				m_game->changeScene("Level2", std::make_shared<Scene_Level>(m_game, "levels_config/level2.txt"));
				break;
			case 2:
				m_game->changeScene("Level3", std::make_shared<Scene_Level>(m_game, "levels_config/level3.txt"));
				break;
			}
		}
		if (action.name() == "QUIT")
		{
			onEnd();
		}
	}
}

void Scene_Menu::sRender()
{
	m_game->window().clear(sf::Color(35, 17, 20));
	m_game->window().draw(m_title);
	m_game->window().draw(m_describe);
	for (int i = 0; i < 3; ++i) {
		m_game->window().draw(m_menu[i]);
	}
	m_game->window().display();
}

void Scene_Menu::onEnd()
{
	m_game->quit();
}
