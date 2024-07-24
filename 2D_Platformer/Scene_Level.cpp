#include "Scene_Level.h"
#include <iostream>
#include <math.h>
#include <algorithm>
#include "GameEngine.h"
#include "Physics.h"

Scene_Level::Scene_Level(GameEngine* gameEngine, const std::string& levelPath)
	:Scene(gameEngine), m_levelPath(levelPath)
{
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
	// TODO: Register all other gameplay Actions

	m_gridText.setCharacterSize(12);
	m_gridText.setFont(m_game->assets().getFont("georgia"));

	loadLevel(levelPath);
}

void Scene_Level::onEnd()
{
}

void Scene_Level::sAnimation()
{
	auto& playerState = m_player->getComponent<CState>();
	if (playerState.state == "jump")
	{
		m_player->getComponent<CAnimation>().animation.getSprite().setTexture(m_game->assets().getTexture("player_jump"));
	}
	if (playerState.state == "idle")
	{
		m_player->getComponent<CAnimation>().animation.getSprite().setTexture(m_game->assets().getTexture("player"));
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

		if (playerInput.up == true)
		{
		}
	}
	
	// TODO: Implement the maxmimum player speed in both X and Y directions 
}

void Scene_Level::sEnemySpawner()
{
}

void Scene_Level::sCollision()
{
//REMEMBER: SFML's (0,0) position is on the TOP-LEFT corner
//	This means jumping will have a negative y - component
//	and gravity will have a positive y - component
//	Also, something BELOW something else will have a y value GREATER than it
//	Also, something ABOVE something else will have a y value LESS than it
//	TODO :Implement Physics::GetOverlap() function, use it inside this function +

//	TODO :  Implement bullet / tile collisions
//			Destroy the tile if it has a Brick animation
//	TODO :  Implement player / tile collisions and resolutions		   +
//			Update the CState component of the player to store whether +
//			it is currently on the ground or in the air.This will be   +
//			used by the Animation system							   +
//	TODO :  Check to see if the player has fallen down a hole(y > height()) +
//	TODO :  Don't let the player walk off the left side of the map +

	Physics physics;
	auto& playerTransform = m_player->getComponent<CTransform>();
	if (playerTransform.pos.y >= height())
	{
		playerTransform.pos = Vec2(gridToMidPixel(0, 6, m_player));
	}
	if ((playerTransform.pos.x - 16 - playerTransform.velocity.x <= 0))
	{
		playerTransform.pos.x += playerTransform.velocity.x;
	}
	for (auto e : m_entityManager.getEntities())
	{
		if (e->hasComponent<CBoundingBox>() && e != m_player)
		{
			if (physics.GetOverlap(m_player, e).y > 0 && physics.GetOverlap(m_player, e).x > 0)
			{
				
				if (playerTransform.pos.y < playerTransform.prevPos.y)
				{
					playerTransform.pos.y += physics.GetOverlap(m_player, e).y;
				}
				else
				{
					playerTransform.pos.y -= physics.GetOverlap(m_player, e).y;
				}
				
			}
			if (physics.GetOverlap(m_player, e).y == 0 || physics.GetOverlap(m_player, e).x == 0)
			{
				m_player->getComponent<CState>().state = "idle";
			}
			else
			{
				m_player->getComponent<CState>().state = "jump";
			}
		}
	}
}

