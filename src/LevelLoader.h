#ifndef LEVELLOADER_H
#define LEVELLOADER_H

#include <SFML/Graphics.hpp>
#include <Box2D/Box2D.h>
#include <vector>
#include <string>
#include "Constants.h"
#include "Coin.h"
#include "Enemy.h"
#include "MovingPlatform.h"
#include "Staircase.h"
#include "Interactable.h"
#include <memory>

namespace LevelLoader {

   

    // Основной метод загрузки уровня
    bool loadLevel(const std::string &filePath,
        sf::Texture &worldTileset,
        sf::Texture &houseTileset,
        std::vector<std::vector<int>> &backgroundData,
        std::vector<std::vector<int>> &foregroundData,
        int &firstgid,
        b2World &world,
        sf::Vector2f &spawnPoint,
        float scale,
        int &mapWidthInTiles,
        int &mapHeightInTiles,
        int &tileSize,
        sf::Texture &coinTexture,
        std::vector<Coin> &coins,
        sf::Texture &enemyTexture,
        std::vector<std::unique_ptr<Enemy>> &enemies,
        std::vector<std::unique_ptr<MovingPlatform>> &movingPlatforms,
        sf::Texture& platformTexture,
        std::vector<Staircase>& staircases,
        std::vector<Key>& keys,
        const sf::Texture& keyTexture, 
        std::vector<Door>& doors,
        const sf::Texture& doorTexture);

}

#endif // LEVELLOADER_H