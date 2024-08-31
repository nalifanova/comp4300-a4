#include "SceneZelda.hpp"

#include <fstream>
#include <iostream>

#include "imgui.h"
#include "imgui-SFML.h"

#include "Components.hpp"
#include "GameEngine.hpp"
#include "Physics.hpp"
#include "SceneMenu.hpp"

SceneZelda::SceneZelda(GameEngine* game, std::string& levelPath) :
    Scene(game), m_levelPath(levelPath)
{
    init(m_levelPath);
}

void SceneZelda::update()
{
    m_entityManager.update();

    sDrag();
    if (!m_paused)
    {
        sAI();
        sMovement();
        sStatus();
        sCollision();
        sAnimation();
        sCamera();
        sGUI();
        m_currentFrame++;
    }
    sRender();
}

// protected
void SceneZelda::onEnd()
{
    // TODO
    // When the scene ends, change back to the MENU scene
    // Stop the music
    // Play the menu music
    m_game->changeScene("Menu", std::make_shared<SceneMenu>(m_game));
}

void SceneZelda::sDoAction(const Action& action)
{
    assert(player() != nullptr);
    auto& input = player()->get<CInput>();
    // TODO:
    // Implement all actions described for the game here
    // Only the setting of the player's input component variables should be set here
    // Do minimal logic in this function

    if (action.name() == "MOUSE_MOVE")
    {
        m_mousePos = action.pos();
    }

    if (action.type() == "START")
    {
        if (action.name() == "PAUSE") { setPaused(!m_paused); }
        else if (action.name() == "QUIT") { onEnd(); }
        else if (action.name() == "TOGGLE_FOLLOW") { m_follow = !m_follow; }
        else if (action.name() == "TOGGLE_TEXTURE") { m_drawTextures = !m_drawTextures; }
        else if (action.name() == "TOGGLE_COLLISION") { m_drawCollision = !m_drawCollision; }
        else if (action.name() == "TOGGLE_GRID") { m_drawGrid = !m_drawGrid; }

        else if (action.name() == "UP") { input.up = true; }
        else if (action.name() == "DOWN") { input.down = true; }
        else if (action.name() == "LEFT") { input.left = true; }
        else if (action.name() == "RIGHT") { input.right = true; }
        else if (action.name() == "ATTACK") { input.attack = true; }

        else if (action.name() == "LEFT_CLICK")
        {
            const Vec2 wPos = windowToWorld(m_mousePos);
            for (auto& e: m_entityManager.getEntities())
            {
                if (!e->has<CDraggable>()) { continue; }

                if (Physics::isInside(wPos, e))
                {
                    e->get<CDraggable>().dragging = !e->get<CDraggable>().dragging;
                }
            }
        }
    }
    else if (action.type() == "END")
    {
        if (action.name() == "UP") { input.up = false; }
        else if (action.name() == "DOWN") { input.down = false; }
        else if (action.name() == "LEFT") { input.left = false; }
        else if (action.name() == "RIGHT") { input.right = false; }
        else
            if (action.name() == "ATTACK") { input.attack = false; }
    }
}

