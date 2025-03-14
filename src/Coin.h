#ifndef COIN_H
#define COIN_H

#include <SFML/Graphics.hpp>
#include <Box2D/Box2D.h>
#include "ObjectBase.h"

class Coin : public ObjectBase {
    public:
        ObjectType getType() const override {
            return ObjectType::Coin;
        }
    Coin(b2World& world, float x, float y, sf::Texture& texture, int frameCount, float animationSpeed);
    ~Coin();

    void destroyBody(b2World& world);
    b2Body* getBody() const;
    void setBody(b2Body* b) { body = b; }

    void updateAnimation(float deltaTime);
    void draw(sf::RenderWindow& window);
    void collect(int score);
    bool isCollected() const;
    sf::FloatRect getBoundingBox() const;
    void markForNoDestruction();
    
    bool markedForDeletion = false; // Новый флаг

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
    bool shouldDestroyBody;
};

#endif // COIN_H