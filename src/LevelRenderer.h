#ifndef LEVELRENDERER_H
#define LEVELRENDERER_H

#include <SFML/Graphics.hpp>
#include <vector>

class LevelRenderer {
    public:
        void renderLevel(sf::RenderWindow& window,
                         const sf::Texture& worldTileset,
                         const sf::Texture& houseTileset,
                         const std::vector<std::vector<int>>& backgroundData,
                         const std::vector<std::vector<int>>& foregroundData,
                         int tileSize,
                         int worldFirstGID,
                         int houseFirstGID);
    
    private:
        void renderLayer(sf::RenderWindow& window,
                         const sf::Texture& worldTileset,
                         const sf::Texture& houseTileset,
                         const std::vector<std::vector<int>>& layerData,
                         int tileSize,
                         int worldFirstGID,
                         int houseFirstGID);
    };

#endif // LEVELRENDERER_H