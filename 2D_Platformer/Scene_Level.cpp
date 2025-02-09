#include "Scene_Level.h"
#include "GameEngine.h"
#include "Physics.h"
#include "Scene_Menu.h"
#include "Scene_GameOver.h"

#include <iostream>
#include <math.h>
#include <algorithm>
#include <fstream>
#include <sstream>
#include "ProfileTimer.hpp"

Scene_Level::Scene_Level(GameEngine* gameEngine, const std::string levelPath, std::string levelName)
	:Scene(gameEngine), m_levelPath(levelPath), m_levelName(levelName)
{
	std::cout << "Level Path: " << levelPath << std::endl;
	std::cout << "Level Name: " << levelName << std::endl;
	init(m_levelPath);
}

void Scene_Level::init(const std::string& levelPath)
{
	registerAction(sf::Keyboard::P, "PAUSE");
	registerAction(sf::Keyboard::Escape, "QUIT");
	registerAction(sf::Keyboard::T, "TOGGLE_TEXTURE");
	registerAction(sf::Keyboard::C, "TOGGLE_COLLISION");
	registerAction(sf::Keyboard::G, "TOGGLE_GRID");
	registerAction(sf::Keyboard::W, "UP");
	registerAction(sf::Keyboard::A, "LEFT");
	registerAction(sf::Keyboard::D, "RIGHT");
	registerAction(sf::Keyboard::R, "RESTART");
	registerAction(sf::Keyboard::E, "SHOOT");

	m_gridText.setCharacterSize(12);
	m_gridText.setFont(m_game->assets().getFont("georgia"));

	m_scoreText.setCharacterSize(22);
	m_scoreText.setFont(m_game->assets().getFont("georgia"));
	m_scoreText.setPosition(sf::Vector2f(0, 0));
	m_scoreText.setString("Score: " + std::to_string(sc));
	loadLevel(levelPath);
}

void Scene_Level::onEnd()
{
	m_player.reset();
    m_actionMap.clear();
    m_entityManager.clear();

    // Clear textures and sprites if needed
    m_backgroundTexture = sf::Texture();
    m_midgroundTexture = sf::Texture();
    m_foregroundTexture = sf::Texture();
    m_backgroundSprite = sf::Sprite();
    m_midgroundSprite = sf::Sprite();
    m_foregroundSprite = sf::Sprite();
}

void Scene_Level::sAnimation()
{
	if (m_player->hasComponent <CState>())
	{
		auto& playerState = m_player->getComponent<CState>();
		auto& playerAnimationSprite = m_player->getComponent<CAnimation>().animation.getSprite();

		if (playerState.state == stateType::AIR)
		{
			playerAnimationSprite.setTexture(m_game->assets().getTexture("player_jump"));
		}
		if (playerState.state == stateType::STAND)
		{
			playerAnimationSprite.setTexture(m_game->assets().getTexture("player"));
		}
		if (playerState.state == stateType::RUN)
		{
			playerAnimationSprite.setTexture(m_game->assets().getTexture("player_run"));
		}
	}
	
	for (auto& e : m_entityManager.getEntities())
	{
		if (e->hasComponent<CAnimation>())
		{
			auto& Anim = e->getComponent<CAnimation>();
			if (Anim.animation.hasEnded() && Anim.repeat == false)
			{
				e->removeComponent<CAnimation>();
				e->destroy();
			}
		}
	}
}

void Scene_Level::sMovement()
{
	if (m_player->hasComponent<CInput>())
	{
		auto& playerInput = m_player->getComponent<CInput>();
		auto& playerTransform = m_player->getComponent<CTransform>();
		playerTransform.pos.y += m_player->getComponent<CGravity>().gravity;
		if (playerInput.right == true)
		{
			playerTransform.pos.x += playerTransform.velocity.x;
			if (playerTransform.scale.x < 0)
			{
				playerTransform.scale.x *= -1;
			}
		}
		if (playerInput.left == true)
		{
			playerTransform.pos.x -= playerTransform.velocity.x;
			if (playerTransform.scale.x > 0)
			{
				playerTransform.scale.x *= -1;
			}
		}
	}
	for (auto& bullet : m_entityManager.getEntities("bullet"))
	{
		auto& bulletTransform = bullet->getComponent<CTransform>();
		bulletTransform.pos.x += bulletTransform.velocity.x;
	}
}