void Scene_Level::sRender()
{
	// color the background darker so you know that the game is paused
	if (m_paused) 
	{
		m_game->window().clear(sf::Color(100, 100, 255)); 
	}
	else 
	{ 
		m_game->window().clear(sf::Color(30, 50, 150));
	}
	// set the viewport of the window to be centered on the player if it's far enough right
	auto& pPos = m_player->getComponent<CTransform>().pos;
	m_player->getComponent<CTransform>().prevPos = pPos;
	float windowCenterX = std::max(m_game->window().getSize().x / 2., pPos.x);
	sf::View view = m_game->window().getView();

	view.setCenter(windowCenterX, m_game->window().getSize().y - view.getCenter().y);
	m_game->window().setView(view);

	// draw all Entity textures / animations
	if (m_drawTextures)
	{
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
				auto &box = e->getComponent<CBoundingBox>();
				auto &transform = e->getComponent<CTransform>();
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
	m_game->window().display();
}

void Scene_Level::sDoAction(const Action& action)
{
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
			onEnd(); 
		}
		else if (action.name() == "UP") 
		{
			
		}
		else if (action.name() == "LEFT") 
		{
			m_player->getComponent<CInput>().left = true;
		}
		else if (action.name() == "RIGHT") 
		{
			m_player->getComponent<CInput>().right = true;
		}
		
	}

	else if (action.type() == "END")
	{
		if (action.name() == "UP")
		{
			
		}
		else if (action.name() == "LEFT")
		{
			m_player->getComponent<CInput>().left = false;
		}
		else if (action.name() == "RIGHT")
		{
			m_player->getComponent<CInput>().right = false;
		}
	}
	std::cout << m_framesForJump << std::endl;
}

void Scene_Level::sDebug()
{
}

void Scene_Level::sLifespan()
{
}

Vec2 Scene_Level::gridToMidPixel(float gridX, float gridY, std::shared_ptr<Entity> entity)
{
	auto size = entity->getComponent<CAnimation>().animation.getSprite().getOrigin();
	auto qOfCellsY = m_game->window().getSize().y / m_gridSize.y;
	return Vec2((gridX * m_gridSize.x) + (size.x * 4), ((qOfCellsY - gridY) * m_gridSize.y) - (size.y * 4));
}

void Scene_Level::loadLevel(const std::string& filename)
{
	// reset the entity manager every time we load a level
	m_entityManager = EntityManager();

	// TODO: read in the level file and add the appropriate entities
	// use the PlayerConfig struct m_playerConfig to store player properties
	// this struct is defined at the top of Scene_Play.h
	// NOTE: all of the code below is sample code which shows you how to
	// set up and use entities with the new syntax, it should be removed

	spawnPlayer();
	for (int i = 0; i <= 8; i++)
	{
		auto brick = m_entityManager.addEntity("tile");

		brick->addComponent<CAnimation>(m_game->assets().getAnimation("tile_one"), true);
		brick->addComponent<CTransform>(gridToMidPixel(i,4, brick), Vec2(0, 0), Vec2(4, 4), 1);
		brick->addComponent<CBoundingBox>(Vec2(64, 64));
		if (brick->getComponent<CAnimation>().animation.getName() == "tile_one")
		{
			std::cout << "This could be a good way of identifying if a tile is a brick!\n";
		}
	}
	auto brick = m_entityManager.addEntity("tile");

	brick->addComponent<CAnimation>(m_game->assets().getAnimation("tile_one"), true);
	brick->addComponent<CTransform>(gridToMidPixel(8, 6, brick), Vec2(0, 0), Vec2(4, 4), 1);
	brick->addComponent<CBoundingBox>(Vec2(64, 64));
	auto tree = m_entityManager.addEntity("tree");
	tree->addComponent<CAnimation>(m_game->assets().getAnimation("tree_one"), true);
	tree->addComponent<CTransform>(gridToMidPixel(4, 5, tree), Vec2(0, 0), Vec2(4, 4), 1);
	
}

void Scene_Level::spawnPlayer()
{
	// here is a sample player entity which you can use to construct other entities
	m_player = m_entityManager.addEntity("player");
	m_player->addComponent<CAnimation>(m_game->assets().getAnimation("idle"), true);
	m_player->addComponent<CTransform>(gridToMidPixel(0, 6, m_player), Vec2(5, 5), Vec2(4, 4), 1);
	m_player->addComponent<CBoundingBox>(Vec2(64, 64));
	m_player->addComponent<CInput>();
	m_player->addComponent<CGravity>(5);
	m_player->addComponent<CState>("idle");
	// TODO: be sure to add the remaining components to the player
}

void Scene_Level::spawnBullet(std::shared_ptr<Entity> entity)
{
}

void Scene_Level::update()
{
	m_entityManager.update();
	// TODO: implement pause functionality

	sMovement();
	sLifespan();
	sCollision();
	sAnimation();
	sRender();
}