void SceneZelda::sRender()
{
    m_game->window().clear(sf::Color(255, 192, 122));
    sf::RectangleShape tick({1.0f, 6.0f});
    tick.setFillColor(sf::Color::Black);

    // draw all Entity textures / animations
    if (m_drawTextures)
    {
        for (const auto& entity: m_entityManager.getEntities())
        {
            auto& transform = entity->get<CTransform>();
            sf::Color c = sf::Color::White;
            if (entity->has<CInvincibility>())
            {
                c = sf::Color(255, 255, 255, 128);
            }
            if (entity->has<CAnimation>())
            {
                auto& animation = entity->get<CAnimation>().animation;
                animation.getSprite().setRotation(transform.angle);
                animation.getSprite().setPosition(
                    transform.pos.x, transform.pos.y
                    );
                animation.getSprite().setScale(
                    transform.scale.x, transform.scale.y
                    );
                animation.getSprite().setColor(c);
                m_game->window().draw(animation.getSprite());
            }
        }

        for (const auto& entity: m_entityManager.getEntities())
        {
            auto& transform = entity->get<CTransform>();
            if (entity->has<CHealth>())
            {
                auto& h = entity->get<CHealth>();
                Vec2 size(64, 6);
                sf::RectangleShape rect({size.x, size.y});
                rect.setPosition(
                    transform.pos.x - 32,
                    transform.pos.y - 48
                    );
                rect.setFillColor(sf::Color(96, 96, 96));
                rect.setOutlineColor(sf::Color::Black);
                rect.setOutlineThickness(2);
                m_game->window().draw(rect);

                float ratio = (float)(h.current) / h.max;
                size.x *= ratio;
                rect.setSize({size.x, size.y});
                rect.setFillColor(sf::Color(255, 0, 0));
                rect.setOutlineThickness(0);
                m_game->window().draw(rect);

                for (int i = 0; i < h.max; i++)
                {
                    tick.setPosition(
                        rect.getPosition() + sf::Vector2f(i * 64 * 1.0 / h.max, 0)
                        );
                    m_game->window().draw(tick);
                }
            }
        }
    }

    // draw all Entity collision bounding boxes with a rectangle shape
    if (m_drawCollision)
    {
        // draw bounding box
        sf::CircleShape dot(4);
        for (const auto& entity: m_entityManager.getEntities())
        {
            if (entity->has<CBoundingBox>())
            {
                auto& box = entity->get<CBoundingBox>();
                auto& transform = entity->get<CTransform>();
                sf::RectangleShape rect;
                rect.setSize(sf::Vector2f(box.size.x - 1, box.size.y - 1));
                rect.setOrigin(sf::Vector2f(box.halfSize.x, box.halfSize.y));
                // rect.setPosition(box.center.x, box.center.y);
                rect.setPosition(transform.pos.x, transform.pos.y);
                rect.setFillColor(sf::Color(0, 0, 0, 0));

                if (box.blockMove && box.blockVision)
                {
                    rect.setOutlineColor(sf::Color::Black);
                }
                if (box.blockMove && !box.blockVision)
                {
                    rect.setOutlineColor(sf::Color::Blue);
                }
                if (!box.blockMove && box.blockVision)
                {
                    rect.setOutlineColor(sf::Color::Red);
                }
                if (!box.blockMove && !box.blockVision)
                {
                    rect.setOutlineColor(sf::Color::White);
                }
                rect.setOutlineThickness(1);
                m_game->window().draw(rect);

                // draw line between player and npc
                if (!player())
                    continue;
                if (entity->tag() == "npc")
                {
                    auto& ePos = entity->get<CTransform>().pos;
                    auto view = m_game->window().getView().getCenter();
                    if (ePos.x >= view.x - (float)width() / 2.0 &&
                        ePos.x <= view.x + (float)width() / 2.0 &&
                        ePos.y >= view.y - (float)height() / 2.0 &&
                        ePos.y <= view.y + (float)height() / 2.0
                    )
                    {
                        drawLine(
                            player()->get<CTransform>().pos,
                            entity->get<CTransform>().pos
                            );
                    }
                }
            }

            // draw patrol points
            if (entity->has<CFollowPlayer>())
            {
                auto& h = entity->get<CFollowPlayer>().home;
                dot.setPosition(h.x, h.y);
                m_game->window().draw(dot);
            }
            if (entity->has<CPatrol>())
            {
                for (auto p: entity->get<CPatrol>().positions)
                {
                    Vec2 r = getRoomXY(entity->get<CTransform>().pos);
                    Vec2 pos = getPosition(r.x, r.y, p.x, p.y);
                    dot.setPosition(pos.x, pos.y);
                    m_game->window().draw(dot);
                }
            }
        }
    }

    // draw the grid so that can easily debug
    if (m_drawGrid)
    {
        float leftX = m_game->window().getView().getCenter().x - width() / 2.0;
        float rightX = leftX + width() + m_gridSize.x;
        float nextGridX = leftX - ((int)leftX % (int)m_gridSize.x);
        float topY = m_game->window().getView().getCenter().y - height() / 2.0;
        float bottomY = topY + height() + m_gridSize.y;
        float nextGridY = topY - ((int)topY % (int)m_gridSize.x);

        // draw room coordinate
        auto p = player();
        if (p)
        {
            Vec2 r = getRoomXY(p->get<CTransform>().pos);
            m_gridText.setString(
                "room \n" + std::to_string((int)r.x) + " " +
                std::to_string((int)r.y)
                );
            m_gridText.setPosition(
                leftX + m_gridSize.x + 3,
                topY + m_gridSize.y / 2
                );
            m_game->window().draw(m_gridText);
        }

        for (float x = nextGridX; x < rightX; x += m_gridSize.x)
        {
            drawLine(Vec2(x, topY), Vec2(x, bottomY));
        }

        for (float y = nextGridY; y < bottomY; y += m_gridSize.y)
        {
            drawLine(Vec2(leftX, y), Vec2(rightX, y));

            for (float x = nextGridX; x < rightX; x += m_gridSize.x)
            {
                int w = width();
                int h = height();
                std::string xCell = std::to_string(
                    ((((int)x % w) + w) % w) / (int)m_gridSize.x
                    );
                std::string yCell = std::to_string(
                    ((((int)y % h) + h) % h) / (int)m_gridSize.y
                    );
                m_gridText.setString("(" + xCell + "," + yCell + ")");
                m_gridText.setPosition(x + 3, y + 2);
                m_game->window().draw(m_gridText);
            }
        }
    }
}

