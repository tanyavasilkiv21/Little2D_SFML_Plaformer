#include "Scene_GameOver.h"
#include "GameEngine.h"
#include "Scene_Menu.h"

#include<iostream>

Scene_GameOver::Scene_GameOver(GameEngine* gameEngine)
    : Scene(gameEngine)
{
    sf::View view = m_game->window().getDefaultView();
    m_game->window().setView(view);

    m_font = m_game->assets().getFont("georgia");
    m_title.setFont(m_font);
    m_title.setString("You win!");
    m_title.setCharacterSize(50);
    m_title.setFillColor(sf::Color(217, 80, 117));
    m_title.setPosition(sf::Vector2f(30, 200));

    for (int i = 0; i < 2; ++i)
    {
        m_menu[i].setFont(m_font);
        m_menu[i].setFillColor(sf::Color(228, 77, 124));
        m_menu[i].setString(m_menuOptions[i]);
        m_menu[i].setPosition(sf::Vector2f(30, 350 + (i * 50)));
    }
    m_menu[m_selectedItemIndex].setFillColor(sf::Color(220, 20, 60));

    registerAction(sf::Keyboard::W, "UP");
    registerAction(sf::Keyboard::S, "DOWN");
    registerAction(sf::Keyboard::F, "PLAY");
}

void Scene_GameOver::onEnd()
{
    m_game->quit();
}

void Scene_GameOver::update()
{
    sf::View view = m_game->window().getDefaultView();
    m_game->window().setView(view);

    sRender();
}

void Scene_GameOver::sDoAction(const Action& action)
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
            if (m_selectedItemIndex + 1 < 2) {
                m_menu[m_selectedItemIndex].setFillColor(sf::Color(228, 77, 124));
                m_selectedItemIndex++;
                m_menu[m_selectedItemIndex].setFillColor(sf::Color(220, 20, 60));
            }
        }
        if (action.name() == "PLAY")
        {
            std::string name;
            switch (m_selectedItemIndex)
            {
            case 0:
                name = "Scene_Menu";
                m_game->changeScene(name, std::make_shared<Scene_Menu>(m_game));
                break;
            case 1:
                onEnd();
                break;
            }
        }
    }
}

void Scene_GameOver::sRender()
{
    m_game->window().clear(sf::Color(35, 17, 20));
    m_game->window().draw(m_title);
    m_game->window().draw(m_describe);
    for (int i = 0; i < 2; ++i) {
        m_game->window().draw(m_menu[i]);
    }
    m_game->window().display();
}

