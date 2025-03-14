#ifndef MOVING_PLATFORM_H
#define MOVING_PLATFORM_H

#include <Box2D/Box2D.h>
#include <SFML/Graphics.hpp>

class MovingPlatform {
public:
    MovingPlatform(b2World& world, float x, float y, float width, float height, float speed,const sf::Texture& texture);
    void update(); // Обновление логики движения
    b2Body* getBody() const { return body; }
    void setBody(b2Body* b) { body = b; }
    ~MovingPlatform();

    void draw(sf::RenderWindow& window);

private:
    b2Body* body;
    float speed;       // Скорость движения 
    bool movingRight;  // Направление движения

    sf::Sprite sprite;
};

#endif