#ifndef LEVELRENDERER_H
#define LEVELRENDERER_H

#include <SFML/Graphics.hpp>
#include <vector>

namespace LevelRenderer {
    void renderLevel(sf::RenderWindow& window, sf::Texture& tilesetTexture,
                     const std::vector<std::vector<int>>& levelData, int tileSize, int firstgid);
}

#endif // LEVELRENDERER_H