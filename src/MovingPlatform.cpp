#include "MovingPlatform.h"
#include "Constants.h" 
#include <iostream>

MovingPlatform::MovingPlatform(b2World& world, float x, float y, float width, float height, float speed,const sf::Texture& texture) 
    : speed(speed), movingRight(true) {
    b2BodyDef bodyDef;
    bodyDef.type = b2_dynamicBody;;
    bodyDef.fixedRotation = true;  
    bodyDef.position.Set(x, y);
    body = world.CreateBody(&bodyDef);

    body->GetUserData().pointer = MOVE_PLATFORM_USER_DATA;
    std::cout << "Created MovingPlatform with userData: " << body->GetUserData().pointer 
              << " at (" << x << ", " << y << ")" << std::endl;

    b2PolygonShape shape;
    shape.SetAsBox(width / 2.0f, height / 2.0f);

    b2FixtureDef fixtureDef;
    fixtureDef.shape = &shape;
    fixtureDef.friction = 1.0f;
    body->CreateFixture(&fixtureDef);

    body->SetLinearVelocity(b2Vec2(speed, 0));

    sprite.setTexture(texture);
    float scaleX = (width * SCALE) / texture.getSize().x;  // Масштаб по X
    float scaleY = (height * SCALE) / texture.getSize().y; // Масштаб по Y
    sprite.setScale(scaleX, scaleY);

    // Устанавливаем origin в центр спрайта
    sprite.setOrigin(texture.getSize().x / 2.0f, texture.getSize().y / 2.0f); // Центр текстуры (23/2, 8/2)
    sprite.setPosition(x * SCALE, y * SCALE); // Позиция теперь соответствует центру

}

MovingPlatform::~MovingPlatform() {
    if (body) std::cout << "MovingPlatform destructor: body still exists" << std::endl;
}

void MovingPlatform::update() {
    b2Vec2 position = body->GetPosition();
    b2Vec2 velocity = body->GetLinearVelocity();
    /*std::cout << "MovingPlatform pos: (" << position.x << ", " << position.y 
              << "), vel: (" << velocity.x << ", " << velocity.y << ")" << std::endl;*/
    sprite.setPosition(position.x * SCALE, position.y * SCALE);
}

void MovingPlatform::draw(sf::RenderWindow& window) {
    window.draw(sprite);}