#include "Player.h"
#include "Constants.h"
#include "ContactListener.h"
#include <iostream>

Player::Player(b2World& world, float x, float y, ContactListener* contactListener) : contactListener(contactListener) {
    // Настройка физического тела
    b2BodyDef bodyDef;
    bodyDef.type = b2_dynamicBody;
    bodyDef.position.Set(x, y); // Начальная позиция
    body = world.CreateBody(&bodyDef);

    // Устанавливаем пользовательские данные для игрока
    body->GetUserData().pointer = PLAYER_USER_DATA;

    // Создаем форму тела (прямоугольник)
    b2PolygonShape shape;
    shape.SetAsBox(ScaleCharacter / 2.0f, ScaleCharacter / 2.0f); // Размер тела: ScaleCharacter x ScaleCharacter

    // Создаем фикстуру
    b2FixtureDef fixtureDef;
    fixtureDef.shape = &shape;
    fixtureDef.density = 1.0f;
    fixtureDef.friction = 0.8f;
    body->CreateFixture(&fixtureDef);

    // Загрузка текстуры игрока
    if (!texture.loadFromFile("assets/textures/player.png")) {
        std::cerr << "Failed to load player texture!" << std::endl;
    }

    sprite.setTexture(texture);
    sprite.setOrigin(texture.getSize().x / 2.0f, texture.getSize().y / 2.0f);

    std::cout << "Player created at position: (" << x << ", " << y << ")" << std::endl;
}

void Player::update(float deltaTime) {
    // Обновляем позицию спрайта на основе физики
    b2Vec2 positionB2 = body->GetPosition();
    sf::Vector2f position(positionB2.x * SCALE, positionB2.y * SCALE);
    sprite.setPosition(position);

    // Логирование позиции игрока
    std::cout << "Player position: (" << position.x << ", " << position.y << ")" << std::endl;

    // Замедление при отпускании клавиш
    if (!isRunning) {
        b2Vec2 velocity = body->GetLinearVelocity();
        velocity.x *= (1.0f - PLAYER_SLOWDOWN_FACTOR * deltaTime);
        body->SetLinearVelocity(velocity);
    }

    // Ограничение максимальной скорости
    b2Vec2 velocity = body->GetLinearVelocity();
    if (velocity.x > PLAYER_MAX_SPEED) {
        body->SetLinearVelocity(b2Vec2(PLAYER_MAX_SPEED, velocity.y));
    } else if (velocity.x < -PLAYER_MAX_SPEED) {
        body->SetLinearVelocity(b2Vec2(-PLAYER_MAX_SPEED, velocity.y));
    }
}

void Player::handleInput() {
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
        b2Vec2 velocity = body->GetLinearVelocity();
        velocity.x = PLAYER_RUN_FORCE;
        body->SetLinearVelocity(velocity);
        isRunning = true;
    } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
        b2Vec2 velocity = body->GetLinearVelocity();
        velocity.x = -PLAYER_RUN_FORCE;
        body->SetLinearVelocity(velocity);
        isRunning = true;
    } else {
        isRunning = false;
    }

    // Проверяем, что игрок стоит на земле и нажата клавиша пробела
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space) && !contactListener->isJumping) {
        b2Vec2 velocity = body->GetLinearVelocity();
        velocity.y = PLAYER_JUMP_IMPULSE;
        body->SetLinearVelocity(velocity);
        contactListener->isJumping = true;
    }
}

void Player::draw(sf::RenderWindow& window) {
    window.draw(sprite);
}

sf::Vector2f Player::getPosition() const {
    b2Vec2 positionB2 = body->GetPosition();
    return sf::Vector2f(positionB2.x * SCALE, positionB2.y * SCALE);
}