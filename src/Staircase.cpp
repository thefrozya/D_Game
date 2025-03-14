// Staircase.cpp
#include "Staircase.h"
#include "Constants.h" 
#include <iostream>

Staircase::Staircase(b2World& world, float x, float y, float width, float height) 
    : playerOnStaircase(false) {
    b2BodyDef bodyDef;
    bodyDef.type = b2_staticBody;
    bodyDef.position.Set(x + width / 2.0f, y + height / 2.0f);
    body = world.CreateBody(&bodyDef);

    // Устанавливаем userData для тела
    body->GetUserData().pointer = STAIRCASE_USER_DATA; // 5

    b2PolygonShape shape;
    shape.SetAsBox(width / 2.0f, height / 2.0f);

    b2FixtureDef fixtureDef;
    fixtureDef.shape = &shape;
    fixtureDef.isSensor = true;
    // Убираем userData из фикстуры
    b2Fixture* fixture = body->CreateFixture(&fixtureDef);

    std::cout << "Staircase created at (" << x << ", " << y << ") with body userData=" 
              << body->GetUserData().pointer << std::endl;

    this->shape.setSize(sf::Vector2f(width * SCALE, height * SCALE));
    this->shape.setPosition(x * SCALE, y * SCALE);
    this->shape.setFillColor(sf::Color::Transparent);
    this->shape.setOutlineColor(sf::Color::Yellow);
    this->shape.setOutlineThickness(1.0f);
}

Staircase::~Staircase() {
    if (body) std::cout << "Staircase destructor: body still exists" << std::endl;
}