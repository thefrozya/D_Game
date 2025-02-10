#include "LevelRenderer.h"
#include <SFML/Graphics.hpp>

void renderLevel(sf::RenderWindow& window, sf::Texture& tilesetTexture,
                 const std::vector<std::vector<int>>& levelData, int tileSize, int firstgid) {
    sf::Sprite sprite(tilesetTexture);

    for (size_t y = 0; y < levelData.size(); ++y) {
        for (size_t x = 0; x < levelData[y].size(); ++x) {
            int tileID = levelData[y][x];
            if (tileID == 0) continue; // Пропускаем пустые тайлы

            // Вычисляем координаты тайла на текстуре
            int tilesetWidth = tilesetTexture.getSize().x / tileSize;
            int tileX = (tileID - firstgid) % tilesetWidth;
            int tileY = (tileID - firstgid) / tilesetWidth;

            // Устанавливаем область текстуры для спрайта
            sprite.setTextureRect(sf::IntRect(tileX * tileSize, tileY * tileSize, tileSize, tileSize));

            // Устанавливаем позицию спрайта
            sprite.setPosition(static_cast<float>(x * tileSize), static_cast<float>(y * tileSize));

            // Отрисовываем тайл
            window.draw(sprite);
        }
    }
}
