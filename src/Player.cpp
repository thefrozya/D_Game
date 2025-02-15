#include "Player.h"
#include "Constants.h"
#include "ContactListener.h"
#include <iostream>

// Конструктор
Player::Player(b2World& world, float x, float y, ContactListener* contactListener, const sf::Texture& runTexture, const sf::Texture& jumpTexture, const sf::Texture& deathTexture)
    : world(world), contactListener(contactListener), runTexture(runTexture), jumpTexture(jumpTexture), deathTexture(deathTexture), isJumping(false), _isDying(false), _isWaitingForRespawn(false), health(100), spawnPoint(x, y) {

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
    shape.SetAsBox(ScaleCharacterX / 3.2f, ScaleCharacterY / 2.6f);
    b2FixtureDef fixtureDef;
    fixtureDef.shape = &shape;
    fixtureDef.density = 1.5f;
    fixtureDef.friction = 1.0f;
    fixtureDef.restitution = 0.1f;
    fixtureDef.userData.pointer = PLAYER_USER_DATA;
    body->CreateFixture(&fixtureDef);

    // Настройка спрайта
    sprite.setTexture(runTexture);
    sprite.setOrigin(frameWidth / 2.0f, frameHeight / 2.0f);
}

// Деструктор
Player::~Player() {
    if (body) {
        world.DestroyBody(body);
        body = nullptr;
    }
}

sf::FloatRect Player::getBoundingBox() const {
    b2Vec2 position = body->GetPosition();
    return sf::FloatRect(
        position.x * SCALE - sprite.getOrigin().x,
        position.y * SCALE - sprite.getOrigin().y,
        sprite.getTexture()->getSize().x,
        sprite.getTexture()->getSize().y
    );
}

void Player::takeDamage(int damage) {
    health -= damage;
    if (health < 0) health = 0; // Здоровье не может быть меньше 0
    if (health == 0) {
        _isDying = true; // Игрок начинает анимацию смерти
        _isWaitingForRespawn = false; // Сбрасываем флаг ожидания ввода
    }
    std::cout << "Player took damage! Current health: " << health << std::endl;
}

bool Player::isDead() const {
    return health <= 0; // Игрок мёртв, если здоровье <= 0
}

void Player::respawn(float x, float y) {
    health = 100;
    b2Vec2 position(x, y);
    body->SetTransform(position, body->GetAngle());
    _isDying = false;
    _isWaitingForRespawn = false;
}

int Player::getHealth() const {
    return health;
}



// Метод для загрузки текстуры и создания маски коллизии
void Player::loadTextureAndCreateCollisionMask() {
    sf::Image image = runTexture.copyToImage();
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

    const int FRAME_COUNT_RUNNING = 8; // Количество кадров бега
    const int FRAME_COUNT_JUMPING = 8; // Количество кадров прыжка
    const int FRAME_COUNT_DEATH = 8; // Количество кадров смерти
    int frameWidth = runTexture.getSize().x / FRAME_COUNT_RUNNING;
    int frameHeight = runTexture.getSize().y;

    // Загрузка кадров бега
    for (int i = 0; i < FRAME_COUNT_RUNNING; ++i) {
        framesRunning.push_back(sf::IntRect(i * frameWidth, 0, frameWidth, frameHeight));
    }

    // Загрузка кадров прыжка из отдельного спрайтшита
    frameWidth = jumpTexture.getSize().x / FRAME_COUNT_JUMPING;
    for (int i = 0; i < FRAME_COUNT_JUMPING; ++i) {
        framesJumping.push_back(sf::IntRect(i * frameWidth, 0, frameWidth, frameHeight));
    }

    // Загрузка кадров смерти из отдельного спрайтшита
    frameWidth = deathTexture.getSize().x / FRAME_COUNT_DEATH;
    for (int i = 0; i < FRAME_COUNT_DEATH; ++i) {
        framesDeath.push_back(sf::IntRect(i * frameWidth, 0, frameWidth, frameHeight));
    }

    currentFrame = framesRunning[0];
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
    if (_isDying) {
        updateAnimation(deltaTime);
    } else if (!_isWaitingForRespawn) {
        b2Vec2 positionB2 = body->GetPosition();
        sf::Vector2f position(positionB2.x * SCALE, positionB2.y * SCALE);
        sprite.setPosition(position);

        b2Vec2 velocity = body->GetLinearVelocity();
        if (!isRunning) {
            velocity.x *= (1.0f - PLAYER_SLOWDOWN_FACTOR * deltaTime);
            body->SetLinearVelocity(velocity);
        }

        updateAnimation(deltaTime);
    }
}

