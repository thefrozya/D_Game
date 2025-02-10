#ifndef LEVELRENDERER_H
#define LEVELRENDERER_H

#include <SFML/Graphics.hpp>
#include <vector>

void renderLevel(sf::RenderWindow& window, sf::Texture& tilesetTexture,
                 const std::vector<std::vector<int>>& levelData, int tileSize, int firstgid);

#endif