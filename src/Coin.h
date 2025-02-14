#ifndef COIN_H
#define COIN_H

#include <SFML/Graphics.hpp>
#include <Box2D/Box2D.h>
#include <vector>

class Coin {
public:
    Coin(b2World& world, float x, float y, const sf::Texture& texture, int frameCount, float animationSpeed);
    void draw(sf::RenderWindow& window);
    bool isCollected() const;
    void collect();
    sf::FloatRect getBoundingBox() const;
    void updateAnimation(float deltaTime);

private:
    sf::Sprite sprite;
    std::vector<sf::IntRect> frames;
    int currentFrame = 0;
    float animationTimer = 0.0f;
    float animationSpeed;
    bool collected = false;
    b2Body* body; // Физическое тело монеты
};

#endif // COIN_H