#include "Player.h"
#include "Constants.h"
#include "ContactListener.h"
#include <iostream>
#include "Enemy.h"
#include "Staircase.h"


Player::Player() 
    : world(*(b2World*)nullptr), // Не создаём мир
      contactListener(nullptr), 
      body(nullptr), 
      velocity(0.0f), 
      isRunning(false), 
      isJumping(false), 
      _isDying(false), 
      _isWaitingForRespawn(false), 
      health(0), 
      damageCooldown(0.0f), 
      damageTimer(0.0f), 
      isInvulnerable(false), 
      spawnPoint(0.0f, 0.0f), 
      shouldDestroyBody(false) {
    std::cout << "Default Player constructor called" << std::endl;
}
// Конструктор
Player::Player(b2World& world, float x, float y, ContactListener* contactListener, const sf::Texture& runTexture, const sf::Texture& jumpTexture, const sf::Texture& deathTexture)
    : world(world), contactListener(contactListener), runTexture(runTexture), jumpTexture(jumpTexture), deathTexture(deathTexture), 
    isJumping(false), _isDying(false), _isWaitingForRespawn(false), health(100), spawnPoint(x, y),damageCooldown(0.2f), damageTimer(0.0f), isInvulnerable(false)  {

        std::cout << "Constructing Player at: " << this << std::endl;

    std::cout << "Initializing player at position: (" << x << ", " << y << ")" << std::endl;

    // Настройка физического тела
    b2BodyDef bodyDef;
    bodyDef.type = b2_dynamicBody;
    bodyDef.position.Set(x, y);
    body = world.CreateBody(&bodyDef);
    body->GetUserData().pointer = reinterpret_cast<uintptr_t>(this); // Сохраняем указатель на объект
    body->SetFixedRotation(true);

    // Загрузка текстуры и создание маски коллизии
    loadTextureAndCreateCollisionMask();

    // Создание фикстуры
    const float frameWidth = 32.0f;
    const float frameHeight = 32.0f;
    const float ScaleCharacterX = frameWidth / SCALE;
    const float ScaleCharacterY = frameHeight / SCALE;
    b2PolygonShape shape;
    shape.SetAsBox(ScaleCharacterX / 4.2f, ScaleCharacterY / 3.2f);
    b2FixtureDef fixtureDef;
    fixtureDef.shape = &shape;
    fixtureDef.density = 1.0f;
    fixtureDef.friction = 1.0f;
    fixtureDef.restitution = 0.1f;
    body->CreateFixture(&fixtureDef);

    // Настройка спрайта
    sprite.setTexture(runTexture);
    sprite.setOrigin(frameWidth / 2.0f, frameHeight / 2.0f);

    // Загрузка звука прыжка
    if (!jumpSoundBuffer.loadFromFile("assets/music/jumpCharacter.ogg")) { 
            std::cerr << "Failed to load jump sound!" << std::endl;
            throw std::runtime_error("Jump sound loading failed");
    }
    
    jumpSound.setBuffer(jumpSoundBuffer); // Присваиваем буфер звуку
}


void Player::markForNoDestruction() {
    shouldDestroyBody = false;
}

void Player::addKey(const std::string& keyType) {
    collectedKeys.insert(keyType);
    std::cout << "Key added: " << keyType << ", total keys: " << collectedKeys.size() << std::endl;
}

bool Player::hasKey(const std::string& keyType) const {
    return collectedKeys.find(keyType) != collectedKeys.end();
}

// Деструктор
Player::~Player() {
    // НЕ вызываем destroyBody здесь
    if (body) {
        std::cout << "Player destructor: body still exists at " << body << ", should be nullptr" << std::endl;
    }
}

void Player::destroyBody(b2World& world) {
    if (body && shouldDestroyBody) {
        world.DestroyBody(body);
        body = nullptr; // Обязательно обнуляем указатель
    }
}

sf::FloatRect Player::getBoundingBox() const {
    b2Vec2 position = body->GetPosition();
    sf::FloatRect bounds = sprite.getGlobalBounds();
    sf::FloatRect result(
        position.x * SCALE - sprite.getOrigin().x,
        position.y * SCALE - sprite.getOrigin().y,
        bounds.width,
        bounds.height
    );
    /*std::cout << "Player bounds: (" << result.left << ", " << result.top << ", " 
              << result.width << ", " << result.height << ")" << std::endl;*/
    return result;
}

