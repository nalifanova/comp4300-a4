#ifndef ANIMATION_H
#define ANIMATION_H

#include <string>

#include <SFML/Graphics/Sprite.hpp>

#include "Vec2.hpp"


class Animation {
public:
    Animation();
    Animation(const std::string &name, const sf::Texture &t);
    Animation(std::string name, const sf::Texture &t, size_t frameCount, size_t speed);

    void update();
    bool hasEnded() const;

    const std::string &getName() const;
    const Vec2 &getSize() const;
    const sf::Sprite &getSprite() const;
    sf::Sprite &getSprite();

private:
    sf::Sprite m_sprite;
    size_t m_frameCount = 1;       // total number of frames of animation
    size_t m_currentFrame = 0;     // the current frame of animation being played
    size_t m_speed = 0;            // the speed to play this animation
    Vec2 m_size = {1, 1}; // size of the animation frame
    std::string m_name = "none";
};

#endif //ANIMATION_H
