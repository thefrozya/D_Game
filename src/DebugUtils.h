#ifndef DEBUG_UTILS_H
#define DEBUG_UTILS_H

#include <SFML/Graphics.hpp>
#include <Box2D/Box2D.h>
#include "Constants.h" 

// Константа для включения/отключения отладочной визуализации
extern bool DEBUG_DRAW_ENABLED;


// Функция для отладочной визуализации физики
void debugDrawPhysics(sf::RenderWindow& window, b2World& world, float scale);

#endif // DEBUG_UTILS_H