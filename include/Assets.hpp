#ifndef ASSETS_H
#define ASSETS_H

#include <map>

#include <SFML/Graphics.hpp>
#include <SFML/Audio/Sound.hpp>

#include "Animation.hpp"

class Assets
{
public:
    Assets();
    ~Assets();

    void loadFromFile(const std::string& path);

    [[nodiscard]] const std::map<std::string, sf::Texture>& getTextures() const;
    [[nodiscard]] const std::map<std::string, Animation>& getAnimations() const;
    [[nodiscard]] std::map<std::string, sf::Sound>& getSounds();

    [[nodiscard]] const sf::Texture& getTexture(const std::string& texName) const;
    [[nodiscard]] const Animation& getAnimation(const std::string& animName) const;
    [[nodiscard]] const sf::Font& getFont(const std::string& fonrName) const;
    [[nodiscard]] const sf::Sound& getSound(const std::string& soundName) const;

private:
    void addTexture(const std::string& texName, const std::string& path); // bool s
    void addAnimation(const std::string& animName, const Animation& animation);
    void addFont(const std::string& fontName, const std::string& path);
    void addSound(const std::string& fontName, const std::string& path);

    std::map<std::string, sf::Texture> m_textureMap;
    std::map<std::string, Animation> m_animationMap;
    std::map<std::string, sf::Font> m_fontMap;
    std::map<std::string, sf::SoundBuffer> m_soundBufferMap;
    std::map<std::string, sf::Sound> m_soundMap;
};

#endif //ASSETS_H
