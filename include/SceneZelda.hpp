#include "SFML/Graphics/Text.hpp"

#include "EntityManager.hpp"
#include "Scene.hpp"

class SceneZelda: public Scene
{
    struct PlayerConfig
    {
        float X, Y, CX, CY, SPEED, HEALTH;
        std::string WEAPON;
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

    void sMovement();
    void sAI();
    void sStatus();
    void sAnimation();
    void sCamera();
    void sCollision();
    void sGUI();

    // void changePlayerStateTo(std::string s);
    // void changePlayerStateTo(const std::string& state, const Vec2& facing);
    // Vec2 windowToWorld(const Vec2& pos);
    Vec2 getRoomXY(const Vec2& pos);

    PlayerConfig m_playerConfig{};
    sf::Text m_gridText;
    const Vec2 m_gridSize = {64, 64};

    std::string m_levelPath;
    bool m_drawCollision = false;
    bool m_drawGrid = false;
    bool m_drawTextures = true;
    bool m_follow = false;
};
