#include "Coin.h"
#include <Box2D/Box2D.h>
#include "Constants.h"

Coin::Coin(b2World& world, float x, float y, const sf::Texture& texture, int frameCount, float animationSpeed)
    : animationSpeed(animationSpeed) {
    sprite.setTexture(texture);
    sprite.setPosition(x * SCALE, y * SCALE); // Устанавливаем позицию спрайта

    // Разделяем спрайт-лист на кадры
    sf::Vector2u textureSize = texture.getSize();
    int frameWidth = textureSize.x / frameCount;
    for (int i = 0; i < frameCount; ++i) {
        frames.emplace_back(i * frameWidth, 0, frameWidth, textureSize.y);
    }

    sprite.setTextureRect(frames[0]); // Устанавливаем первый кадр
    sprite.setOrigin(frameWidth / 2.0f, textureSize.y / 2.0f); // Центрируем спрайт

    // Создание физического тела для монеты
    b2BodyDef bodyDef;
    bodyDef.type = b2_staticBody;
    bodyDef.position.Set(x, y);
    body = world.CreateBody(&bodyDef); // Используем переданный world
    body->GetUserData().pointer = reinterpret_cast<uintptr_t>(this); // Устанавливаем UserData

    b2PolygonShape shape;
    shape.SetAsBox(frameWidth / 2.0f / SCALE, textureSize.y / 2.0f / SCALE);

    b2FixtureDef fixtureDef;
    fixtureDef.shape = &shape;
    fixtureDef.isSensor = true; // Монета не должна влиять на физику

    body->CreateFixture(&fixtureDef);
}

void Coin::draw(sf::RenderWindow& window) {
    if (!collected) { // Отрисовываем только если монета не собрана
        window.draw(sprite);
    }
}

bool Coin::isCollected() const {
    return collected;
}

void Coin::collect() {
    collected = true;
}

sf::FloatRect Coin::getBoundingBox() const {
    return sprite.getGlobalBounds();
}

void Coin::updateAnimation(float deltaTime) {
    if (collected) return; // Не обновляем анимацию, если монета собрана

    animationTimer += deltaTime;
    if (animationTimer >= animationSpeed) {
        animationTimer = 0.0f;
        currentFrame = (currentFrame + 1) % frames.size(); // Переключаем кадр
        sprite.setTextureRect(frames[currentFrame]);
    }
}