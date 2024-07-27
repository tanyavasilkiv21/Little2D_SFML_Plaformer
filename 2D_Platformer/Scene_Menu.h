#pragma once
#include "Scene.h"
class Scene_Menu : public Scene
{
private:
	int m_selectedItemIndex = 0;
	sf::Font m_font;
	const std::string m_menuOptions[3] = { "LEVEL 1", "LEVEL 2", "LEVEL 3" };
	sf::Text m_title;
	sf::Text m_describe;
	sf::Text m_menu[3];
public:
	Scene_Menu(GameEngine* gameEngine);
	void update();
	void sDoAction(const Action& action);
	void sRender();
	void onEnd();
	void doAction();
};

