#include "SceneZelda.hpp"

#include <fstream>
#include <iostream>

#include "imgui.h"
#include <imgui_internal.h>
#include "imgui-SFML.h"

#include "Components.hpp"
#include "GameEngine.hpp"
#include "Physics.hpp"
#include "SceneMenu.hpp"

void SceneZelda::guiShowTable(const std::vector<std::shared_ptr<Entity>>& entities, bool showHeader)
{
    if (ImGui::BeginTable("Table", 6))
    {
        ImGui::TableSetupColumn("Del", ImGuiTableColumnFlags_WidthFixed, 30.0f);
        ImGui::TableSetupColumn("Img", ImGuiTableColumnFlags_WidthFixed, 40.0f);
        ImGui::TableSetupColumn("ID", ImGuiTableColumnFlags_WidthFixed, 30.0f);
        ImGui::TableSetupColumn("Type", ImGuiTableColumnFlags_WidthFixed, 50.0f);
        ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthFixed, 70.0f);
        ImGui::TableSetupColumn("Position");

        if (showHeader) { ImGui::TableHeadersRow(); }

        for (auto& e: entities)
        {
            auto& anim = e->get<CAnimation>().animation;
            auto& pos = e->get<CTransform>().pos;

            ImGui::TableNextRow(0, 32.0f);

            ImGui::TableSetColumnIndex(0);
            if (ImGui::Button("[x]"))
            {
                e->destroy();
                if (e->tag() == "Player") { spawnPlayer(); }
            }
            ImGui::SetItemTooltip("Remove %s", e->tag().c_str());

            ImGui::TableSetColumnIndex(1);
            ImGui::Image(anim.getSprite(), sf::Vector2f(30.0f, 30.0f));

            ImGui::TableSetColumnIndex(2);
            ImGui::Text("%d", static_cast<int>(e->id()));

            ImGui::TableSetColumnIndex(3);
            ImGui::Text("%s", e->tag().c_str());

            ImGui::TableSetColumnIndex(4);
            ImGui::Text("%s", anim.getName().c_str());

            ImGui::TableSetColumnIndex(5);
            ImGui::Text("(%d, %d)", static_cast<int>(pos.x), static_cast<int>(pos.y));
        }

        ImGui::EndTable();
    }
}

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
    m_game->changeScene("Menu", std::make_shared<SceneMenu>(m_game));
}