void Scene_Level::sEnemySpawner()
{
}

std::shared_ptr<Entity> Scene_Level::searchIntersectBlock(Vec2 positionForSearch)
{
	for (auto e : m_entityManager.getEntities())
	{
		auto& eTransform = e->getComponent<CTransform>();
		auto eAnimationName = e->getComponent<CAnimation>().animation.getName();
		
		if (e->hasComponent<CBoundingBox>() && e != m_player)
		{
			if (eTransform.pos == positionForSearch)
			{
				return e;
			}
		}
	}

	return nullptr;
}

void Scene_Level::checkConditionsForBlock(std::shared_ptr<Entity> entity, CTransform& playerTransform)
{
	auto eAnimationName = entity->getComponent<CAnimation>().animation.getName();

	if (eAnimationName == "flag")
	{
		std::shared_ptr<Scene> newScene = std::make_shared<Scene_GameOver>(m_game);
		m_game->changeScene("Scene_GameOver", newScene);
	}
	if (eAnimationName == "coin")
	{
		entity->destroy();
		sc+= 100;
		m_scoreText.setString("Score: " + std::to_string(sc));
		playerTransform.pos = playerTransform.prevPos;

	}
	if (eAnimationName == "water" || eAnimationName == "lava")
	{
		std::shared_ptr<Scene> newScene = std::make_shared<Scene_Level>(m_game, m_levelPath, m_levelName);
		m_hasEnded = true;
		m_game->changeScene(m_levelName, newScene);
	}
}

bool Scene_Level::isPlayerRun(CTransform playerTransform)
{
	return playerTransform.pos.x != playerTransform.prevPos.x
		&& (playerTransform.pos.y == playerTransform.prevPos.y);
}

