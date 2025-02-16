#ifndef COIN_H
#define COIN_H

#include <SFML/Graphics.hpp>
#include <Box2D/Box2D.h>

class Coin {
public:
    Coin(b2World& world, float x, float y, sf::Texture& texture, int frameCount, float animationSpeed);
    ~Coin();

    void updateAnimation(float deltaTime);
    void draw(sf::RenderWindow& window);
    void collect();
    bool isCollected() const;
    sf::FloatRect getBoundingBox() const;

private:
    b2Body* body;
    sf::Sprite sprite;
    sf::IntRect textureRect;
    int frameWidth, frameHeight;
    int frameCount;
    float animationSpeed;
    int currentFrame;
    float elapsedTime;
    bool collected;
};

#endif // COIN_H