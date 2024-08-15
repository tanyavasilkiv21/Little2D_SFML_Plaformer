#pragma once
#include "Scene.h"

class Scene_Level : public Scene
{
	struct PlayerConfig
	{
		float X, Y, BX, BY, SPEED, MAXSPEED, JUMP, GRAVITY;
		std::string WEAPON;
	};

protected:

	std::string m_levelPath;
	std::string m_levelName;
	std::shared_ptr<Entity> m_player;
	PlayerConfig m_playerConfig;
	bool m_drawTextures = true;
	bool m_drawCollision = false;
	bool m_drawGrid = false;
	Vec2 m_gridSize = { 64, 64 };
	sf::Text m_gridText;
	sf::Text m_scoreText;
	bool m_jump = false;
	int m_framesForJump = 0;
	int sc = 0;
	void init(const std::string& levelPath);
	std::shared_ptr<Entity> playerStaysOnBlock(CTransform playerTransform);
	Vec2 playerIntersectBlock(CTransform playerTransform);
	std::shared_ptr<Entity> searchIntersectBlock(Vec2 positionForSearch);
	bool isPlayerRun(CTransform playerTransform);
	void onEnd();
	//Systems
	void sAnimation();
	void sMovement();
	void sEnemySpawner();
	void sCollision();
	void sRender();
	void sDoAction(const Action& action);
	void sDebug();
	void sLifespan();
public:
	Scene_Level(GameEngine* gameEngine, const std::string& levelPath, std::string& levelName);
	void update();

	Vec2 gridToMidPixel(float gridx, float gridY, std::shared_ptr<Entity> entity);
	void loadLevel(const std::string& filename);
	void spawnPlayer();
	void spawnBullet(std::shared_ptr<Entity> entity);
};