void Scene_Level::sCollision()
{
	Physics physics;
	auto& playerTransform = m_player->getComponent<CTransform>();
	auto& playerState = m_player->getComponent<CState>();
	auto& playerInput = m_player->getComponent<CInput>();
	if (playerTransform.pos.y >= height() || playerTransform.pos.y <= 0)
	{
		playerTransform.pos = Vec2(gridToMidPixel(m_playerConfig.X, m_playerConfig.Y, m_player));
	}
	if ((playerTransform.pos.x - 32 <= 0))
	{
		playerTransform.pos.x += (playerTransform.velocity.x );
	}

	auto downBlockPosition = Vec2(((int(playerTransform.pos.x) / int(m_gridSize.x)) * m_gridSize.x) + 32,
		(((int(playerTransform.pos.y) / int(m_gridSize.y)) + 2) * m_gridSize.y) - 32);
	if (auto de = searchIntersectBlock(downBlockPosition))
	{
		Vec2 downOverlap = physics.GetOverlap(m_player, de);
		if (downOverlap.y > 0)
		{
			playerTransform.pos.y = de->getComponent<CTransform>().pos.y - m_gridSize.y;
		}
		checkConditionsForBlock(de, playerTransform);
		if (m_hasEnded)
		{
			return;
		}
		playerState.state = stateType::STAND;
		playerInput.canJump = true;
		if (playerTransform.pos.x != playerTransform.prevPos.x
			&& (playerTransform.pos.y == playerTransform.prevPos.y))
		{
			playerState.state = stateType::RUN;

		}
	}
	else
	{
		if (!playerInput.canJump)
		{
			playerState.state = stateType::AIR;
		}
		playerInput.canJump = false;
	}

	if (int(m_gridSize.x) * int(m_gridSize.x) > 0)
	{
		auto leftBlockPosition = Vec2(((int(playerTransform.pos.x) / int(m_gridSize.x)) * m_gridSize.x) - m_gridSize.x + 32,
			(((int(playerTransform.pos.y) / int(m_gridSize.y)) + 1) * m_gridSize.y) - 32);
		if (leftBlockPosition.x > 0)
		{
			if (auto le = searchIntersectBlock(leftBlockPosition))
			{
				Vec2 leftOverlap = physics.GetOverlap(m_player, le);
				if (leftOverlap.x > 0)
				{
					checkConditionsForBlock(le, playerTransform);
					if (m_hasEnded)
					{
						return;
					}
					playerTransform.pos.x += leftOverlap.x;

				}
			}
		}
	}
	
	auto rightBlockPosition = Vec2(((int(playerTransform.pos.x) / int(m_gridSize.x)) * m_gridSize.x) + m_gridSize.x + 32,
		(((int(playerTransform.pos.y) / int(m_gridSize.y)) + 1) * m_gridSize.y) - 32);
	if (auto re = searchIntersectBlock(rightBlockPosition))
	{
		Vec2 rightOverlap = physics.GetOverlap(m_player, re);

		if (rightOverlap.x > 0)
		{
			checkConditionsForBlock(re, playerTransform);
			if (m_hasEnded)
			{
				return;
			}
			playerTransform.pos.x -= rightOverlap.x;
		}
	}
	auto upBlockPosition = Vec2((int(playerTransform.pos.x) / int(m_gridSize.x) * m_gridSize.x) + 32,
		(int(playerTransform.pos.y) / int(m_gridSize.y) * m_gridSize.y) - (m_gridSize.y) + 32);
	if (auto ue = searchIntersectBlock(upBlockPosition))
	{
		Vec2 upOverlap = physics.GetOverlap(m_player, ue);
		if (upOverlap.y > 0)
		{
			checkConditionsForBlock(ue, playerTransform);
			if (m_hasEnded)
			{
				return;
			}
			playerTransform.pos.y += upOverlap.y;
		}

	}
	bool diag = false;
	auto upLeftBlockPosition = Vec2((int(playerTransform.pos.x) / int(m_gridSize.x) * m_gridSize.x) - m_gridSize.x + 32,
		(int(playerTransform.pos.y) / int(m_gridSize.y) * m_gridSize.y) - (m_gridSize.y) + 32);
	if (auto ule = searchIntersectBlock(upLeftBlockPosition))
	{
		Vec2 upLeftOverlap = physics.GetOverlap(m_player, ule);
		if (upLeftOverlap.y > 0 && upLeftOverlap.x > 0)
		{
			checkConditionsForBlock(ule, playerTransform);
			if (m_hasEnded)
			{
				return;
			}
			playerTransform.pos.x += upLeftOverlap.x;
			diag = true;
		}
	}
	if(!diag)
	{
		auto upRightBlockPosition = Vec2((int(playerTransform.pos.x) / int(m_gridSize.x) * m_gridSize.x) + m_gridSize.x + 32,
			(int(playerTransform.pos.y) / int(m_gridSize.y) * m_gridSize.y) - (m_gridSize.y) + 32);
		if (auto ure = searchIntersectBlock(upRightBlockPosition))
		{
			Vec2 upRightOverlap = physics.GetOverlap(m_player, ure);
			if (upRightOverlap.y > 0 && upRightOverlap.x > 0)
			{
				checkConditionsForBlock(ure, playerTransform);
				if (m_hasEnded)
				{
					return;
				}
				playerTransform.pos.x -= upRightOverlap.x;
				diag = true;
			}
		}
		if (!diag)
		{
			auto downRightBlockPosition = Vec2(((int(playerTransform.pos.x) / int(m_gridSize.x)) * m_gridSize.x) + m_gridSize.x + 32,
				(((int(playerTransform.pos.y) / int(m_gridSize.y)) + 2) * m_gridSize.y) - 32);
			if (auto dre = searchIntersectBlock(downRightBlockPosition))
			{
				Vec2 downRightOverlap = physics.GetOverlap(m_player, dre);
				if (downRightOverlap.y > 0 && downRightOverlap.x > 0)
				{
					checkConditionsForBlock(dre, playerTransform);
					if (m_hasEnded)
					{
						return;
					}
					playerTransform.pos.x -= downRightOverlap.x;
					diag = true;
				}
			}
			if (!diag)
			{
				auto downLeftBlockPosition = Vec2(((int(playerTransform.pos.x) / int(m_gridSize.x)) * m_gridSize.x) - m_gridSize.x + 32,
					(((int(playerTransform.pos.y) / int(m_gridSize.y)) + 2) * m_gridSize.y) - 32);
				if (auto dle = searchIntersectBlock(downLeftBlockPosition))
				{
					Vec2 downLeftOverlap = physics.GetOverlap(m_player, dle);
					if (downLeftOverlap.y > 0 && downLeftOverlap.x > 0)
					{
						checkConditionsForBlock(dle, playerTransform);
						if (m_hasEnded)
						{
							return;
						}
						playerTransform.pos.x += downLeftOverlap.x;
					}
				}
			}
		}

	}
	for (auto& bullet : m_entityManager.getEntities("bullet"))
	{
		for (auto tile : m_entityManager.getEntities("tile"))
		{
			if (physics.GetOverlap(bullet, tile).y > 0 && physics.GetOverlap(bullet, tile).x > 0)
			{
				if (tile->getComponent<CAnimation>().animation.getName() == "rock")
				{
					tile->addComponent<CAnimation>(m_game->assets().getAnimation("explosion"), false);
					tile->removeComponent<CBoundingBox>();
					bullet->destroy();
				}
				else
				{
					bullet->destroy();
				}
			}
		}
	}
}