void SceneZelda::sDoAction(const Action& action)
{
    auto& input = player()->get<CInput>();

    // Only the setting of the player's input component variables should be set here
    if (action.name() == "MOUSE_MOVE") { m_mousePos = action.pos(); }

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
        // Note: last rendered is on top of previous rendered
        std::vector<std::string> tags = {"Tile", "NPC", "Player", "Sword"};
        for (const auto& tag: tags)
        {
            for (const auto& entity: m_entityManager.getEntities(tag))
            {
                drawTexture(entity);

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
                if (entity->tag() == "NPC")
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

    auto& path = fileName; // should think of a path

    std::ifstream file(path);
    if (!file)
    {
        std::cerr << "Could not load " << path << " file!\n";
        exit(-1);
    }

    std::string token; // player, tile, npc
    std::string name;
    while (file.good())
    {
        file >> token;
        if (token == "Tile")
        {
            int rx, ry, tx, ty, bm, bv;
            file >> name >> rx >> ry >> tx >> ty >> bm >> bv;
            auto tile = m_entityManager.addEntity(token);
            tile->add<CAnimation>(m_game->assets().getAnimation(name), true);
            tile->add<CTransform>(getPosition(rx, ry, tx, ty));
            tile->add<CBoundingBox>(tile->get<CAnimation>().animation.getSize(), bm, bv);
            tile->add<CDraggable>();
        }
        else if (token == "NPC")
        {
            std::string aiType;
            int rx, ry, tx, ty, bm, bv, health, damage;
            float speed;
            file >> name >> rx >> ry >> tx >> ty >> bm >> bv >> health >> damage >> aiType >> speed;

            auto npc = m_entityManager.addEntity(token);
            npc->add<CAnimation>(m_game->assets().getAnimation(name), true);
            npc->add<CTransform>(getPosition(rx, ry, tx, ty));
            npc->add<CBoundingBox>(npc->get<CAnimation>().animation.getSize(), bm, bv);
            npc->add<CDraggable>();
            npc->add<CHealth>(health, health);
            npc->add<CDamage>(damage);

            if (aiType == "Follow")
            {
                npc->add<CFollowPlayer>(npc->get<CTransform>().pos, speed);
            }
            else if (aiType == "Patrol")
            {
                int n, posX, posY;
                std::vector<Vec2> patrolNodes{};
                file >> n;
                // Could be N positions with coords posX, posY
                for (size_t i = 0; i < n; i++)
                {
                    file >> posX >> posY;
                    patrolNodes.emplace_back(posX, posY);
                }
                npc->add<CPatrol>(patrolNodes, speed);
            }
        }
        else if (token == "Player")
        {
            file >> m_playerConfig.x >> m_playerConfig.y >> m_playerConfig.cX >> m_playerConfig.cY
                >> m_playerConfig.speed >> m_playerConfig.health;
        }
    }
    spawnPlayer();
}

Vec2 SceneZelda::getPosition(const int rx, const int ry, const int tx, const int ty) const
{
    const auto roomX = static_cast<float>(rx);
    const auto roomY = static_cast<float>(ry);
    const auto tileX = static_cast<float>(tx);
    const auto tileY = static_cast<float>(ty);

    return {
        roomX * width() + tileX * m_gridSize.x - m_gridSize.x / 2.0f
      , roomY * height() + tileY * m_gridSize.y - m_gridSize.y / 2.0f,
    };
}

void SceneZelda::spawnPlayer()
{
    if (const auto mPlayer = player()) { mPlayer->destroy(); }

    auto p = m_entityManager.addEntity("Player");
    p->add<CTransform>(Vec2(m_playerConfig.x, m_playerConfig.y));
    p->get<CTransform>().facing = Vec2(0.0, -1.0f); // down
    p->add<CAnimation>(m_game->assets().getAnimation("LinkStandDown"), true);
    p->add<CBoundingBox>(Vec2(m_playerConfig.cX, m_playerConfig.cY), true, false);
    p->add<CDraggable>(); // to test draggable
    p->add<CHealth>(m_playerConfig.health, m_playerConfig.health);
    p->add<CState>("LinkStand");
}

void SceneZelda::spawnSword(std::shared_ptr<Entity> entity)
{
    auto& transf = entity->get<CTransform>();

    auto sword = m_entityManager.addEntity("Sword");
    // for sword position regarding player's hand and movement direction
    auto sign = transf.facing.y != 0 ? -1.0f : 1.0f;
    sword->add<CTransform>(transf.pos + Vec2(m_gridSize.x, m_gridSize.y) * transf.facing * sign);
    sword->get<CTransform>().facing = transf.facing;

    std::string animName = "LinkSword";
    stateAnimation(animName, sword);
    sword->add<CAnimation>(m_game->assets().getAnimation(animName), true);
    sword->add<CBoundingBox>(sword->get<CAnimation>().animation.getSize());
    sword->add<CDamage>(1);
    sword->add<CLifespan>(15, m_currentFrame);

    m_game->playSound("SSwordSlash");
}

std::shared_ptr<Entity> SceneZelda::player()
{
    for (auto e: m_entityManager.getEntities("Player")) { return e; }
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

    const auto mPlayer = player();
    const auto& input = mPlayer->get<CInput>();
    auto& state = mPlayer->get<CState>();
    auto& transf = mPlayer->get<CTransform>();

    Vec2 playerVelocity(0, 0);
    Vec2 facing = transf.facing;
    bool isMoving = true;

    if ((input.up && input.down) || (input.left && input.right) || (input.up && input.attack) ||
        (input.down && input.attack) || (input.left && input.attack) || (input.right && input.attack)
    )
    {
        player()->get<CAnimation>().paused = true;
        return; // can't hold keys in opposite directions
    }

    if (input.up)
    {
        playerVelocity.y = -m_playerConfig.speed;
        facing = Vec2(0.0, 1.0f);
    }
    else if (input.down)
    {
        playerVelocity.y = m_playerConfig.speed;
        facing = Vec2(0.0, -1.0f);
    }
    else if (input.right)
    {
        playerVelocity.x = m_playerConfig.speed;
        facing = Vec2(1.0, 0.0f);
    }
    else if (input.left)
    {
        playerVelocity.x = -m_playerConfig.speed;
        facing = Vec2(-1.0, 0.0f);
    }
    else
    {
        playerVelocity = Vec2(0.0, 0.0f);;
        isMoving = false;
    }

    if (transf.facing != facing)
    {
        transf.facing = facing; // update
        state.changed = true;
    }

    // attack / move / stand
    if (input.attack)
    {
        if (state.state != "LinkAtk") // prefix is set here, suffix in animation
        {
            state.state = "LinkAtk";
            state.changed = true;
            spawnSword(mPlayer);
        }
    }
    else if (isMoving)
    {
        if (state.state != "LinkMove")
        {
            state.state = "LinkMove";
            state.changed = true;
        }
    }
    else
    {
        if (state.state != "LinkStand")
        {
            state.state = "LinkStand";
            state.changed = true;
        }
    }

    player()->get<CTransform>().velocity = playerVelocity;

    for (const auto& el: m_entityManager.getEntities())
    {
        auto& t = el->get<CTransform>();
        t.prevPos = t.pos;
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
    for (auto sword: m_entityManager.getEntities("Sword"))
    {
        auto& lifespan = sword->get<CLifespan>();

        if (static_cast<int>(m_currentFrame) - lifespan.frameCreated > lifespan.lifespan)
        {
            sword->destroy();
            player()->get<CInput>().attack = false;
        }
    }

    auto mPlayer = player();
    if (mPlayer->has<CInvincibility>())
    {
        auto& invincibility = mPlayer->get<CInvincibility>();
        invincibility.iframes -= 1;
        if (invincibility.iframes < 0) { mPlayer->remove<CInvincibility>(); }
    }
}

void facingDirection(std::string& animName, CTransform& transf)
{
    if (transf.facing.y == 1.0f)
    {
        animName.append("Up");
        transf.scale.x = 1.0f;
    }
    else if (transf.facing.y == -1.0f)
    {
        animName.append("Down");
        transf.scale.x = 1.0f;
    }
    else if (transf.facing.x != 0.0f)
    {
        animName.append("Right");
        transf.scale.x = transf.facing.x; // flipping to left if needed
    }
}

void SceneZelda::sAnimation()
{
    // Implement destruction of entities with non-repeating finished animations ?
    auto p = player();
    auto& state = p->get<CState>();

    if (state.changed)
    {
        std::string animName = state.state; // prefix like LinkStand
        stateAnimation(animName, p);
        state.changed = false;
    }

    // Animate entities
    for (auto& e: m_entityManager.getEntities())
    {
        if (!e->has<CAnimation>()) { continue; }

        auto& anim = e->get<CAnimation>();
        if (anim.animation.hasEnded() && !anim.repeat) { e->destroy(); }
        if (!anim.paused) { anim.animation.update(); }
    }
}

void SceneZelda::sCamera()
{
    assert(player() != nullptr);
    auto position = player()->get<CTransform>().pos;
    sf::View view = m_game->window().getView();

    if (m_follow)
    {
        // calculate view for player follow camera
        view.setCenter(position.x, position.y);
    }
    else
    {
        // calculate view for room-based camera
        Vec2 room = getRoomXY(position);
        view.setCenter(room.x * width() + width() / 2.0f, room.y * height() + height() / 2.0f);
    }
    m_game->window().setView(view);
}

void SceneZelda::sCollision()
{
    entityTileCollision();
    playerNpcCollision();
    swordNpcCollision();
    entityHeartCollision();
    blackTileCollision(); // 'teleporting'
}

void SceneZelda::sGUI()
{
    // ImGui::ShowDemoWindow();
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

        if (ImGui::BeginTabItem("Sounds"))
        {
            for (auto& [name, sound]: m_game->assets().getSounds())
            {
                ImGui::PushID(name.c_str()); // bind id with a unique identifier which is a name
                if (ImGui::Button("Play")) { sound.play(); }
                ImGui::PopID(); // unbind an id
                ImGui::SameLine();

                ImGui::PushID(name.c_str());
                if (ImGui::Button("Stop")) { sound.stop(); }
                ImGui::PopID();
                ImGui::SameLine();

                ImGui::Text("%s", name.c_str()); // Sounds name
            }

            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Animations"))
        {
            size_t counter = 0;
            const auto windowSize = ImGui::GetWindowSize().x;
            const int tilesNumber = static_cast<int>(windowSize / (m_gridSize.x));

            for (auto& [name, anim]: m_game->assets().getAnimations())
            {
                if (name.find("Link") != std::string::npos) { continue; } // No player pics

                if (counter++ % tilesNumber != 0) { ImGui::SameLine(); } // tilesNumber columns

                bool isNpc = name.find("Stand") != std::string::npos;
                const sf::Texture* animTex = anim.getSprite().getTexture();
                ImVec2 uv0(0.0f, 0.0f); // Beginning of texture
                ImVec2 uv1(1.0f, 1.0f); // Center of texture
                int padding = 1;
                const auto& bgColor = isNpc
                    ? ImVec4(255, 128, 128, 20) // Not available
                    : ImVec4(0, 0, 0, 0); // available

                if (isNpc) { uv1.x = 0.5f; } // multiple imgs

                ImVec2 size(m_gridSize.x - 10.0f, m_gridSize.y - 10.0f);

                if (ImGui::ImageButton(
                    reinterpret_cast<void*>(animTex->getNativeHandle()),
                    size, uv0, uv1, padding, bgColor))
                {
                    if (isNpc) { continue; }

                    const auto tile = m_entityManager.addEntity("Tile");
                    tile->add<CAnimation>(anim, true);
                    auto view = m_game->window().getView().getCenter();
                    view.x += m_gridSize.x / 2.0f;
                    view.y += m_gridSize.y / 2.0f;
                    while (isPositionOccupied(view))
                    {
                        view = view + Physics::getRandomOffset(anim.getSprite().getGlobalBounds().width);
                    }
                    tile->add<CTransform>(Vec2(view.x, view.y));
                    tile->add<CBoundingBox>(anim.getSize());
                    tile->add<CDraggable>();
                }
                ImGui::SetItemTooltip("%s", !isNpc ? name.c_str() : (name + (" is unvailable")).c_str());
            }
            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Entity Manager"))
        {
            if (ImGui::TreeNode("Entities by Tag"))
            {
                for (auto& [tag, entityVector]: m_entityManager.getEntityMap())
                {
                    const bool showHeader = (tag != "Player" && tag != "Sword");
                    if (ImGui::CollapsingHeader(tag.c_str()))
                    {
                        guiShowTable(entityVector, showHeader);
                    }
                }
                ImGui::TreePop();
            }

            if (ImGui::TreeNode("All Entities"))
            {
                guiShowTable(m_entityManager.getEntities());

                ImGui::TreePop();
            }

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

// helper methods
void SceneZelda::collisionEntities(std::shared_ptr<Entity>& entity, std::shared_ptr<Entity>& tile)
{
    const auto overlap = Physics::getOverlap(entity, tile);
    if (overlap.x > 0.0f && overlap.y > 0.0f)
    {
        // Overlap: defining a direction
        const auto prevOverlap = Physics::getPreviousOverlap(entity, tile);
        auto& entityPos = entity->get<CTransform>().pos;
        auto& tilePos = tile->get<CTransform>().pos;

        // top/bottom collision
        if (prevOverlap.x > 0.0f) { entityPos.y += entityPos.y < tilePos.y ? -overlap.y : overlap.y; }

        // side collision
        if (prevOverlap.y > 0.0f) { entityPos.x += entityPos.x < tilePos.x ? -overlap.x : overlap.x; }
    }
}

void SceneZelda::entityTileCollision()
{
    auto mPlayer = player();
    for (auto& tile: m_entityManager.getEntities("Tile"))
    {
        collisionEntities(mPlayer, tile);

        for (auto& npc: m_entityManager.getEntities("NPC"))
        {
            collisionEntities(npc, tile);
        }
    }
}

void SceneZelda::playerNpcCollision()
{
    auto mPlayer = player();
    if (mPlayer->has<CInvincibility>()) { return; }

    for (auto& npc: m_entityManager.getEntities("NPC"))
    {
        auto overlap = Physics::getOverlap(mPlayer, npc);
        if (overlap.x > 0.0f && overlap.y > 0.0f)
        {
            mPlayer->get<CHealth>().current -= npc->get<CDamage>().damage;
            mPlayer->add<CInvincibility>(60);
            // Debug msg
            std::cout << "Damage of " << npc->get<CAnimation>().animation.getName() << " is "
                << npc->get<CDamage>().damage << "\n";

            if (mPlayer->get<CHealth>().current <= 0)
            {
                mPlayer->destroy();
                m_game->playSound("SLinkDied");
                spawnPlayer();
            }
            else
            {
                m_game->playSound("SLinkDamaged");
            }
        }
    }
}

void SceneZelda::swordNpcCollision()
{
    for (auto& sword: m_entityManager.getEntities("Sword"))
    {
        if (!sword->has<CDamage>()) { continue; }

        for (auto& npc: m_entityManager.getEntities("NPC"))
        {
            auto overlap = Physics::getOverlap(sword, npc);
            if (overlap.x > 0.0f && overlap.y > 0.0f)
            {
                // Debug msg
                std::cout << npc->get<CAnimation>().animation.getName() << " has HP " << npc->get<CHealth>().current <<
                    "\n";

                npc->get<CHealth>().current -= sword->get<CDamage>().damage;
                sword->remove<CDamage>(); // one click - one damage
                // Debug msg
                std::cout << "Player hits NPC with " << sword->get<CDamage>().damage << "\n";

                if (npc->get<CHealth>().current <= 0)
                {
                    npc->destroy();
                    m_game->playSound("SEnemyDied");
                }
                else
                {
                    m_game->playSound("SEnemyDamaged");
                }
            }
        } // end npc loop
    } // end sword loop
}

void SceneZelda::entityHeartCollision()
{
    for (auto tile: m_entityManager.getEntities("Tile"))
    {
        if (tile->get<CAnimation>().animation.getName() != "TileHeart") { continue; }

        // Player and heard
        auto mPlayer = player();
        Vec2 overlap = Physics::getOverlap(mPlayer, tile);
        if (overlap.x >= 0.0f && overlap.y >= 0.0f)
        {
            mPlayer->get<CHealth>().current = mPlayer->get<CHealth>().max;
            tile->destroy();
            m_game->playSound("SLinkPickupHeart");
        }

        // NPC and heart
        for (auto npc: m_entityManager.getEntities("NPC"))
        {
            overlap = Physics::getOverlap(mPlayer, tile);
            if (overlap.x >= 0.0f && overlap.y >= 0.0f)
            {
                npc->get<CHealth>().current = npc->get<CHealth>().max;
                tile->destroy();
                m_game->playSound("SLinkPickupHeart");
            }
        }
    }
}

void SceneZelda::blackTileCollision()
{
    auto mPlayer = player();
    auto& playerPos = mPlayer->get<CTransform>().pos;
    std::vector<Vec2> coords;
    bool move = false;
    for (auto& tile: m_entityManager.getEntities("Tile"))
    {
        auto& anim = tile->get<CAnimation>().animation;
        if (anim.getName() != "TileBlack") { continue; }

        Vec2 overlap = Physics::getOverlap(mPlayer, tile);
        if (overlap.x >= 0.0f && overlap.y >= 0.0f) { move = true; }
        else { coords.push_back(tile->get<CTransform>().pos); }
    }

    if (move)
    {
        auto randomPos = Physics::getRandomCoordinates(coords);
        sf::Vector2f pos2f(randomPos.x, randomPos.y);
        while (isPositionOccupied(pos2f)) { pos2f = pos2f + Physics::getRandomOffset(m_gridSize.x); }
        playerPos = Vec2(pos2f.x, pos2f.y);
    }
}

void SceneZelda::stateAnimation(std::string& animName, const std::shared_ptr<Entity>& entity) const
{
    auto& transf = entity->get<CTransform>();
    facingDirection(animName, transf);
    entity->add<CAnimation>(m_game->assets().getAnimation(animName), true);
}

void SceneZelda::drawTexture(const std::shared_ptr<Entity>& entity) const
{
    auto& transform = entity->get<CTransform>();
    sf::Color c = sf::Color::White;
    if (entity->has<CInvincibility>()) { c = sf::Color(255, 255, 255, 128); }
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

bool SceneZelda::isPositionOccupied(const sf::Vector2f& position)
{
    for (auto& e: m_entityManager.getEntities())
    {
        if (e->get<CAnimation>().animation.getSprite().getGlobalBounds().contains(position))
            return true;
    }
    return false;
}
