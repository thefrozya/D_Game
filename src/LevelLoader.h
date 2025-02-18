#ifndef LEVELLOADER_H
#define LEVELLOADER_H

#include <SFML/Graphics.hpp>
#include <Box2D/Box2D.h>
#include <vector>
#include <string>
#include "Constants.h"
#include "Coin.h"
#include "Enemy.h"
#include <memory>

namespace LevelLoader {
    bool loadLevel(const std::string& filePath, sf::Texture& tilesetTexture,
        std::vector<std::vector<int>>& levelData, int& firstgid,
        b2World& world, sf::Vector2f& spawnPoint, float scale,
        int& mapWidthInTiles, int& mapHeightInTiles, int& tileSize,sf::Texture& coinTexture,std::vector<Coin>& coins,sf::Texture& enemyTexture,std::vector<std::unique_ptr<Enemy>>& enemies);

    
    };

#endif // LEVELLOADER_H