void SceneZelda::init(const std::string& levelPath)
{
    loadLevel(levelPath);

    m_gridText.setCharacterSize(12);
    m_gridText.setFont(m_game->assets().getFont("Tech"));

    registerAction(sf::Keyboard::Escape, "QUIT");
    registerAction(sf::Keyboard::P, "PAUSE");
    registerAction(sf::Keyboard::Y, "TOGGLE_FOLLOW"); // toggle follow camera
    registerAction(sf::Keyboard::T, "TOGGLE_TEXTURE"); // toggle drawing (T)extures
    registerAction(sf::Keyboard::C, "TOGGLE_COLLISION"); // toggle drawing (C)ollision Box
    registerAction(sf::Keyboard::G, "TOGGLE_GRID"); // toggle drawing (G)rid

    registerAction(sf::Keyboard::W, "UP");
    registerAction(sf::Keyboard::S, "DOWN");
    registerAction(sf::Keyboard::A, "LEFT");
    registerAction(sf::Keyboard::D, "RIGHT");
    registerAction(sf::Keyboard::Space, "ATTACK");
}

void SceneZelda::loadLevel(const std::string& fileName)
{
    m_entityManager = EntityManager();

    // TODO:
    // Load the level file and put all entities in the manager
    // Use the getPosition() function below to convert room-tile coords to game world coords

    spawnPlayer();
}

Vec2 SceneZelda::getPosition(int rx, int ry, int tx, int ty) const
{
    // TODO:
    // Implement this function, which takes in the room (rx, ry) coordinate
    // as well as the tile (tx, ty) coordinate, and return the Vec2 game world
    // position of the center of the entity
    // e.g. room + gridX - m_gridSize.x / 2.0f
    const auto roomX = static_cast<float>(rx);
    const auto roomY = static_cast<float>(ry);
    const auto tileX = static_cast<float>(tx);
    const auto tileY = static_cast<float>(ty);

    return {
        roomX * width() + tileX * m_gridSize.x - m_gridSize.x / 2.0f,
        roomY * height() + tileY * m_gridSize.y - m_gridSize.y / 2.0f,
    };
}

void SceneZelda::spawnPlayer()
{
    auto p = m_entityManager.addEntity("player");
    p->add<CTransform>(Vec2(640, 480));
    p->add<CAnimation>(m_game->assets().getAnimation("LinkStandDown"), true);
    p->add<CBoundingBox>(Vec2(48, 48), true, false);
    p->add<CDraggable>(); // to test draggable
    p->add<CHealth>(7, 3);
    p->add<CState>("stand_down");

    // TODO:
    // Implement this function so that it uses the parameters input from the level file
    // Those parameters should be stored in the m_playerConfig variable
}

void SceneZelda::spawnSword(std::shared_ptr<Entity> entity)
{
    // TODO:
    // Implement the spawning of the sword, which:
    // - should be given the appropriate lifespan
    // - should spawn at the appropriate location based on player's facing direction
    // - be given a damage value of 1
    // - should play the "Slash" sound

    // e.g. m_game->playSound("Slash");
}

