#pragma once
#include "Scene.h"

class Scene_GameOver : public Scene
{
protected:
	int m_selectedItemIndex = 0;
	
	sf::Font m_font;
	sf::Text m_title;
	sf::Text m_describe;
	const std::string m_menuOptions[2] = { "MENU", "EXIT" };
	sf::Text m_menu[2];
public:
	Scene_GameOver(GameEngine* gameEngine);
	void update();
	void sDoAction(const Action& action);
	void sRender();
	void onEnd();
};

