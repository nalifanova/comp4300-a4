#include "SFML/Graphics/Text.hpp"

#include "EntityManager.hpp"
#include "Scene.hpp"

class SceneZelda: public Scene
{
    struct PlayerConfig
    {
        float x, y, cX, cY, speed, health;
        std::string weapon;
    };

public:
    SceneZelda(GameEngine* gameEngine, std::string& levelPath);

    void update() override;

protected:
    void onEnd() override;
    void sDoAction(const Action& action) override;
    void sRender() override;

    void init(const std::string& levelPath);
    void loadLevel(const std::string& filename);

    Vec2 getPosition(int rx, int ry, int tx, int ty) const;
    void spawnPlayer();
    void spawnSword(std::shared_ptr<Entity> entity);
    std::shared_ptr<Entity> player();

    void sDrag();
    void sMovement();
    void sAI();
    void sStatus();
    void sAnimation();
    void sCamera();
    void sCollision();
    void sGUI();

    Vec2 windowToWorld(const Vec2& mousePosition) const;
    Vec2 getRoomXY(const Vec2& pos) const;

    void collisionEntities(std::shared_ptr<Entity>& entity, std::shared_ptr<Entity>& tile);
    void entityTileCollision();
    void playerNpcCollision();
    void swordNpcCollision();
    void entityHeartCollision();
    void blackTileCollision();

    void stateAnimation(std::string& animName, const std::shared_ptr<Entity>& entity) const;
    bool isPositionOccupied(const sf::Vector2f& position);
    void drawTexture(const std::shared_ptr<Entity>& entity) const;

    PlayerConfig m_playerConfig{};
    sf::Text m_gridText;
    const Vec2 m_gridSize = {64, 64};
    Vec2 m_mousePos;

    std::string m_levelPath;
    bool m_drawCollision = false;
    bool m_drawGrid = false;
    bool m_drawTextures = true;
    bool m_follow = false;

    bool m_isDragging = false;
};