void Player::takeDamage(int damage, b2Vec2 damageSourcePosition) {
    if (!this) {
        std::cerr << "Error: Player object is null!" << std::endl;
        return;
    }
    if (!isInvulnerable) {
        health -= damage;
        if (health < 0) health = 0;
        if (health == 0) {
            _isDying = true;
            _isWaitingForRespawn = false;
        }
        std::cout << "Player took damage! Current health: " << health << std::endl;
    }

    std::cout << "Damage source position: (" << damageSourcePosition.x << ", " << damageSourcePosition.y << ")" << std::endl;
    if (damageSourcePosition != b2Vec2(0.0f, 0.0f)) {
        b2Vec2 playerPosition = body->GetPosition();
        b2Vec2 direction = playerPosition - damageSourcePosition;
        direction.Normalize();

        float impulseStrength = 20.0f; // Увеличиваем силу
        b2Vec2 impulse = impulseStrength * direction;
        std::cout << "Applying impulse: (" << impulse.x << ", " << impulse.y << ")" << std::endl;
        body->ApplyLinearImpulse(impulse, body->GetWorldCenter(), true);
    } else {
        std::cout << "No impulse applied: damage source is (0, 0)" << std::endl;
    }

    isInvulnerable = true;
    damageTimer = 0.0f;
}


bool Player::isDead() const {
    std::cout << "Checking if player is dead..." << std::endl;
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

    b2Vec2 velocity = body->GetLinearVelocity();

    if (!body) {
        std::cout << "Player body is null, skipping update" << std::endl;
        return; // Ничего не делаем, если тело не создано
    }
    // Обновляем таймер задержки
    if (isInvulnerable) {
        damageTimer += deltaTime;
        if (damageTimer >= damageCooldown) {
            isInvulnerable = false; // Игрок снова становится уязвимым
            damageTimer = 0.0f;     // Сбрасываем таймер
        }
    }

        if (onStaircase) {
            body->SetGravityScale(0.0f);
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) velocity.x = -5.0f;
            else if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) velocity.x = 5.0f;
            else velocity.x = 0.0f;
        }
        else {
            body->SetGravityScale(1.0f);
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) velocity.x = -5.0f;
            else if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) velocity.x = 5.0f;
            else velocity.x = 0.0f;
        }
    

    // Остальная логика обновления...
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
   // Подсвечиваем красным, если в лаве
        if (inLava) {
            sprite.setColor(sf::Color::Red);
        } else {
            sprite.setColor(sf::Color::White);
        }
    
}

void Player::bounce() {
    // Получаем текущую скорость игрока
    b2Vec2 velocity = body->GetLinearVelocity();

    // Применяем силу отскока (например, вверх)
    velocity.y = -BOUNCE_FORCE;

    // Устанавливаем новую скорость телу Box2D
    body->SetLinearVelocity(velocity);
}


// Метод для воспроизведения звука прыжка
void Player::playJumpSound() {
    if (jumpSound.getStatus() != sf::SoundSource::Playing) { // Проверяем, играет ли уже звук
        jumpSound.play();
    }
}
void Player::stopPlayJumpSound(){
    if(jumpSound.getStatus() == sf::SoundSource::Playing) { // Проверяем, играет ли музыка
        jumpSound.stop(); // Останавливаем воспроизведение
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
        sprite.setScale(0.8f, 0.8f);
    } else {
        sprite.setScale(-0.8f, 0.8f);
    }
}

// Метод для обработки ввода
void Player::handleInput() {
    if (_isDying || _isWaitingForRespawn || !body) {
        std::cout << "Player body is null, skipping update" << std::endl;
        return;
    }

    b2Vec2 velocity = body->GetLinearVelocity();

    if (onStaircase) {
        body->SetGravityScale(0.0f);
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::W) || sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
            velocity.y = -2.0f;
            isJumping = false;
        }
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::S) || sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
            velocity.y = 2.0f;
            isJumping = false;
        }
        else {
            velocity.y = 0.0f;
        }
    }
    else {
        body->SetGravityScale(1.0f);
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space) && contactListener->isOnGround()) {
            velocity.y = PLAYER_JUMP_IMPULSE;
            isJumping = true;
            playJumpSound();
        }
        else if (!contactListener->isOnGround()) {
            isJumping = true;
        }
        else {
            isJumping = false;
            stopPlayJumpSound();
        }
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
        velocity.x = std::min(velocity.x + PLAYER_RUN_FORCE, PLAYER_MAX_SPEED);
        isRunning = true;
        facingRight = true;
    }
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
        velocity.x = std::max(velocity.x - PLAYER_RUN_FORCE, -PLAYER_MAX_SPEED);
        isRunning = true;
        facingRight = false;
    }
    else {
        velocity.x = 0.0f;
        isRunning = false;
    }

    body->SetLinearVelocity(velocity);
}

// Метод для отрисовки игрока
void Player::draw(sf::RenderWindow& window) {
    if (!body) {
        std::cout << "Player body is null, skipping draw" << std::endl;
        return;
    }
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