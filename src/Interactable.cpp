#include "Interactable.h"
#include "Constants.h"
#include <iostream>

Interactable::Interactable(b2World& world, float x, float y, float width, float height, const std::string& type, bool isSensor)
    : type(type) {
    b2BodyDef bodyDef;
    bodyDef.type = b2_staticBody;
    bodyDef.position.Set(x + width / 2, y + height / 2);
    body = world.CreateBody(&bodyDef);

    b2PolygonShape box;
    box.SetAsBox(width / 2, height / 2);

    b2FixtureDef fixtureDef;
    fixtureDef.shape = &box;
    fixtureDef.isSensor = isSensor;
    body->CreateFixture(&fixtureDef);

    body->GetUserData().pointer = (type.find("Door") == 0) ? DOOR_USER_DATA : reinterpret_cast<uintptr_t>(this);

    shape.setSize(sf::Vector2f(width * SCALE, height * SCALE));
    shape.setPosition(x * SCALE, y * SCALE);
    shape.setFillColor(sf::Color::Transparent);
    shape.setOutlineColor(sf::Color::Magenta);
    shape.setOutlineThickness(1.0f);
}




void Interactable::destroyBody(b2World& world) {
    if (body) {
        world.DestroyBody(body);
        body = nullptr;
    }
}

// Key implementation
Key::Key(b2World& world, float x, float y, const sf::Texture& texture, const std::string& keyType)
    : Interactable(world, x, y, 0.5f, 0.5f, keyType, true),
      collected(false), frameCount(6), frameTime(0.1f), currentFrame(0), animationTimer(0.0f) {
    sprite.setTexture(texture);
    sprite.setTextureRect(sf::IntRect(0, 0, 16, 16));
    sprite.setPosition(x * SCALE, y * SCALE);
    sprite.setScale(SCALE / 32.0f, SCALE / 32.0f);
    std::cout << "Key created: " << keyType << " at (" << x << ", " << y << ")" << std::endl;
}

void Key::update(float deltaTime) {
    if (!collected) {
        animationTimer += deltaTime;
        if (animationTimer >= frameTime) {
            animationTimer -= frameTime;
            currentFrame = (currentFrame + 1) % frameCount;
            sprite.setTextureRect(sf::IntRect(currentFrame * 16, 0, 16, 16));
        }
    }
}

void Key::draw(sf::RenderWindow& window) {
    if (!collected) {
        window.draw(sprite);
    }
}
Key::~Key() {
    if (body) std::cout << "Key destructor: body still exists" << std::endl;
}

void Key::collect(int scoreValue) {
    collected = true;
    std::cout << "Key collected: " << type << " at (" << body->GetPosition().x << ", " << body->GetPosition().y << ")" << std::endl;
}

// Door implementation
Door::Door(b2World& world, float x, float y, float width, float height, const std::string& doorType, const sf::Texture& texture)
    : Interactable(world, x, y, width, height, doorType, true), isActivated(false), linkedDoor(nullptr),
      isAnimating(false), animationTimer(0.0f), currentFrame(0), frameCount(9), frameTime(0.1f) { // Обновлено frameCount на 9
    sprite.setTexture(texture);

    // Настройка кадров анимации (271 / 9 ≈ 30 пикселей на кадр)
    int baseFrameWidth = 30;
    for (int i = 0; i < frameCount - 1; ++i) {
        frames.push_back(sf::IntRect(i * baseFrameWidth, 0, baseFrameWidth, 32));
    }
    frames.push_back(sf::IntRect(8 * baseFrameWidth, 0, 31, 32)); // Последний кадр 31 пиксель

    // Устанавливаем origin в центр одного кадра текстуры (ширина кадра 30-31, высота 32)
    sprite.setOrigin(baseFrameWidth / 2.0f, 32 / 2.0f); // Центр: (15, 16)

    // Позиция теперь соответствует центру тела
    sprite.setPosition((x + width / 2.0f) * SCALE, (y + height / 2.0f) * SCALE);
    sprite.setScale(SCALE / 32.0f, SCALE / 32.0f); // Масштабируем под размер тайла

    // Устанавливаем начальный кадр в зависимости от типа двери
    if (doorType.find('L') != std::string::npos) {
        isActivated = true;
        currentFrame = frameCount - 1; // Последний кадр (8)
        sprite.setTextureRect(frames[currentFrame]);
        std::cout << "Exit Door created: " << doorType << " at (" << x << ", " << y << "), set to open state" << std::endl;
    } else {
        isActivated = false;
        sprite.setTextureRect(frames[0]); // Первый кадр для закрытого состояния
        std::cout << "Entry Door created: " << doorType << " at (" << x << ", " << y << "), set to closed state" << std::endl;
    }
}


Door::~Door() {
    if (body) std::cout << "Door destructor: body still exists" << std::endl;
}

void Door::draw(sf::RenderWindow& window) {
    window.draw(sprite); // Отрисовываем спрайт вместо прямоугольника
    if (DEBUG_DRAW_ENABLED) window.draw(shape); // Для отладки
}

void Door::update(float deltaTime) {
    if (isAnimating) {
        animationTimer += deltaTime;
        if (animationTimer >= frameTime) {
            animationTimer -= frameTime;
            currentFrame++;
            if (currentFrame >= frameCount) {
                currentFrame = frameCount - 1; // Останавливаемся на последнем кадре
                isAnimating = false; // Завершаем анимацию
            }
            sprite.setTextureRect(frames[currentFrame]);
        }
    }
}

void Door::activate() {
    if (!isActivated) {
        isActivated = true;
        startOpeningAnimation();
        std::cout << "Door activated and animation started: " << type << " at (" << body->GetPosition().x << ", " << body->GetPosition().y << ")" << std::endl;
    }
}

void Door::startOpeningAnimation() {
    isAnimating = true;
    currentFrame = 0;
    animationTimer = 0.0f;
}

// LinkDoors implementation remains unchanged
void Door::linkDoors(std::vector<Door>& doors) {
    for (auto& door : doors) {
        if (!door.isExit()) {
            std::string baseType = door.getType();
            if (baseType == "DoorsFive" || baseType == "DoorsFiveL") continue;
            std::string exitType = baseType + "L";
            for (auto& exitDoor : doors) {
                if (exitDoor.getType() == exitType) {
                    door.setLinkedDoor(&exitDoor);
                    break;
                }
            }
        }
    }
    Door* doorsLast = nullptr;
    Door* doorLowLast = nullptr;
    for (auto& door : doors) {
        if (door.getType() == "DoorsFive") doorsLast = &door;
        if (door.getType() == "DoorsFiveL") doorLowLast = &door;
    }
    if (doorsLast && doorLowLast) {
        doorsLast->setLinkedDoor(doorLowLast);
        std::cout << "Linked DoorsLast to DoorLowLast" << std::endl;
    }
}