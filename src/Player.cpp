#include "Player.h"
#include "Constants.h"
#include "ContactListener.h"
#include <iostream>

// Конструктор
Player::Player(b2World& world, float x, float y, ContactListener* contactListener) : world(&world), contactListener(contactListener) {
    std::cout << "Initializing player at position: (" << x << ", " << y << ")" << std::endl;
    // Настройка физического тела
    b2BodyDef bodyDef;
    bodyDef.type = b2_dynamicBody;
    bodyDef.position.Set(x, y); // Начальная позиция в метрах
    body = this->world->CreateBody(&bodyDef);
    // Устанавливаем пользовательские данные для игрока
    body->GetUserData().pointer = PLAYER_USER_DATA;
    // Отключаем вращение тела
    body->SetFixedRotation(true);
    // Загрузка текстуры и создание маски коллизии
    loadTextureAndCreateCollisionMask();
    // Вычисление размеров фикстуры
    const float frameWidth = 32.0f;  // Ширина одного кадра в пикселях
    const float frameHeight = 32.0f; // Высота одного кадра в пикселях
    const float ScaleCharacterX = frameWidth / SCALE;  // Размер фикстуры в метрах
    const float ScaleCharacterY = frameHeight / SCALE;
    if (ScaleCharacterX < 0.02f || ScaleCharacterY < 0.02f) {
        std::cerr << "Error: Fixture size is too small!" << std::endl;
        return;
    }
    // Создаем форму тела (прямоугольник)
    b2PolygonShape shape;
    shape.SetAsBox(ScaleCharacterX / 2.5f, ScaleCharacterY / 2.5f); // Центр фикстуры
    // Создаем фикстуру
    b2FixtureDef fixtureDef;
    fixtureDef.shape = &shape;
    fixtureDef.density = 1.0f; // Плотность для динамического тела
    fixtureDef.friction = 0.3f; // Трение
    fixtureDef.restitution = 0.1f; // Жесткость (отскок)
    body->CreateFixture(&fixtureDef);
    std::cout << "Player fixture created successfully." << std::endl;
    // Настройка спрайта
    sprite.setTexture(texture);
    sprite.setOrigin(frameWidth / 2.0f, frameHeight / 2.0f); // Центр спрайта
    std::cout << "Player sprite set successfully." << std::endl;
}

// Деструктор
Player::~Player() {
    if (body) {
        world->DestroyBody(body);
        body = nullptr;
    }
    std::cout << "Player destroyed successfully." << std::endl;
}

// Метод для загрузки текстуры и создания маски коллизии
void Player::loadTextureAndCreateCollisionMask() {
    // Загружаем текстуру
    if (!texture.loadFromFile("assets/textures/player_spritesheet.png")) {
        std::cerr << "Failed to load player texture from path: assets/textures/player_spritesheet.png" << std::endl;
        return;
    }
    std::cout << "Player texture loaded successfully." << std::endl;
    // Получаем пиксели текстуры
    sf::Image image = texture.copyToImage();
    unsigned int width = image.getSize().x;
    unsigned int height = image.getSize().y;
    // Создаем маску коллизии
    for (unsigned int y = 0; y < height; ++y) {
        for (unsigned int x = 0; x < width; ++x) {
            sf::Color pixel = image.getPixel(x, y);
            if (pixel.a > 0) { // Если пиксель непрозрачный
                collisionPixels.push_back(sf::Vector2f(x - width / 2.0f, y - height / 2.0f));
            }
        }
    }
    // Настройка анимации
    int frameWidth = texture.getSize().x / 8; // Ширина одного кадра (8 кадров в строке)
    int frameHeight = texture.getSize().y;   // Высота кадра равна высоте текстуры
    for (int i = 0; i < 8; ++i) { // 8 кадров анимации
        frames.push_back(sf::IntRect(i * frameWidth, 0, frameWidth, frameHeight));
    }
    currentFrame = frames[0]; // Устанавливаем первый кадр
    sprite.setTextureRect(currentFrame);
}

// Метод для проверки пиксельной коллизии
bool Player::checkPixelCollision(const std::vector<sf::Vector2f>& otherPixels, const sf::Vector2f& otherPosition) {
    sf::Vector2f playerPosition = getPosition(); // Позиция игрока в пикселях
    for (const auto& p1 : collisionPixels) {
        sf::Vector2f playerPixel = playerPosition + p1;
        for (const auto& p2 : otherPixels) {
            sf::Vector2f otherPixel = otherPosition + p2;
            if (std::abs(playerPixel.x - otherPixel.x) < 1.0f && std::abs(playerPixel.y - otherPixel.y) < 1.0f) {
                return true; // Коллизия обнаружена
            }
        }
    }
    return false; // Коллизия не обнаружена
}

// Метод для обновления позиции спрайта
void Player::update(float deltaTime) {
    // Обновляем позицию спрайта на основе физики
    b2Vec2 positionB2 = body->GetPosition();
    sf::Vector2f position(positionB2.x * SCALE, positionB2.y * SCALE);
    sprite.setPosition(position);
    // Логирование позиции игрока
    std::cout << "Physics position: (" << positionB2.x << ", " << positionB2.y << ") meters" << std::endl;
    std::cout << "Sprite position: (" << position.x << ", " << position.y << ") pixels" << std::endl;
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
    // Обновляем анимацию
    updateAnimation(deltaTime);
}

void Player::updateAnimation(float deltaTime) {
    if (isRunning) {
        animationTimer += deltaTime;
        if (animationTimer >= animationSpeed) {
            animationTimer = 0.0f;
            // Переход к следующему кадру
            static int currentFrameIndex = 0;
            currentFrameIndex = (currentFrameIndex + 1) % frames.size();
            currentFrame = frames[currentFrameIndex];
            sprite.setTextureRect(currentFrame);
        }
    } else {
        // Если игрок не бежит, показываем первый кадр
        currentFrame = frames[0];
        sprite.setTextureRect(currentFrame);
    }
    // Устанавливаем направление текстуры в зависимости от направления движения
    if (facingRight) {
        sprite.setScale(1.0f, 1.0f); // Возвращаем текстуру в исходное положение
    } else {
        sprite.setScale(-1.0f, 1.0f); // Переворачиваем текстуру влево
    }
}

void Player::handleInput() {
    b2Vec2 velocity = body->GetLinearVelocity();
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
        velocity.x = std::min(velocity.x + PLAYER_RUN_FORCE, PLAYER_MAX_SPEED);
        isRunning = true;
        facingRight = true; // Устанавливаем направление вправо
    } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
        velocity.x = std::max(velocity.x - PLAYER_RUN_FORCE, -PLAYER_MAX_SPEED);
        isRunning = true;
        facingRight = false; // Устанавливаем направление влево
    } else {
        isRunning = false;
    }
    // Проверяем, что игрок стоит на земле и нажата клавиша пробела
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space) && contactListener->isJumping) {
        velocity.y = PLAYER_JUMP_IMPULSE;
        contactListener->isJumping = true;
    }
    body->SetLinearVelocity(velocity);
}

void Player::draw(sf::RenderWindow& window) {
    window.draw(sprite);
    std::cout << "Player drawn." << std::endl;
}

sf::Vector2f Player::getPosition() const {
    b2Vec2 positionB2 = body->GetPosition();
    return sf::Vector2f(positionB2.x * SCALE, positionB2.y * SCALE);
}