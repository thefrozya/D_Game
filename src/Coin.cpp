#include "Coin.h"
#include "Constants.h"
#include <iostream>

Coin::Coin(b2World& world, float x, float y, sf::Texture& texture, int frameCount, float animationSpeed)
    :body(nullptr),shouldDestroyBody(true), frameCount(frameCount), animationSpeed(animationSpeed), currentFrame(0), elapsedTime(0.0f), collected(false) {
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

    b2BodyDef bodyDef;
    bodyDef.type = b2_staticBody;
    bodyDef.position.Set(x, y);
    std::cout << "Attempting to create body at (" << x << ", " << y << "), world body count: " << world.GetBodyCount() << std::endl;
    body = world.CreateBody(&bodyDef);
    std::cout << "Body created successfully, new body count: " << world.GetBodyCount() << std::endl;
    body->GetUserData().pointer = reinterpret_cast<uintptr_t>(this);


    b2CircleShape shape;
    shape.m_radius = 0.3f; // Радиус монеты

    b2FixtureDef fixtureDef;
    fixtureDef.shape = &shape;
    fixtureDef.isSensor = true; // Монета является сенсором
    

    body->CreateFixture(&fixtureDef);
}

b2Body* Coin::getBody() const {
    return body;
}

void Coin::markForNoDestruction() {
    shouldDestroyBody = false;
}
Coin::~Coin() {
    if (body) std::cout << "Coin destructor: body still exists" << std::endl;
}

void Coin::updateAnimation(float deltaTime) {
    if (collected) return;
    elapsedTime += deltaTime;
    if (elapsedTime >= animationSpeed) {
        elapsedTime = -0.05f;
        currentFrame = (currentFrame + 1) % frameCount;
        textureRect.left = currentFrame * frameWidth;
        sprite.setTextureRect(textureRect);
    }
}

void Coin::draw(sf::RenderWindow& window) {
    if (!collected) {
        //std::cout << "Drawing coin at (" << sprite.getPosition().x << ", " << sprite.getPosition().y << ")" << std::endl;
        window.draw(sprite);
    } else {
        //std::cout << "Coin at (" << sprite.getPosition().x << ", " << sprite.getPosition().y << ") is collected, not drawing" << std::endl;
    }
}

void Coin::collect(int score) {
    if (collected) {
        std::cout << "Coin at (" << sprite.getPosition().x << ", " << sprite.getPosition().y << ") already collected" << std::endl;
        return;
    }
    collected = true;
    sprite.setColor(sf::Color::Transparent);
    std::cout << "Coin collected at (" << sprite.getPosition().x << ", " << sprite.getPosition().y << ")" << std::endl;
}

bool Coin::isCollected() const {
    return collected;
}

sf::FloatRect Coin::getBoundingBox() const {
    return sprite.getGlobalBounds();
}

void Coin::destroyBody(b2World& world) {
    if (body && shouldDestroyBody) {
        std::cout << "Destroying coin body at (" << body->GetPosition().x << ", " << body->GetPosition().y << ")" << std::endl;
        world.DestroyBody(body);
        body = nullptr;
        shouldDestroyBody = false;
    }
}