#include "GameEngine.h"
#include "Scene_Menu.h"
#include <iostream>
#include "ProfileTimer.hpp"

void GameEngine::update()
{
    if (currentScene()) 
    {

        {
            PROFILING_SCOPE("Update");
            currentScene()->update();
        }
        sUserInput();
    }
}

GameEngine::GameEngine(const std::string& path)
{
    PROFILING_FUNCTION();
    init(path);
}

void GameEngine::init(const std::string& path)
{
    PROFILING_FUNCTION();

    {
        PROFILING_SCOPE("Assets");
        m_assets.loadFromFile(path);
    }
    {
        PROFILING_SCOPE("SFML WINDOW");
        m_window.create(sf::VideoMode(1280, 768), "Meow-Knight!");
    }
    m_window.setFramerateLimit(60);
    changeScene("Scene_Menu", std::make_shared<Scene_Menu>(this));
}

std::shared_ptr<Scene>& GameEngine::currentScene()
{
    return m_sceneMap[m_currentScene];
}

void GameEngine::changeScene(std::string sceneName, std::shared_ptr<Scene> scene, bool endCurrentScene)
{
    if (endCurrentScene && m_currentScene == sceneName)
    {
        auto currentSceneIt = m_sceneMap.find(m_currentScene);
        if (currentSceneIt != m_sceneMap.end() && currentSceneIt->second)
        {
            currentSceneIt->second->onEnd();
            currentSceneIt->second.reset(); 
            m_sceneMap.erase(currentSceneIt);  
        }
    }

    m_sceneMap[sceneName] = scene;
    m_currentScene = sceneName;

}
void GameEngine::quit()
{
    m_window.close();
    exit(0);
}

void GameEngine::run()
{
    while (isRunning())
    {
        update();
    }
}

sf::RenderWindow& GameEngine::window()
{
    return m_window;
}

const Assets& GameEngine::assets() const
{
    return m_assets;
}

bool GameEngine::isRunning()
{
    return (m_running && m_window.isOpen());
}

void GameEngine::sUserInput()
{
    sf::Event event;
    while (m_window.pollEvent(event))
    {
        if (event.type == sf::Event::Closed)
        {
            quit();
        }
        if (event.type == sf::Event::KeyPressed)
        {
            if (event.key.code == sf::Keyboard::X)
            {
                std::cout << "screenshot saved to " << "test.png" << std::endl;
                sf::Texture texture;

                texture.create(m_window.getSize().x, m_window.getSize().y);
                texture.update(m_window);

                if (texture.copyToImage().saveToFile("test.png"))
                {
                    std::cout << "screenshot saved to " << "test.png" << std::endl;
                }    
            }
        }
        if (event.type == sf::Event::KeyPressed || event.type == sf::Event::KeyReleased)
        {
            // if the current scene does not have an action associated with this key, skip the event
            if (currentScene()->getActionMap().find(event.key.code) == currentScene()->getActionMap().end())
            {
                continue;
            }

            // determine start or end action by whether it was key pres or release
            const std::string actionType = (event.type == sf::Event::KeyPressed) ? "START" : "END";

            // look up the action and send the action to the scene
            currentScene()->doAction(Action(currentScene()->getActionMap().at(event.key.code), actionType));
        }
    }
}
