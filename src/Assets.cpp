#include "Assets.hpp"

#include <cassert>
#include <fstream>
#include <iostream>

#include <SFML/Audio/SoundBuffer.hpp>


Assets::Assets() = default;

Assets::~Assets() = default;

void Assets::loadFromFile(const std::string& path)
{
    std::ifstream file(path);
    if (!file)
    {
        std::cerr << "Could not load " << path << " file!\n";
        exit(-1);
    }

    std::string assetType;
    while (file >> assetType)
    {
        if (assetType == "Texture")
        {
            std::string name;
            std::string imageFile;
            file >> name >> imageFile;
            addTexture(name, imageFile);
        }
        else if (assetType == "Animation")
        {
            std::string animationName;
            std::string texName;
            int frames, speed;
            file >> animationName >> texName >> frames >> speed;
            const sf::Texture& tex = getTexture(texName);
            addAnimation(animationName, Animation(animationName, tex, frames, speed));
        }
        else if (assetType == "Font")
        {
            std::string fontName;
            std::string fontPath;
            file >> fontName >> fontPath;
            addFont(fontName, fontPath);
        }
        else if (assetType == "Sound")
        {
            std::string soundName;
            std::string soundPath;
            file >> soundName >> soundPath;
            addSound(soundName, soundPath);
        }
        else
        {
            std::cerr << "Incorrect asset type: " << assetType << "\n";
            exit(-1);
        }
    }
}

const sf::Texture& Assets::getTexture(const std::string& texName) const
{
    assert(m_textureMap.find(texName) != m_textureMap.end());
    return m_textureMap.at(texName);
}

const Animation& Assets::getAnimation(const std::string& animName) const
{
    assert(m_animationMap.find(animName) != m_animationMap.end());
    return m_animationMap.at(animName);
}

std::map<std::string, sf::Sound>& Assets::getSounds()
{
    return m_soundMap;
}

const std::map<std::string, sf::Texture>& Assets::getTextures() const
{
    return m_textureMap;
}

const std::map<std::string, Animation>& Assets::getAnimations() const
{
    return m_animationMap;
}

const sf::Font& Assets::getFont(const std::string& fonrName) const
{
    assert(m_fontMap.find(fonrName) != m_fontMap.end());
    return m_fontMap.at(fonrName);
}

const sf::Sound& Assets::getSound(const std::string& soundName) const
{
    assert(m_soundMap.find(soundName) != m_soundMap.end());
    return m_soundMap.at(soundName);
}

// private
void Assets::addTexture(const std::string& texName, const std::string& path)
{
    sf::Texture texture;
    if (!texture.loadFromFile(path))
    {
        std::cerr << "Could not load image: " << path << "!\n";
        exit(-1);
    }
    m_textureMap[texName] = texture;
}

void Assets::addAnimation(const std::string& animName, const Animation& animation)
{
    m_animationMap[animName] = animation;
}

void Assets::addFont(const std::string& fontName, const std::string& path)
{
    sf::Font font;
    if (!font.loadFromFile(path))
    {
        std::cerr << "Could not load font: " << path << "\n";
        exit(-1);
    }
    m_fontMap[fontName] = font;
}

void Assets::addSound(const std::string& fontName, const std::string& path)
{
    sf::SoundBuffer sb;
    if (!sb.loadFromFile(path))
    {
        std::cerr << "Could not load sound: " << path << "\n";
        exit(-1);
    }
    m_soundBufferMap[fontName] = sb;
    m_soundMap[fontName] = sf::Sound(m_soundBufferMap.at(fontName));
}
