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

	sf::Color m_colorBackground; 

	sf::Texture m_backgroundTexture;
	sf::Texture m_midgroundTexture;
	sf::Texture m_foregroundTexture;

	sf::Sprite m_backgroundSprite;
	sf::Sprite m_midgroundSprite;
	sf::Sprite m_foregroundSprite;

	float m_backgroundSpeed;
	float m_midgroundSpeed;
	float m_foregroundSpeed;

	bool m_jump = false;
	int m_framesForJump = 0;
	int sc = 0;
	void init(const std::string& levelPath);
	std::shared_ptr<Entity> searchIntersectBlock(Vec2 positionForSearch);
	void checkConditionsForBlock(std::shared_ptr<Entity> entity, CTransform& playerTransform);
	bool isPlayerRun(CTransform playerTransform);
	
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
	Scene_Level(GameEngine* gameEngine, const std::string levelPath, std::string levelName);
	void update();

	Vec2 gridToMidPixel(float gridx, float gridY, std::shared_ptr<Entity> entity);
	void loadLevel(const std::string& filename);
	void spawnPlayer();
	void spawnBullet(std::shared_ptr<Entity> entity);
	void onEnd();

};

