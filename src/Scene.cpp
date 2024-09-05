#include "Scene.hpp"

#include "GameEngine.hpp"

Scene::Scene() = default;

Scene::Scene(GameEngine* gameEngine) :
    m_game(gameEngine) {}

Scene::~Scene() = default;

// abstract update, sDoAction, sRender

void Scene::doAction(const Action& action)
{
    sDoAction(action);
}

void Scene::simulate(const size_t frames)
{
    for (size_t i = 0; i < frames; i++)
    {
        update();
    }
}

void Scene::registerAction(int inputKey, const std::string& actionName)
{
    m_actionMap[inputKey] = actionName;
}

float Scene::width() const
{
    return static_cast<float>(m_game->window().getSize().x);
}

float Scene::height() const
{
    return static_cast<float>(m_game->window().getSize().y);
}

size_t Scene::currentFrame() const
{
    return m_currentFrame;
}

bool Scene::hasEnded() const
{
    return m_hasEnded;
}

const ActionMap& Scene::getActionMap() const
{
    return m_actionMap;
}

void Scene::drawLine(const Vec2& p1, const Vec2& p2)
{
    sf::Vertex line[] = {
        sf::Vertex(sf::Vector2f(p1.x, p1.y)), sf::Vertex(sf::Vector2f(p2.x, p2.y))
    };

    m_game->window().draw(line, 2, sf::Lines);
}

// protected
// onEnd is abstract

void Scene::setPaused(bool paused)
{
    m_paused = paused;
}
