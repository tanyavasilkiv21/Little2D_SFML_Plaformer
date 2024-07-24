#pragma once
#include "Scene.h"
class Scene_Menu /*: public Scene*/
{
public:
	Scene_Menu();
	void update();
	void sDoAction(const Action& action);
	void sRender();
	void onEnd();

};

