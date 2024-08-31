#ifndef SCENE_MENU_H
#define SCENE_MENU_H

#include <deque>

#include "SFML/Graphics/Text.hpp"

#include "EntityManager.hpp"
#include "Scene.hpp"

class SceneMenu: public Scene
{
public:
    explicit SceneMenu(GameEngine* gameEngine = nullptr);

    void sRender() override;

protected:
    void init();
    void update() override;
    void onEnd() override;
    void sDoAction(const Action& action) override;

    std::vector<std::string> m_menuStrings;
    std::vector<sf::Text> m_menuItems;
    std::vector<std::string> m_levelPaths;

    sf::Sound m_titleMusic;
    sf::Text m_menuText;

    std::string m_title;
    size_t m_selectedMenuIndex = 0;
};

#endif //SCENE_MENU_H
