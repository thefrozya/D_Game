#ifndef DEBUG_UTILS_H
#define DEBUG_UTILS_H

#include <SFML/Graphics.hpp>
#include <Box2D/Box2D.h>

// Функция для отладочной визуализации физики
void debugDrawPhysics(sf::RenderWindow& window, b2World& world, float scale);

#endif // DEBUG_UTILS_H