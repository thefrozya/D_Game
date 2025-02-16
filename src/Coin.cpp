#include "Coin.h"
#include "Constants.h"
#include <iostream>

Coin::Coin(b2World& world, float x, float y, sf::Texture& texture, int frameCount, float animationSpeed)
    : frameCount(frameCount), animationSpeed(animationSpeed), currentFrame(0), elapsedTime(0.0f), collected(false) {
    // Устанавливаем текстуру
    sprite.setTexture(texture);

    // Вычисляем размеры одного кадра
    frameWidth = texture.getSize().x / frameCount; // 96 / 6 = 16
    frameHeight = texture.getSize().y;            // 12

    // Начальный текстурный прямоугольник (первый кадр)
    textureRect = sf::IntRect(0, 0, frameWidth, frameHeight);
    sprite.setTextureRect(textureRect);

    // Устанавливаем позицию спрайта
    sprite.setOrigin(frameWidth / 2.0f, frameHeight / 2.0f); // Центр спрайта
    sprite.setPosition(x * SCALE, y * SCALE);

    // Создаем тело Box2D для монеты
    b2BodyDef bodyDef;
    bodyDef.type = b2_staticBody;
    bodyDef.position.Set(x, y);
    body = world.CreateBody(&bodyDef);

    b2CircleShape shape;
    shape.m_radius = 1.5f; // Радиус монеты

    b2FixtureDef fixtureDef;
    fixtureDef.shape = &shape;
    fixtureDef.isSensor = true; // Монета является сенсором
    fixtureDef.userData.pointer = COIN_USER_DATA;

    body->CreateFixture(&fixtureDef);
}

Coin::~Coin() {
    if (body) {
        body->GetWorld()->DestroyBody(body);
    }
}

void Coin::updateAnimation(float deltaTime) {
    if (collected) return; // Если монета собрана, анимация не обновляется

    elapsedTime += deltaTime;
    if (elapsedTime >= animationSpeed) {
        elapsedTime = 0.0f;
        currentFrame = (currentFrame + 1) % frameCount; // Переход к следующему кадру
        textureRect.left = currentFrame * frameWidth;   // Обновляем текстурный прямоугольник
        sprite.setTextureRect(textureRect);
    }
}

void Coin::draw(sf::RenderWindow& window) {
    if (!collected) {
        window.draw(sprite);
    }
}

void Coin::collect() {
    if (collected) return; // Защита от повторного сбора
    collected = true;
    sprite.setColor(sf::Color::Transparent); // Делаем монету невидимой
}

bool Coin::isCollected() const {
    return collected;
}

sf::FloatRect Coin::getBoundingBox() const {
    return sprite.getGlobalBounds();
}