// Метод для обновления анимации
void Player::updateAnimation(float deltaTime) {
    if (_isDying) {
        sprite.setTexture(deathTexture); // Используем текстуру смерти
        animationTimer += deltaTime;
        if (animationTimer >= animationSpeed) {
            animationTimer = 0.0f;
            static int currentFrameIndex = 0;
            currentFrameIndex = (currentFrameIndex + 1) % framesDeath.size();
            currentFrame = framesDeath[currentFrameIndex];
            sprite.setTextureRect(currentFrame);
            if (currentFrameIndex == framesDeath.size() - 1) {
                // Завершение анимации смерти
                _isDying = false;
                _isWaitingForRespawn = true; // Ждем ввода для возрождения
            }
        }
    } else if (_isWaitingForRespawn) {
        // Не обновляем анимацию, ждем ввода
    } else if (isJumping) {
        sprite.setTexture(jumpTexture); // Используем текстуру прыжка
        animationTimer += deltaTime;
        if (animationTimer >= animationSpeed) {
            animationTimer = 0.0f;
            static int currentFrameIndex = 0;
            currentFrameIndex = (currentFrameIndex + 1) % framesJumping.size();
            currentFrame = framesJumping[currentFrameIndex];
            sprite.setTextureRect(currentFrame);
        }
    } else if (isRunning) {
        sprite.setTexture(runTexture); // Используем текстуру бега
        animationTimer += deltaTime;
        if (animationTimer >= animationSpeed) {
            animationTimer = 0.0f;
            static int currentFrameIndex = 0;
            currentFrameIndex = (currentFrameIndex + 1) % framesRunning.size();
            currentFrame = framesRunning[currentFrameIndex];
            sprite.setTextureRect(currentFrame);
        }
    } else {
        sprite.setTexture(runTexture); // Используем текстуру бега
        currentFrame = framesRunning[1]; // Используем первый кадр бега как статическое изображение
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
    if (_isDying || _isWaitingForRespawn) {
        // Не обрабатываем ввод во время смерти и ожидании ввода
        return;
    }

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
        velocity.y = PLAYER_JUMP_IMPULSE;
        body->SetLinearVelocity(velocity);
        isJumping = true;
    } else if (!contactListener->isOnGround()) {
        isJumping = true;
    } else {
        isJumping = false;
    }

    body->SetLinearVelocity(velocity);
}

// Метод для отрисовки игрока
void Player::draw(sf::RenderWindow& window) {
    window.draw(sprite);
}

// Метод для получения позиции игрока
sf::Vector2f Player::getPosition() const {
    b2Vec2 position = body->GetPosition();
    return sf::Vector2f(position.x, position.y);
}

Player& Player::operator=(Player&& other) noexcept {
    if (this != &other) {
        body = other.body;
        runTexture = std::move(other.runTexture);
        jumpTexture = std::move(other.jumpTexture);
        deathTexture = std::move(other.deathTexture);
        sprite = std::move(other.sprite);
        isRunning = other.isRunning;
        isJumping = other.isJumping;
        _isDying = other._isDying;
        _isWaitingForRespawn = other._isWaitingForRespawn;
        contactListener = other.contactListener;
        collisionPixels = std::move(other.collisionPixels);
        currentFrame = other.currentFrame;
        framesRunning = std::move(other.framesRunning);
        framesJumping = std::move(other.framesJumping);
        framesDeath = std::move(other.framesDeath);
        animationTimer = other.animationTimer;
        animationSpeed = other.animationSpeed;
        facingRight = other.facingRight;
        health = other.health;
        spawnPoint = other.spawnPoint;
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