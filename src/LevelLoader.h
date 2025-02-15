#ifndef LEVELLOADER_H
#define LEVELLOADER_H

#include <SFML/Graphics.hpp>
#include <Box2D/Box2D.h>
#include <vector>
#include <string>
#include "Constants.h"

namespace LevelLoader {
    bool loadLevel(const std::string& filePath, sf::Texture& tilesetTexture,
        std::vector<std::vector<int>>& levelData, int& firstgid,
        b2World& world, sf::Vector2f& spawnPoint, float scale,
        int& mapWidthInTiles, int& mapHeightInTiles, int& tileSize);
    }

#endif // LEVELLOADER_H