#include "Player.h"
#include "Constants.h"
#include "ContactListener.h"
#include <iostream>

// Конструктор
Player::Player(b2World& world, float x, float y, ContactListener* contactListener)
    : world(world), contactListener(contactListener),isJumping(false) {  
    
        std::cout << "Initializing player at position: (" << x << ", " << y << ")" << std::endl;
    // Настройка физического тела
    b2BodyDef bodyDef;
    bodyDef.type = b2_dynamicBody;
    bodyDef.position.Set(x, y);
    body = world.CreateBody(&bodyDef);
    body->GetUserData().pointer = PLAYER_USER_DATA;

    body->SetFixedRotation(true);

    // Загрузка текстуры и создание маски коллизии
    loadTextureAndCreateCollisionMask();

    // Создание фикстуры
    const float frameWidth = 32.0f;
    const float frameHeight = 32.0f;
    const float ScaleCharacterX = frameWidth / SCALE;
    const float ScaleCharacterY = frameHeight / SCALE;

    b2PolygonShape shape;
    shape.SetAsBox(ScaleCharacterX / 2.5f, ScaleCharacterY / 2.5f);

    b2FixtureDef fixtureDef;
    fixtureDef.shape = &shape;
    fixtureDef.density = 1.0f;
    fixtureDef.friction = 0.3f;
    fixtureDef.restitution = 0.1f;
    fixtureDef.userData.pointer = PLAYER_USER_DATA;

    body->CreateFixture(&fixtureDef);

    // Настройка спрайта
    sprite.setTexture(texture);
    sprite.setOrigin(frameWidth / 2.0f, frameHeight / 2.0f);
}

// Деструктор
Player::~Player() {
    if (body) {
        world.DestroyBody(body);
        body = nullptr;
    }
}

// Метод для загрузки текстуры и создания маски коллизии
void Player::loadTextureAndCreateCollisionMask() {
    if (!texture.loadFromFile("assets/textures/player_spritesheet.png")) {
        std::cerr << "Failed to load player texture!" << std::endl;
        return;
    }

    sf::Image image = texture.copyToImage();
    unsigned int width = image.getSize().x;
    unsigned int height = image.getSize().y;

    for (unsigned int y = 0; y < height; ++y) {
        for (unsigned int x = 0; x < width; ++x) {
            sf::Color pixel = image.getPixel(x, y);
            if (pixel.a > 0) {
                collisionPixels.push_back(sf::Vector2f(x - width / 2.0f, y - height / 2.0f));
            }
        }
    }

    const int FRAME_COUNT = 8;
    int frameWidth = texture.getSize().x / FRAME_COUNT;
    int frameHeight = texture.getSize().y;

    for (int i = 0; i < FRAME_COUNT; ++i) {
        frames.push_back(sf::IntRect(i * frameWidth, 0, frameWidth, frameHeight));
    }

    currentFrame = frames[0];
    sprite.setTextureRect(currentFrame);
}

// Метод для проверки пиксельной коллизии
bool Player::checkPixelCollision(const std::vector<sf::Vector2f>& otherPixels, const sf::Vector2f& otherPosition) {
    sf::Vector2f playerPosition = getPosition();
    for (const auto& p1 : collisionPixels) {
        sf::Vector2f playerPixel = playerPosition + p1;
        for (const auto& p2 : otherPixels) {
            sf::Vector2f otherPixel = otherPosition + p2;
            if (std::abs(playerPixel.x - otherPixel.x) < 1.0f && std::abs(playerPixel.y - otherPixel.y) < 1.0f) {
                return true;
            }
        }
    }
    return false;
}

// Метод для обновления позиции спрайта
void Player::update(float deltaTime) {
    b2Vec2 positionB2 = body->GetPosition();
    sf::Vector2f position(positionB2.x * SCALE, positionB2.y * SCALE);
    sprite.setPosition(position);
    // Отладочная информация
    //std::cout << "Player sprite position updated to: (" << sprite.getPosition().x << ", " << sprite.getPosition().y << ")" << std::endl;

    b2Vec2 velocity = body->GetLinearVelocity();
    if (!isRunning) {
        velocity.x *= (1.0f - PLAYER_SLOWDOWN_FACTOR * deltaTime);
        body->SetLinearVelocity(velocity);
    }

    updateAnimation(deltaTime);
}

// Метод для обновления анимации
void Player::updateAnimation(float deltaTime) {
    if (isRunning) {
        animationTimer += deltaTime;
        if (animationTimer >= animationSpeed) {
            animationTimer = 0.0f;
            static int currentFrameIndex = 0;
            currentFrameIndex = (currentFrameIndex + 1) % frames.size();
            currentFrame = frames[currentFrameIndex];
            sprite.setTextureRect(currentFrame);
        }
    } else {
        currentFrame = frames[0];
        sprite.setTextureRect(currentFrame);
    }

    if (facingRight) {
        sprite.setScale(1.0f, 1.0f);
    } else {
        sprite.setScale(-1.0f, 1.0f);
    }
}

// Метод для обработки ввода
void Player::handleInput() {
    b2Vec2 velocity = body->GetLinearVelocity();
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
        velocity.x = std::min(velocity.x + PLAYER_RUN_FORCE, PLAYER_MAX_SPEED);
        isRunning = true;
        facingRight = true;
    } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
        velocity.x = std::max(velocity.x - PLAYER_RUN_FORCE, -PLAYER_MAX_SPEED);
        isRunning = true;
        facingRight = false;
    } else {
        isRunning = false;
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space) && contactListener->isOnGround()) {
        std::cout << "Attempting to jump..." << std::endl;

               // Устанавливаем вертикальную скорость для прыжка
        
        velocity.y = PLAYER_JUMP_IMPULSE;
        body->SetLinearVelocity(velocity);
        std::cout << "Jump applied!" << std::endl;
    }
    

    body->SetLinearVelocity(velocity);
}

// Метод для отрисовки игрока
void Player::draw(sf::RenderWindow& window) {
    window.draw(sprite);
}

// Метод для получения позиции игрока
sf::Vector2f Player::getPosition() const {
    b2Vec2 positionB2 = body->GetPosition();
    return sf::Vector2f(positionB2.x * SCALE, positionB2.y * SCALE);
}
Player& Player::operator=(Player&& other) noexcept {
    if (this != &other) {
        
        body = other.body;
        texture = std::move(other.texture);
        sprite = std::move(other.sprite);
        isRunning = other.isRunning;
        contactListener = other.contactListener;
        collisionPixels = std::move(other.collisionPixels);
        currentFrame = other.currentFrame;
        frames = std::move(other.frames);
        animationTimer = other.animationTimer;
        animationSpeed = other.animationSpeed;
        facingRight = other.facingRight;
        isJumping = other.isJumping;

        other.body = nullptr; // Предотвращаем удаление тела в деструкторе временного объекта
    }
    return *this;
}
// Реализация метода setIsJumping
void Player::setIsJumping(bool value) {
    isJumping = value;
}

// Реализация метода getIsJumping
bool Player::getIsJumping() const {
    return isJumping;
}