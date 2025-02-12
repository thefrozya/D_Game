#include "LevelRenderer.h"
#include <SFML/Graphics.hpp>

void LevelRenderer::renderLevel(sf::RenderWindow& window, sf::Texture& tilesetTexture,
                                const std::vector<std::vector<int>>& levelData, int tileSize, int firstgid) {
    sf::Sprite tile(tilesetTexture);
    for (size_t y = 0; y < levelData.size(); ++y) {
        for (size_t x = 0; x < levelData[y].size(); ++x) {
            int tileIndex = levelData[y][x];
            if (tileIndex > 0) { // Если тайл существует
                int tilesetId = tileIndex - firstgid;
                int tilesetX = tilesetId % (tilesetTexture.getSize().x / tileSize);
                int tilesetY = tilesetId / (tilesetTexture.getSize().x / tileSize);

                tile.setTextureRect(sf::IntRect(tilesetX * tileSize, tilesetY * tileSize, tileSize, tileSize));
                tile.setPosition(x * tileSize, y * tileSize);
                window.draw(tile);
            }
        }
    }
}