std::shared_ptr<Entity> SceneZelda::player()
{
    for (auto e: m_entityManager.getEntities("player"))
    {
        return e;
    }
    return nullptr;
}

void SceneZelda::sDrag()
{
    for (const auto& e: m_entityManager.getEntities())
    {
        if (e->has<CDraggable>() && e->get<CDraggable>().dragging)
        {
            const Vec2 wPos = windowToWorld(m_mousePos);
            e->get<CTransform>().pos = wPos;
        }
    }
}

void SceneZelda::sMovement()
{
    assert(player() != nullptr);

    auto& input = player()->get<CInput>();
    auto& state = player()->get<CState>();

    Vec2 playerVelocity(0, 0);
    m_playerConfig.speed = 5;

    if (input.up)
    {
        state.state = "standUp";
        playerVelocity.y = -m_playerConfig.speed;
    }
    else if (input.down)
    {
        state.state = "standDown";
        playerVelocity.y = m_playerConfig.speed;
    }
    else if (input.right)
    {
        state.state = "standRight";
        playerVelocity.x = m_playerConfig.speed;
    }
    else if (input.left)
    {
        state.state = "standReft"; // flipped
        playerVelocity.x = -m_playerConfig.speed;
    }
    else
    {
        state.state = "standUp";
    }
    player()->get<CTransform>().velocity = playerVelocity;

    for (const auto& el: m_entityManager.getEntities())
    {
        auto& t = el->get<CTransform>();
        t.pos += t.velocity;
    }
}

void SceneZelda::sAI()
{
    // TODO: Implement enemy AI
    // Implement Follow behavior
    // Implement Patrol behavior
}

void SceneZelda::sStatus()
{
    // TODO:
    // Implement Lifespan here
    // Implement Invincibility Frames here
}

void SceneZelda::sAnimation()
{
    // TODO:
    // Implement player facing direction animation
    // Implement sword animation based on player facing
    // The sword should move if the player changes direction mid swing
    // Implement destruction of entities with non-repeating finished animations
}

void SceneZelda::sCamera()
{
    // TODO:
    // Implement camera view logic

    // get the current view, which we will modify in the if-statement below
    sf::View view = m_game->window().getView();

    if (m_follow)
    {
        // calculate view for player follow camera
    }
    else
    {
        // calculate view for room-based camera
    }

    // then set the window view
    m_game->window().setView(view);
}

void SceneZelda::sCollision()
{
    // TODO:
    // Implement entity - tile collisions
    // Implement player - enemy collisions with appropriate damage calculations
    // Implement sword - NPC collisions
    // Implement entity - heart collisions and life gain logic
    // Implement black tile collisions / 'teleporting'
    // You may want to use helper functions for these behaviors or this function will get long
}

void SceneZelda::sGUI()
{
    ImGui::Begin("Scene Properties");

    if (ImGui::BeginTabBar("MyTabBar"))
    {

        if (ImGui::BeginTabItem("Debug"))
        {
            ImGui::Checkbox("Draw Grid (G)", &m_drawGrid);
            ImGui::Checkbox("Draw Textures (T)", &m_drawTextures);
            ImGui::Checkbox("Draw Debug (C)", &m_drawCollision);
            ImGui::Checkbox("Follow Cam (Y)", &m_follow);

            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Animations"))
        {
            // TODO:
            ImGui::Text("Do this");

            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Entity Manager"))
        {
            // TODO:
            ImGui::Text("Do this too");

            ImGui::EndTabItem();
        }

        ImGui::EndTabBar();
    }
    ImGui::End();
}

Vec2 SceneZelda::windowToWorld(const Vec2& mousePosition) const
{
    // For a non-moving up/down window this solution is okay
    auto view = m_game->window().getView();
    float wx = view.getCenter().x - (m_game->window().getSize().x / 2.0f);
    return {mousePosition.x + wx, mousePosition.y};
}

Vec2 SceneZelda::getRoomXY(const Vec2& pos) const
{
    auto winSize = m_game->window().getSize();
    int roomX = static_cast<int>(pos.x) / static_cast<int>(winSize.x);
    int roomY = static_cast<int>(pos.y) / static_cast<int>(winSize.y);
    if (pos.x < 0) { roomX--; }
    if (pos.y < 0) { roomY--; }
    return {static_cast<float>(roomX), static_cast<float>(roomY)};
}