void Scene_Level::sRender()
{
	
	if (m_paused)
	{
		m_game->window().clear(sf::Color(100, 100, 255));
	}
	else
	{
		m_game->window().clear(m_colorBackground);
	}

	// Set the viewport of the window to be centered on the player if it's far enough right
	auto& pPos = m_player->getComponent<CTransform>().pos;
	m_player->getComponent<CTransform>().prevPos = m_player->getComponent<CTransform>().pos;
	float windowCenterX = std::max(m_game->window().getSize().x / 2., pPos.x);
	sf::View gameView = m_game->window().getView();
	gameView.setCenter(windowCenterX, gameView.getCenter().y);
	m_game->window().setView(gameView);

	// draw all Entity textures / animations
	if (m_drawTextures)
	{
		// Parallax
		sf::Vector2u backgroundSize = m_backgroundTexture.getSize();
		sf::Vector2u midgroundSize = m_midgroundTexture.getSize();
		sf::Vector2u foregroundSize = m_foregroundTexture.getSize();
		float backgroundOffsetX = fmod(windowCenterX * m_backgroundSpeed, backgroundSize.x);
		float midgroundOffsetX = fmod(windowCenterX * m_midgroundSpeed, midgroundSize.x);
		float foregroundOffsetX = fmod(windowCenterX * m_foregroundSpeed, foregroundSize.x);
		// Draw background layers with horizontal looping
		for (float x = -backgroundOffsetX; x < m_game->window().getSize().x; x += backgroundSize.x)
		{
			m_backgroundSprite.setPosition(x, 0);
			m_game->window().draw(m_backgroundSprite);
			m_backgroundSprite.setPosition(x + backgroundSize.x, 0);
			m_game->window().draw(m_backgroundSprite);
		}

		for (float x = -midgroundOffsetX; x < m_game->window().getSize().x; x += midgroundSize.x)
		{
			m_midgroundSprite.setPosition(x, 0);
			m_game->window().draw(m_midgroundSprite);
			m_midgroundSprite.setPosition(x + midgroundSize.x, 0);
			m_game->window().draw(m_midgroundSprite);
		}

		for (float x = -foregroundOffsetX; x < m_game->window().getSize().x; x += foregroundSize.x)
		{
			m_foregroundSprite.setPosition(x, 0);
			m_game->window().draw(m_foregroundSprite);
			m_foregroundSprite.setPosition(x + foregroundSize.x, 0);
			m_game->window().draw(m_foregroundSprite);
		}
		for (auto e : m_entityManager.getEntities())
		{
			if (m_player == e)
			{
				continue;
			}
			auto& transform = e->getComponent<CTransform>();

			if (e->hasComponent<CAnimation>())
			{
				auto& animation = e->getComponent<CAnimation>().animation;

				animation.getSprite().setRotation(transform.angle);
				animation.getSprite().setPosition(transform.pos.x, transform.pos.y);
				animation.getSprite().setScale(transform.scale.x, transform.scale.y);
				animation.update();
				m_game->window().draw(animation.getSprite());
			}
		}
		auto& transform = m_player->getComponent<CTransform>();

		if (m_player->hasComponent<CAnimation>())
		{
			auto& animation = m_player->getComponent<CAnimation>().animation;

			animation.getSprite().setRotation(transform.angle);
			animation.getSprite().setPosition(transform.pos.x, transform.pos.y);
			animation.getSprite().setScale(transform.scale.x, transform.scale.y);
			animation.update();
			m_game->window().draw(animation.getSprite());
		}

		
	}
	if (m_drawCollision)
	{
		for (auto e : m_entityManager.getEntities())
		{
			if (e->hasComponent<CBoundingBox>())
			{
				auto& box = e->getComponent<CBoundingBox>();
				auto& transform = e->getComponent<CTransform>();
				sf::RectangleShape rect;

				rect.setSize(sf::Vector2f(box.size.x - 1, box.size.y - 1));
				rect.setOrigin(sf::Vector2f(box.halfsize.x, box.halfsize.y));
				rect.setPosition(transform.pos.x, transform.pos.y);
				rect.setFillColor(sf::Color(0, 0, 0, 0));
				rect.setOutlineColor(sf::Color(255, 255, 255, 255));
				rect.setOutlineThickness(1);
				m_game->window().draw(rect);
			}

		}
	}
	if (m_drawGrid)
	{
		float leftX = m_game->window().getView().getCenter().x - width() / 2;
		float rightX = leftX + width() + m_gridSize.x;
		float nextGridX = leftX - ((int)leftX % (int)m_gridSize.x);

		for (float x = nextGridX; x < rightX; x += m_gridSize.x)
		{
			drawLine(Vec2(x, 0), Vec2(x, height()));
		}

		for (float y = 0; y < height(); y += m_gridSize.y)
		{
			drawLine(Vec2(leftX, height() - y), Vec2(rightX, height() - y));

			for (float x = nextGridX; x < rightX; x += m_gridSize.x)
			{
				std::string xCell = std::to_string((int)x / (int)m_gridSize.x);
				std::string yCell = std::to_string((int)y / (int)m_gridSize.y);
				std::string xyCell = "(" + xCell + "," + yCell + ")";
				m_gridText.setString(xyCell);
				m_gridText.setPosition(x + 3, height() - y - m_gridSize.y + 2);
				m_game->window().draw(m_gridText);
			}
		}
	}
	m_game->window().setView(m_game->window().getDefaultView());
	m_game->window().draw(m_scoreText);
	m_game->window().display();
}

