#include "LevelRenderer.h"
#include <SFML/Graphics.hpp>
#include <iostream>

void LevelRenderer::renderLevel(sf::RenderWindow& window,
                                const sf::Texture& worldTileset,
                                const sf::Texture& houseTileset,
                                const std::vector<std::vector<int>>& backgroundData,
                                const std::vector<std::vector<int>>& foregroundData,
                                int tileSize,
                                int worldFirstGID,
                                int houseFirstGID) {
    // Рисуем фоновый слой
    renderLayer(window, worldTileset, houseTileset, backgroundData, tileSize, worldFirstGID, houseFirstGID);

    // Рисуем передний слой
    renderLayer(window, worldTileset, houseTileset, foregroundData, tileSize, worldFirstGID, houseFirstGID);
}

void LevelRenderer::renderLayer(sf::RenderWindow& window,
                                const sf::Texture& worldTileset,
                                const sf::Texture& houseTileset,
                                const std::vector<std::vector<int>>& layerData,
                                int tileSize,
                                int worldFirstGID,
                                int houseFirstGID) {
    for (size_t y = 0; y < layerData.size(); ++y) {
        for (size_t x = 0; x < layerData[y].size(); ++x) {
            int tileID = layerData[y][x];
            if (tileID == 0) continue; // Пропускаем пустые тайлы

            const sf::Texture* tilesetTexture = nullptr;
            int localTileID = 0;

            // Выбираем tileset в зависимости от tileID
            if (tileID >= houseFirstGID) {
                tilesetTexture = &houseTileset;
                localTileID = tileID - houseFirstGID;
            } else if (tileID >= worldFirstGID) {
                tilesetTexture = &worldTileset;
                localTileID = tileID - worldFirstGID;
            } else {
                //std::cerr << "Invalid tile ID: " << tileID << " at position (" << x << ", " << y << ")" << std::endl;
                continue;
            }

            if (!tilesetTexture) {
                std::cerr << "No tileset found for tile ID: " << tileID << " at position (" << x << ", " << y << ")" << std::endl;
                continue;
            }

            // Вычисляем координаты тайла на текстуре
            int tilesPerRow = tilesetTexture->getSize().x / tileSize;
            int tileX = (localTileID % tilesPerRow) * tileSize;
            int tileY = (localTileID / tilesPerRow) * tileSize;

            // Создаем спрайт и рисуем тайл
            sf::Sprite tile(*tilesetTexture);
            tile.setTextureRect(sf::IntRect(tileX, tileY, tileSize, tileSize));
            tile.setPosition(x * tileSize, y * tileSize);
            window.draw(tile);
        }
    }
}