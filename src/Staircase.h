// Staircase.h
#ifndef STAIRCASE_H
#define STAIRCASE_H

#include <box2d/box2d.h>
#include <SFML/Graphics.hpp>

class Staircase {
public:
    Staircase(b2World& world, float x, float y, float width, float height);
    ~Staircase();

    b2Body* getBody() const { return body; }
    void setBody(b2Body* b) { body = b; }
    sf::RectangleShape& getShape() { return shape; }
    bool isPlayerOnStaircase() const { return playerOnStaircase; }
    void setPlayerOnStaircase(bool on) { playerOnStaircase = on; }

private:
    b2Body* body;
    sf::RectangleShape shape;
    bool playerOnStaircase;
};

#endif