void Scene_Level::sDoAction(const Action& action)
{
	auto& playerGravity = m_player->getComponent<CGravity>();
	auto& playerInput = m_player->getComponent<CInput>();
	if (action.type() == "START")
	{
		if (action.name() == "TOGGLE_TEXTURE")
		{
			m_drawTextures = !m_drawTextures;
		}
		else if (action.name() == "TOGGLE_COLLISION")
		{
			m_drawCollision = !m_drawCollision;
		}
		else if (action.name() == "TOGGLE_GRID")
		{
			m_drawGrid = !m_drawGrid;
		}
		else if (action.name() == "PAUSE")
		{
			setPaused(!m_paused);
		}
		else if (action.name() == "QUIT")
		{
			std::shared_ptr<Scene> newScene = std::make_shared<Scene_Menu>(m_game);
			m_game->changeScene("Scene_Menu", newScene, true);
		}
		else if (action.name() == "RESTART")
		{
			m_hasEnded = true;
			std::shared_ptr<Scene> newScene = std::make_shared<Scene_Level>(m_game, m_levelPath, m_levelName);
			m_game->changeScene(m_levelName, newScene, true);
		}
		if (m_framesForJump >= m_playerConfig.JUMP)
		{
			playerGravity.gravity = m_playerConfig.GRAVITY;

		}
		if (action.name() == "UP" && m_framesForJump <= m_playerConfig.JUMP)
		{
			if (playerGravity.gravity > 0 && playerInput.canJump)
			{
				playerInput.canJump = false;
				playerGravity.gravity *= -1;

			}
			else if (!playerInput.canJump && m_framesForJump >= m_playerConfig.JUMP)
			{
				playerGravity.gravity = m_playerConfig.GRAVITY;
			}
		}
		if (action.name() == "LEFT")
		{
			if (!playerInput.canJump)
			{
				playerGravity.gravity = m_playerConfig.GRAVITY;
			}
			playerInput.left = true;
		}
		if (action.name() == "RIGHT")
		{
			if (!playerInput.canJump)
			{
				playerGravity.gravity = m_playerConfig.GRAVITY;
			}
			playerInput.right = true;
		}
		if (action.name() == "SHOOT")
		{
			if (playerInput.canShoot)
			{
				spawnBullet(m_player);
				playerInput.canShoot = false;
			}
		}
		if (!playerInput.canJump)
		{
			m_framesForJump++;
		}
	}

	else if (action.type() == "END")
	{
		if (action.name() == "UP")
		{
			if (playerGravity.gravity < 0)
			{
				playerGravity.gravity = m_playerConfig.GRAVITY;
			}
			m_framesForJump = 0;
		}
		else if (action.name() == "LEFT")
		{
			playerInput.left = false;
		}
		else if (action.name() == "RIGHT")
		{
			playerInput.right = false;
		}
		if (action.name() == "SHOOT")
		{
			playerInput.canShoot = true;
		}
	}
}

void Scene_Level::sDebug()
{
}

void Scene_Level::sLifespan()
{
	for (auto& e : m_entityManager.getEntities())
	{
		if (!e->hasComponent<CLifespan>())
		{
			continue;
		}
		auto& lifespanE = e->getComponent<CLifespan>();
		if (lifespanE.remaining > 0)
		{
			lifespanE.remaining -= 1;
			float lifePercentage = float(lifespanE.remaining) / lifespanE.lifespan;

		}
		if (lifespanE.remaining == 0)
		{
			e->destroy();
		}
	}
}

Vec2 Scene_Level::gridToMidPixel(float gridX, float gridY, std::shared_ptr<Entity> entity)
{
	auto size = entity->getComponent<CAnimation>().animation.getSprite().getOrigin();
	auto qOfCellsY = m_game->window().getSize().y / m_gridSize.y;
	auto scale = entity->getComponent <CTransform>().scale;
	return Vec2((gridX * m_gridSize.x) + (size.x * scale.x), ((qOfCellsY - gridY) * m_gridSize.y) - (size.y * scale.y));
}

void Scene_Level::loadLevel(const std::string& filename)
{
	// Reset the entity manager every time we load a level
	m_entityManager = EntityManager();

	std::ifstream fileIn(filename);
	if (!fileIn.is_open())
	{
		return;
	}

	std::string line;
	while (fileIn.peek() != EOF && std::getline(fileIn, line))
	{
		std::istringstream iss(line);
		std::string pieceOfLevelType;

		if (!(iss >> pieceOfLevelType))
		{
			continue;
		}

		if (pieceOfLevelType == "flag")
		{
			std::string nameAnim;
			int x, y;

			if (!(iss >> nameAnim >> x >> y))
			{
				continue;
			}

			auto dec = m_entityManager.addEntity(pieceOfLevelType);
			dec->addComponent<CAnimation>(m_game->assets().getAnimation(nameAnim), true);
			dec->addComponent<CTransform>(Vec2(0, 0), Vec2(0, 0), Vec2(1, 1), 1);
			dec->getComponent<CTransform>().pos = gridToMidPixel(x, y, dec);
			dec->addComponent<CBoundingBox>(Vec2(dec->getComponent<CAnimation>().animation.getSize().x * dec->getComponent<CTransform>().scale.x,
				dec->getComponent<CAnimation>().animation.getSize().y * dec->getComponent<CTransform>().scale.y));
		}
		else if (pieceOfLevelType == "tile")
		{
			std::string nameAnim;
			int x, y;

			if (!(iss >> nameAnim >> x >> y))
			{
				continue;
			}

			auto tile = m_entityManager.addEntity(pieceOfLevelType);
			tile->addComponent<CAnimation>(m_game->assets().getAnimation(nameAnim), true);
			tile->addComponent<CTransform>(Vec2(0, 0), Vec2(0, 0), Vec2(4, 4), 1);
			tile->getComponent<CTransform>().pos = gridToMidPixel(x, y, tile);
			tile->addComponent<CBoundingBox>(Vec2(64, 64));
		}
		else if (pieceOfLevelType == "dec")
		{
			std::string nameAnim;
			int x, y;

			if (!(iss >> nameAnim >> x >> y))
			{
				continue;
			}

			auto dec = m_entityManager.addEntity(pieceOfLevelType);
			dec->addComponent<CAnimation>(m_game->assets().getAnimation(nameAnim), true);
			dec->addComponent<CTransform>(Vec2(0, 0), Vec2(0, 0), Vec2(4, 4), 1);
			dec->getComponent<CTransform>().pos = gridToMidPixel(x, y, dec);
		}
		else if (pieceOfLevelType == "coin")
		{
			std::string nameAnim;
			int x, y;

			if (!(iss >> nameAnim >> x >> y))
			{
				continue;
			}

			auto dec = m_entityManager.addEntity(pieceOfLevelType);
			dec->addComponent<CAnimation>(m_game->assets().getAnimation(nameAnim), true);
			dec->addComponent<CTransform>(Vec2(0, 0), Vec2(0, 0), Vec2(4, 4), 1);
			dec->getComponent<CTransform>().pos = gridToMidPixel(x, y, dec);
			dec->addComponent<CBoundingBox>(Vec2(dec->getComponent<CAnimation>().animation.getSize().x * 4,
				dec->getComponent<CAnimation>().animation.getSize().y * 4));
		}
		else if (pieceOfLevelType == "player")
		{
			if (!(iss >> m_playerConfig.X >> m_playerConfig.Y >> m_playerConfig.BX >> m_playerConfig.BY))
			{
				continue;
			}
			if (!(iss >> m_playerConfig.SPEED >> m_playerConfig.MAXSPEED >> m_playerConfig.JUMP >> m_playerConfig.GRAVITY))
			{
				continue;
			}
		}
		else if (pieceOfLevelType == "background")
		{
			std::string textureFileName;
			float speed;

			if (!(iss >> textureFileName >> speed))
			{
				continue;
			}

			m_backgroundSpeed = speed;
			if (!m_backgroundTexture.loadFromFile(textureFileName))
			{
				continue;
			}
			m_backgroundSprite.setTexture(m_backgroundTexture);
		}
		else if (pieceOfLevelType == "midground")
		{
			std::string textureFileName;
			float speed;

			if (!(iss >> textureFileName >> speed))
			{
				continue;
			}

			m_midgroundSpeed = speed;
			if (!m_midgroundTexture.loadFromFile(textureFileName))
			{
				continue;
			}
			m_midgroundSprite.setTexture(m_midgroundTexture);
		}
		else if (pieceOfLevelType == "foreground")
		{
			std::string textureFileName;
			float speed;

			if (!(iss >> textureFileName >> speed))
			{
				continue;
			}

			m_foregroundSpeed = speed;
			if (!m_foregroundTexture.loadFromFile(textureFileName))
			{
				continue;
			}
			m_foregroundSprite.setTexture(m_foregroundTexture);
		}
		else if (pieceOfLevelType == "color")
		{
			int r = 0, g = 0, b = 0;
			if (!(iss >> r >> g >> b))
			{
				continue;
			}
			m_colorBackground = sf::Color(r, g, b);
		}
	}
	fileIn.close();
	spawnPlayer();
}
void Scene_Level::spawnPlayer()
{
	m_player = m_entityManager.addEntity("player");
	m_player->addComponent<CAnimation>(m_game->assets().getAnimation("idle"), true);
	m_player->addComponent<CTransform>(Vec2(0,0),
		Vec2(m_playerConfig.SPEED, m_playerConfig.SPEED), Vec2(4, 4), 1);
	m_player->getComponent< CTransform>().pos = gridToMidPixel(m_playerConfig.X, m_playerConfig.Y, m_player);
	m_player->addComponent<CBoundingBox>(Vec2(m_playerConfig.BX, m_playerConfig.BY));
	m_player->addComponent<CInput>();
	m_player->addComponent<CGravity>(m_playerConfig.GRAVITY);
	m_player->addComponent<CState>(stateType::STAND);
}

void Scene_Level::spawnBullet(std::shared_ptr<Entity> entity)
{
	auto pPos = m_player->getComponent<CTransform>().pos;
	auto bullet = m_entityManager.addEntity("bullet");
	bullet->addComponent<CAnimation>(m_game->assets().getAnimation("star"), true);
	int direction = 1;
	if (entity->getComponent<CTransform>().scale.x < 0)
	{
		direction *= -1;
	}
	bullet->addComponent<CTransform>(Vec2(pPos.x + (m_gridSize.x * direction), pPos.y),
		Vec2(m_playerConfig.SPEED * direction, m_playerConfig.SPEED * direction), Vec2(direction, 1), 1);
	bullet->addComponent<CBoundingBox>(Vec2(16, 16));
	bullet->addComponent<CLifespan>(50);
}

void Scene_Level::update()
{
	
	if (!m_hasEnded)
	{
		m_entityManager.update();

		{
			PROFILING_SCOPE("Movement");
			sMovement();
		}
		{
			PROFILING_SCOPE("Collision");
			sCollision();
		}
		if (m_hasEnded)
		{
			return;
		}
		sLifespan();
		sAnimation();
		{
			PROFILING_SCOPE("Render");
			sRender();
		}
	}
}